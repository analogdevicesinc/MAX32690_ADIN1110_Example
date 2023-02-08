/******************************************************************************
 * Copyright (C) 2023 Analog Devices, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * ANALOG DEVICES BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Analog Devices, Inc. shall
 * not be used except as stated in the Analog Devices, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses of trade
 * secrets, proprietary technology, copyrights, patents, trademarks, maskwork
 * rights, or any other form of intellectual property whatsoever. Analog
 * Devices, Inc. retains all ownership rights.
 ******************************************************************************/

#include "boardsupport.h"
#include "gpio.h"
#include "rtc.h"
#include "mxc_delay.h"
#include "led.h"
#include "hal.h"
#include "spi.h"
#include "dma.h"
#include "nvic_table.h"

char aDebugString[150u];

// A macro to convert a GPIO port number to an IRQn number
#define ADIN1110_INT_IRQn ((IRQn_Type)MXC_GPIO_GET_IRQ(CFG_INT_PIN_PORT))

// A macro to convert a SPI instance number to a SPI instance pointer
#define SPI_INST MXC_SPI_GET_SPI(CFG_SPI_INST)

// The GPIO configuration structure for the pin connected to the ADIN1110's reset pin.  See config.h to change the pin used.
static const mxc_gpio_cfg_t ResetPin = {
                                          MXC_GPIO_GET_GPIO(CFG_RESET_PIN_PORT),
                                          (1 << CFG_RESET_PIN_IDX),
                                          MXC_GPIO_FUNC_OUT,
                                          MXC_GPIO_PAD_NONE,
                                          (mxc_gpio_vssel_t)(MXC_GPIO_VSSEL_VDDIO + CFG_RESET_PIN_USE_VDDIOH)
                                       };

// The GPIO configuration structure for the external interrupt pin.  See config.h to change the pin used.
static const mxc_gpio_cfg_t IntPin = {
                                          MXC_GPIO_GET_GPIO(CFG_INT_PIN_PORT),
                                          (1 << CFG_INT_PIN_IDX),
                                          MXC_GPIO_FUNC_IN,
                                          MXC_GPIO_PAD_NONE,
                                          (mxc_gpio_vssel_t)(MXC_GPIO_VSSEL_VDDIO + CFG_INT_PIN_USE_VDDIOH)
                                       };

// The GPIO configuration structure for the selected SPI pins.  See config.h to change the SPI instance and pins used.
static const mxc_gpio_cfg_t SPIMainPins = {
                                          MXC_GPIO_GET_GPIO(CFG_SPI_MAIN_PINS_PORT),
                                          (1 << CFG_SPI_SCLK_PIN_IDX) | (1 << CFG_SPI_MOSI_PIN_IDX) | (1 << CFG_SPI_MISO_PIN_IDX),
                                          (mxc_gpio_func_t)(MXC_GPIO_FUNC_OUT + CFG_SPI_MAIN_PINS_AF_IDX),
                                          MXC_GPIO_PAD_NONE,
                                          (mxc_gpio_vssel_t)(MXC_GPIO_VSSEL_VDDIO + CFG_SPI_PINS_USE_VDDIOH)
                                       };

// The GPIO configuration structure for the SPI SSEL pin.  See config.h to change the SPI instance and pins used.
static const mxc_gpio_cfg_t SPISSelPin = {
                                          MXC_GPIO_GET_GPIO(CFG_SPI_SSEL_PIN_PORT),
                                          (1 << CFG_SPI_SSEL_PIN_IDX),
                                          (mxc_gpio_func_t)(MXC_GPIO_FUNC_OUT + CFG_SPI_SSEL_PIN_AF_IDX),
                                          MXC_GPIO_PAD_NONE,
                                          (mxc_gpio_vssel_t)(MXC_GPIO_VSSEL_VDDIO + CFG_SPI_PINS_USE_VDDIOH)
                                       };

static HAL_Callback_t int_cb = NULL;    // The callback function to use when an external interrupt occurs

static HAL_Callback_t spi_cb = NULL;    // The callback function to use when a SPI transaction completes
static void* spi_cb_data = NULL;        // Data to pass along with the callback function

static uint8_t* txBuff;                 // The transmit buffer to use with SPI transactions
static uint8_t* rxBuff;                 // The receive buffer to use with SPI transactions
static uint32_t txBytes;                // The number of bytes to send over SPI
static uint32_t rxBytes;                // The number of bytes to receive over SPI

static int txChannel;                   // The DMA channel assigned to SPI transmit
static int rxChannel;                   // The DMA channel assinged to SPI receive

static void setPriority(IRQn_Type IRQn, uint32_t preemptPriority, uint32_t subPriority)
{
    uint32_t priorityGroup = NVIC_GetPriorityGrouping();
    NVIC_SetPriority(IRQn, NVIC_EncodePriority(priorityGroup, preemptPriority, subPriority));
}

static void processSPI()
{
    // Clear interrupt flags
    SPI_INST->intfl |= MXC_F_SPI_INTFL_RX_THD | MXC_F_SPI_INTFL_MST_DONE;

    // Unload any SPI data that has come in
    while(rxBytes && (SPI_INST->dma & MXC_F_SPI_DMA_RX_LVL)) {
        *rxBuff++ = SPI_INST->fifo8[0];
        rxBytes--;
    }

    // Write any pending bytes out.
    while(txBytes && (((SPI_INST->dma & MXC_F_SPI_DMA_TX_LVL) >> MXC_F_SPI_DMA_TX_LVL_POS) < MXC_SPI_FIFO_DEPTH)) {
        SPI_INST->fifo8[0] = *txBuff++;
        txBytes--;
    }
}

static void SPI_IRQHandler()
{
    // Two SPI interrupts are enabled: "rx threshold crossed" and "spi complete".
    //  Both indicate there is SPI data in the RX FIFO ready to be consumed.
    processSPI();

    // If we've received all the bytes that were asked for, the SPI transaction
    //  is complete and we can call the registered callback function.
    if(!rxBytes) {
        if(spi_cb) {
            (*spi_cb)(spi_cb_data, 0, NULL);
        }
    }
}

static void DMA_IRQHandler()
{
    // The only DMA interrupt that is enabled is "transaction complete".  If
    //  this interrupt occurs it means the pending SPI transaction has completed.
  
    // Clear the DMA interrupt flag
    MXC_DMA->ch[rxChannel].status |= MXC_F_DMA_STATUS_CTZ_IF;
    
    // See if a SPI callback has been registered and call it.
    if(spi_cb) {
        (*spi_cb)(spi_cb_data, 0, NULL);
    }
}

static void IntOccurred(void* cbdata)
{
    // See if a callback for the interrupt has been registered and call it.
    if(int_cb) {
        (*int_cb)(cbdata, 0, NULL);
    }
}

uint32_t BSP_InitSystem()
{
    int res;

    // Configure the MAX32650's GPIO pin that is connected to the ADIN1110's RESET pin
    res = MXC_GPIO_Config(&ResetPin);
    if (res != E_NO_ERROR) {
        return res;
    }
    MXC_GPIO_OutSet(ResetPin.port, ResetPin.mask);

    // Initialize and start the MAX32650's RTC
    res = MXC_RTC_Init(0, 0);
    if(res != E_NO_ERROR) {
        return res;
    }
    MXC_RTC_Start();

    return 0;
}

void BSP_HWReset(bool set)
{
    MXC_GPIO_OutClr(ResetPin.port, ResetPin.mask);      // Drive the GPIO pin low
    MXC_Delay(BSP_ADIN1110_RESET_TIME_USEC);            // Wait the specfied amount of time
    MXC_GPIO_OutSet(ResetPin.port, ResetPin.mask);      // Drive the GPIO pin high
}
  
void BSP_delayMs(uint32_t delay)
{
    // Use MAX32650 API function for delay
    MXC_Delay(MSEC(delay));
}

uint32_t BSP_SysNow()
{
    uint32_t sec;
    uint32_t subsec;

    // Block until the time can be read.  (MXC_RTC_GetTime may return E_BUSY 
    //  when the subsecond count is rolling over and the sec register has not
    //  been incremented yet.)
    while(MXC_RTC_GetTime(&sec, &subsec) != E_NO_ERROR);
    
    // Convert sec and subsecond values to milliseconds
    return (sec * 1000) + ((subsec * 244) / 1000);
}
  
void BSP_HeartBeat()
{
    // MAX32650 board file has a convenience function for toggling on-board LEDs
    LED_Toggle(CFG_HEARTBEAT_LED_IDX);
}
  
uint32_t BSP_spi2_write_and_read(uint8_t *pBufferTx, uint8_t *pBufferRx, uint32_t nbBytes, bool useDma)
{
    // Pass the BSP function through to the HAL function. They are the same in this simple implementation.
    return HAL_SpiReadWrite(pBufferTx, pBufferRx, nbBytes, useDma);
}
  
uint32_t BSP_spi2_register_callback(ADI_CB const *pfCallback, void *const pCBParam)
{
    // Pass the BSP function through to the HAL function. They are the same in this simple implementation.
    return HAL_SpiRegisterCallback(pfCallback, pCBParam);
}
  
uint32_t BSP_RegisterIRQCallback(ADI_CB const *intCallback, void * hDevice)
{
    // Pass the BSP function through to the HAL function. They are the same in this simple implementation.
    return HAL_RegisterCallback(intCallback, hDevice);
}
  
void common_Fail(char* FailureReason)
{
    printf("Failed: %s\n", FailureReason);
}
  
void common_Perf(char* InfoString)
{
    printf("%s\n", InfoString);
}
  
uint32_t HAL_INT_N_EnableIRQ()
{
    // Use the CMSIS core function to manage interrupts
    NVIC_EnableIRQ(ADIN1110_INT_IRQn);
    return ADI_HAL_SUCCESS;
}
  
uint32_t HAL_INT_N_DisableIRQ()
{
    // Use the CMSIS core function to manage interrupts
    NVIC_DisableIRQ(ADIN1110_INT_IRQn);
    return ADI_HAL_SUCCESS;
}

uint32_t HAL_EnableIrq()
{
    // Use the CMSIS core function to manage interrupts
    NVIC_EnableIRQ(ADIN1110_INT_IRQn);
    return ADI_HAL_SUCCESS;
}

uint32_t HAL_DisableIrq()
{
    // Use the CMSIS core function to manage interrupts
    NVIC_DisableIRQ(ADIN1110_INT_IRQn);
    return ADI_HAL_SUCCESS;
}

uint32_t HAL_GetEnableIrq(void)
{
    // Use the CMSIS core function to manage interrupts
    return NVIC_GetEnableIRQ(ADIN1110_INT_IRQn);
}

uint32_t HAL_SetPendingIrq()
{
    // Use the CMSIS core function to manage interrupts
    NVIC_SetPendingIRQ(ADIN1110_INT_IRQn);
    return ADI_HAL_SUCCESS;
}

uint32_t HAL_SpiReadWrite(uint8_t *pBufferTx, uint8_t *pBufferRx, uint32_t nbBytes, bool useDma)
{
    SPI_INST->ctrl1 = (nbBytes << MXC_F_SPI_CTRL1_TX_NUM_CHAR_POS) |            // Configure the transfer size
                      (nbBytes << MXC_F_SPI_CTRL1_RX_NUM_CHAR_POS);
    if(useDma) {
        SPI_INST->dma |= (MXC_F_SPI_DMA_DMA_TX_EN | MXC_F_SPI_DMA_DMA_RX_EN);   // Enable DMA for SPI transmit and receive operations
        SPI_INST->inten &= ~(MXC_F_SPI_INTEN_RX_THD | MXC_F_SPI_INTEN_MST_DONE);// Disable SPI interrupts, we'll use the DMA interrupts for notificataion
        
        MXC_DMA->ch[txChannel].src = (uint32_t)pBufferTx;                       // Set up the source pointer for the transmit DMA
        MXC_DMA->ch[txChannel].cnt = nbBytes;                                   // Set up the number of bytes to transfer
        MXC_DMA->ch[txChannel].ctrl |= MXC_F_DMA_CTRL_EN;                       // Start the transmit DMA

        MXC_DMA->ch[rxChannel].dst = (uint32_t)pBufferRx;                       // Set up the destination pointer for the receive DMA
        MXC_DMA->ch[rxChannel].cnt = nbBytes;                                   // Set up the number of bytes to transfer
        MXC_DMA->ch[rxChannel].ctrl |= MXC_F_DMA_CTRL_EN;                       // Start the receive DMA
    } else {
        SPI_INST->dma &= ~(MXC_F_SPI_DMA_DMA_TX_EN | MXC_F_SPI_DMA_DMA_RX_EN);  // Disable DMA for SPI transmit and receive operations
        SPI_INST->inten |= MXC_F_SPI_INTEN_RX_THD | MXC_F_SPI_INTEN_MST_DONE;   // Enable SPI receive threshold and transaction complete interrupts

        txBytes = nbBytes;                                                      // Save the number of bytes to be transmitted
        txBuff = pBufferTx;                                                     // Save the transmit buffer pointer
        rxBytes = nbBytes;                                                      // Save the number of bytes to be received
        rxBuff = pBufferRx;                                                     // Save the receive buffer pointer
        processSPI();                                                           // Configure the SPI transaction
    }

    SPI_INST->ctrl0 |= MXC_F_SPI_CTRL0_START;                                   // Start the SPI transaction
    return ADI_HAL_SUCCESS;
}

uint32_t HAL_RegisterCallback(HAL_Callback_t const *intCallback, void *hDevice)
{
    // Save the callback information for later use.
    int_cb = (HAL_Callback_t)intCallback;
    MXC_GPIO_RegisterCallback(&IntPin, IntOccurred, hDevice);
    return ADI_HAL_SUCCESS;
}

uint32_t HAL_SpiRegisterCallback(HAL_Callback_t const *spiCallback, void * hDevice)
{
    // Save the callback information for later use.
    spi_cb = (HAL_Callback_t)spiCallback;
    spi_cb_data = hDevice;
    return ADI_HAL_SUCCESS;
}

uint32_t HAL_Init_Hook(void)
{
    MXC_SYS_Reset_Periph(SPI_RESET_BIT);                                        // Reset selected SPI block
    MXC_SYS_ClockEnable(SPI_CLOCK_EN_BIT);                                      // Enable clock to selected SPI block
    MXC_GPIO_Config(&SPIMainPins);                                              // Put appropriate GPIO pins in SPI mode
    MXC_GPIO_Config(&SPISSelPin);                                               // Put appropriate GPIO pins in SPI mode

    SPIMainPins.port->ds0 |= SPIMainPins.mask;                                  // Set strongest possible drive strength for the SPI pins
    SPIMainPins.port->ds1 |= SPIMainPins.mask;
    SPISSelPin.port->ds0 |= SPISSelPin.mask;                                      
    SPISSelPin.port->ds1 |= SPISSelPin.mask;
    SPI_INST->ctrl0 = (MXC_S_SPI_CTRL0_SS_ACTIVE_SS0 << CFG_SPI_SSEL_IDX) |     // Configure which SSEL pin is used
                      MXC_F_SPI_CTRL0_MST_MODE |                                // Select controller mode
                      MXC_F_SPI_CTRL0_EN;                                       // Enable SPI
    SPI_INST->ctrl2 = MXC_S_SPI_CTRL2_NUMBITS_8;                                // Perform 8-bit transfers
    SPI_INST->sstime = (1 << MXC_F_SPI_SSTIME_PRE_POS) |                        // Remove any delay time betweeen SSEL and SCLK edges
                       (1 << MXC_F_SPI_SSTIME_POST_POS) | 
                       (1 << MXC_F_SPI_SSTIME_INACT_POS);
    SPI_INST->clkctrl = (CFG_SPI_CLOCK_LOW << MXC_F_SPI_CLKCTRL_LO_POS) | 
                        (CFG_SPI_CLOCK_HIGH << MXC_F_SPI_CLKCTRL_HI_POS);       // Set the SPI clock rate
    SPI_INST->dma = MXC_F_SPI_DMA_RX_FIFO_EN |                                  // Enable the receive FIFO
                    MXC_F_SPI_DMA_TX_FIFO_EN |                                  // Enable the transmit FIFO
                    (CFG_SPI_RX_THRESHOLD << MXC_F_SPI_DMA_RX_THD_VAL_POS) |    // Configure the receive threshold that triggers another DMA transfer
                    (CFG_SPI_TX_THRESHOLD << MXC_F_SPI_DMA_TX_THD_VAL_POS);     // Configure the transmit treshold that triggers another DMA transfer
    SPI_INST->intfl = SPI_INST->intfl;                                          // Clear any interrupt flags that may already be set

    // Enable SPI interrupts
    MXC_NVIC_SetVector(MXC_SPI_GET_IRQ(MXC_SPI_GET_IDX(SPI_INST)), SPI_IRQHandler);
    setPriority(MXC_SPI_GET_IRQ(CFG_SPI_INST), 0, 0);
    NVIC_EnableIRQ(MXC_SPI_GET_IRQ(CFG_SPI_INST));

    // Intialize DMA and reserve a couple of channels
    if(MXC_DMA_Init() != E_NO_ERROR) {
        return ADI_HAL_ERROR;
    }
    txChannel = MXC_DMA_AcquireChannel();
    rxChannel = MXC_DMA_AcquireChannel();
    if((txChannel < 0) || (rxChannel < 0)) {
        return ADI_HAL_ERROR;
    }
    
    MXC_DMA->ch[txChannel].ctrl = MXC_F_DMA_CTRL_SRCINC |                       // Auto-increment the tx source buffer pointer during DMA transfers
                                  SPI_TX_DMA_REQSEL;                            // Set the DMA destination to the SPI's transmit FIFO
    MXC_DMA->ch[rxChannel].ctrl = MXC_F_DMA_CTRL_DIS_IE |                       // Enable the DMA finished interrupt for the receive channel
                                  MXC_F_DMA_CTRL_DSTINC |                       // Auto-increment the rx destination buffer pointer during DMA transfers
                                  SPI_RX_DMA_REQSEL;                            // Set the DMA source to the SPI's receive FIFO

    // Enable DMA interrupts.  (We are only enabling interrupts on the receive channel.  By
    //  the nature of SPI, if the receive operation is complete, the transmit operation is
    //  guaranteed to have already completed.)
    MXC_DMA->inten |= (1 << rxChannel);
    MXC_NVIC_SetVector(GetIRQnForDMAChannel(rxChannel), DMA_IRQHandler);
    setPriority(GetIRQnForDMAChannel(rxChannel), 0, 0);
    NVIC_EnableIRQ(GetIRQnForDMAChannel(rxChannel));

    // Prepare the pin that is attached to the ADIN1110's interrupt pin
    if(MXC_GPIO_Config(&IntPin) != E_NO_ERROR) {
        return ADI_HAL_ERROR;
    }
    if(MXC_GPIO_IntConfig(&IntPin, MXC_GPIO_INT_FALLING) != E_NO_ERROR) {
      return ADI_HAL_ERROR;
    }
    MXC_GPIO_EnableInt(IntPin.port, IntPin.mask);
    setPriority((IRQn_Type)MXC_GPIO_GET_IRQ(CFG_INT_PIN_PORT), 0xF, 0);
    NVIC_EnableIRQ((IRQn_Type)MXC_GPIO_GET_IRQ(CFG_INT_PIN_PORT));

    return ADI_HAL_SUCCESS;
}
