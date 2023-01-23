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

#ifndef __MAX32690_HAL_H__
#define __MAX32690_HAL_H__

#include <stdint.h>
#include <stdbool.h>
#include "hal.h"

/*!
* @brief Callback function definition for HAL.
*/
#define ADI_CB HAL_Callback_t

uint32_t BSP_InitSystem(void);

/*
 * @brief Hardware reset to DUT
 *
 * @param [in]      set - obosolete, keep it for consistancy with older versions
 * @param [out]     none
 * @return none
 *
 * @details Puld down Reset Pin, wait for 1mS release the Reset Pin
 *
 * @sa
 */
void BSP_HWReset(bool set);

/*
 * @brief Blocking delay function
 *
 * @param [in]      delay - delay in miliseconds
 * @param [out]     none
 * @return none
 *
 * @details Based on assumption that SysTick counter fires every milisecond
 *
 * @sa
 */
void BSP_delayMs(uint32_t delay);

uint32_t BSP_SysNow(void);
void BSP_HeartBeat(void);
uint32_t BSP_spi2_write_and_read(uint8_t *pBufferTx, uint8_t *pBufferRx, uint32_t nbBytes, bool useDma);
uint32_t BSP_spi2_register_callback(ADI_CB const *pfCallback, void *const pCBParam);
uint32_t BSP_RegisterIRQCallback(ADI_CB const *intCallback, void * hDevice);

uint32_t HAL_INT_N_EnableIRQ(void);
uint32_t HAL_INT_N_DisableIRQ(void);

/*!
 * @brief           Interrupt enable.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Enables the external interrupt (INT_N) in the host processor.
 *
 * @sa              HAL_DisableIrq()
 */
uint32_t HAL_EnableIrq(void);

/*!
 * @brief           Interrupt disable.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Disables the external interrupt (INT_N) in the host processor.
 *
 * @sa              HAL_EnableIrq()
 */
uint32_t HAL_DisableIrq(void);

/*!
 * @brief           Get interrupt enable status.
 *
 * @return          Enable status
 *                  - 1         Interrupt enabled.
 *                  - 0         Interrupt disabled.
 *
 * @details         Disables the external interrupt (INT_N) in the host processor.
 *
 * @sa
 */
uint32_t HAL_GetEnableIrq(void);

uint32_t HAL_SetPendingIrq(void);

/*!
 * @brief           HAL SPI read/write.
 *
 * @param [in]      pBufferTx   Pointer to transmit data buffer.
 * @param [in]      nbBytes     Number bytes to transfer.
 * @param [in]      useDma      Enable/disable DMA for SPI.
 * @param [out]     pBufferRx   Pointer to receive data buffer.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Initiate data transfer using the SPI interface.
 *                  Implements a non blocking call.
 *
 * @sa
 */
uint32_t HAL_SpiReadWrite(uint8_t *pBufferTx, uint8_t *pBufferRx, uint32_t nbBytes, bool useDma);

/*!
 * @brief           Register device IRQ callback function.
 *
 * @param [in]      intCallback     Callback function.
 * @param [in]      hDevice         Pointer to device handler.
 * 
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Registers a callback for the PHY interrupt.
 *
 * @sa
 */
uint32_t HAL_RegisterCallback(HAL_Callback_t const *intCallback, void *hDevice);

/*!
 * @brief           Register SPI IRQ callback function.
 *
 * @param [in]      spiCallback     Callback function.
 * @param [in]      hDevice         Pointer to device handler.
 * 
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Registers a callback for the SPI interrupt.
 *
 * @sa
 */
uint32_t HAL_SpiRegisterCallback(HAL_Callback_t const *spiCallback, void * hDevice);

/*!
 * @brief           HAL initialization hook.
 *
 * @return          Status
 *                  - #ADI_HAL_SUCCESS      Call completed successfully.
 *                  - #ADI_HAL_ERROR        Error encountered during function execution.
 *
 * @details         Called during the driver initialization, to allow the user to run application-specific
 *                  configuration, if needed. Can be left empty if such functionality is not required.
 *
 * @sa              HAL_UnInit_Hook()
 */
uint32_t HAL_Init_Hook(void);

#endif //__MAX32690_HAL_H__
