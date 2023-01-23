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

#ifndef __BOARDSUPPORT_H__
#define __BOARDSUPPORT_H__

#include "max32690_hal.h"
#include "config.h"

// Define the appropriate reset register selection, clock enable, DMA request selects, and
//  pin locations based on the SPI index selected in config.h
#if (CFG_SPI_INST == 0)
  #define SPI_RESET_BIT             MXC_SYS_RESET0_SPI0
  #define SPI_CLOCK_EN_BIT          MXC_SYS_PERIPH_CLOCK_SPI0
  #define SPI_RX_DMA_REQSEL         MXC_S_DMA_CTRL_REQUEST_SPI0RX
  #define SPI_TX_DMA_REQSEL         MXC_S_DMA_CTRL_REQUEST_SPI0TX
  #define CFG_SPI_MAIN_PINS_PORT    2
  #define CFG_SPI_SCLK_PIN_IDX      29
  #define CFG_SPI_MOSI_PIN_IDX      28
  #define CFG_SPI_MISO_PIN_IDX      27
  #define CFG_SPI_MAIN_PINS_AF_IDX  2
  #if (CFG_SPI_SSEL_IDX == 0)  
    #define CFG_SPI_SSEL_PIN_PORT   0
    #define CFG_SPI_SSEL_PIN_AF_IDX 1
    #define CFG_SPI_SSEL_PIN_IDX    22
  #elif (CFG_SPI_SSEL_IDX == 1)  
    #define CFG_SPI_SSEL_PIN_PORT   2
    #define CFG_SPI_SSEL_PIN_AF_IDX 2
    #define CFG_SPI_SSEL_PIN_IDX    26
  #else
    #error "SSEL index for SPI0 must be less than 2."
  #endif
#elif (CFG_SPI_INST == 1)
  #define SPI_RESET_BIT             MXC_SYS_RESET0_SPI1
  #define SPI_CLOCK_EN_BIT          MXC_SYS_PERIPH_CLOCK_SPI1
  #define SPI_RX_DMA_REQSEL         MXC_S_DMA_CTRL_REQUEST_SPI1RX
  #define SPI_TX_DMA_REQSEL         MXC_S_DMA_CTRL_REQUEST_SPI1TX
  #define CFG_SPI_MAIN_PINS_PORT    1
  #define CFG_SPI_SCLK_PIN_IDX      26
  #define CFG_SPI_MOSI_PIN_IDX      29
  #define CFG_SPI_MISO_PIN_IDX      28
  #define CFG_SPI_MAIN_PINS_AF_IDX  2
  #define CFG_SPI_SSEL_PIN_PORT     1
  #define CFG_SPI_SSEL_PIN_AF_IDX   1
  #if (CFG_SPI_SSEL_IDX == 0)  
    #define CFG_SPI_SSEL_PIN_IDX    23
  #elif (CFG_SPI_SSEL_IDX == 1)  
    #define CFG_SPI_SSEL_PIN_IDX    25
  #elif (CFG_SPI_SSEL_IDX == 2)  
    #define CFG_SPI_SSEL_PIN_IDX    24
  #else
    #error "SSEL index for SPI1 must be less than 3."
  #endif
#elif (CFG_SPI_INST == 2)
  #define SPI_RESET_BIT             MXC_SYS_RESET0_SPI2
  #define SPI_CLOCK_EN_BIT          MXC_SYS_PERIPH_CLOCK_SPI2
  #define SPI_RX_DMA_REQSEL         MXC_S_DMA_CTRL_REQUEST_SPI2RX
  #define SPI_TX_DMA_REQSEL         MXC_S_DMA_CTRL_REQUEST_SPI2TX
  #define CFG_SPI_MAIN_PINS_PORT    2
  #define CFG_SPI_SCLK_PIN_IDX      2
  #define CFG_SPI_MOSI_PIN_IDX      4
  #define CFG_SPI_MISO_PIN_IDX      3
  #define CFG_SPI_MAIN_PINS_AF_IDX  1
  #define CFG_SPI_SSEL_PIN_AF_IDX   1
  #if (CFG_SPI_SSEL_IDX == 0)  
    #define CFG_SPI_SSEL_PIN_PORT   2
    #define CFG_SPI_SSEL_PIN_IDX    5
  #elif (CFG_SPI_SSEL_IDX == 1)  
    #define CFG_SPI_SSEL_PIN_PORT   2
    #define CFG_SPI_SSEL_PIN_IDX    1
  #elif (CFG_SPI_SSEL_IDX == 2)  
    #define CFG_SPI_SSEL_PIN_PORT   1
    #define CFG_SPI_SSEL_PIN_IDX    27
  #else
    #error "SSEL index for SPI2 must be less than 3."
  #endif
#elif (CFG_SPI_INST == 3)
  #define SPI_RESET_BIT             MXC_SYS_RESET0_SPI3
  #define SPI_CLOCK_EN_BIT          MXC_SYS_PERIPH_CLOCK_SPI3
  #define SPI_RX_DMA_REQSEL         MXC_S_DMA_CTRL_REQUEST_SPI3RX
  #define SPI_TX_DMA_REQSEL         MXC_S_DMA_CTRL_REQUEST_SPI3TX
  #define CFG_SPI_MAIN_PINS_PORT    0
  #define CFG_SPI_SCLK_PIN_IDX      16
  #define CFG_SPI_MOSI_PIN_IDX      21
  #define CFG_SPI_MISO_PIN_IDX      20
  #define CFG_SPI_MAIN_PINS_AF_IDX  1
  #define CFG_SPI_SSEL_PIN_PORT     0
  #define CFG_SPI_SSEL_PIN_AF_IDX   1
  #if (CFG_SPI_SSEL_IDX == 0)  
    #define CFG_SPI_SSEL_PIN_IDX    19
  #elif (CFG_SPI_SSEL_IDX == 1)  
    #define CFG_SPI_SSEL_PIN_IDX    13
  #elif (CFG_SPI_SSEL_IDX == 2)  
    #define CFG_SPI_SSEL_PIN_IDX    14
  #else
    #error "SSEL index for SPI3 must be less than 3."
  #endif
#elif (CFG_SPI_INST == 4)
  #define SPI_RESET_BIT             MXC_SYS_RESET0_SPI4
  #define SPI_CLOCK_EN_BIT          MXC_SYS_PERIPH_CLOCK_SPI4
  #define SPI_RX_DMA_REQSEL         MXC_S_DMA_CTRL_REQUEST_SPI4RX
  #define SPI_TX_DMA_REQSEL         MXC_S_DMA_CTRL_REQUEST_SPI4TX
  #define CFG_SPI_MAIN_PINS_PORT    1
  #define CFG_SPI_SCLK_PIN_IDX      3
  #define CFG_SPI_MOSI_PIN_IDX      1
  #define CFG_SPI_MISO_PIN_IDX      2
  #define CFG_SPI_MAIN_PINS_AF_IDX  1
  #define CFG_SPI_SSEL_PIN_PORT     1
  #define CFG_SPI_SSEL_PIN_AF_IDX   1
  #if (CFG_SPI_SSEL_IDX == 0)  
    #define CFG_SPI_SSEL_PIN_IDX    0
  #elif (CFG_SPI_SSEL_IDX == 1)  
    #define CFG_SPI_SSEL_PIN_IDX    6
  #elif (CFG_SPI_SSEL_IDX == 2)  
    #define CFG_SPI_SSEL_PIN_IDX    11
  #else
    #error "SSEL index for SPI3 must be less than 3."
  #endif
#else
  #error "SPI Instance Not Supported."
#endif

// A macro to convert a DMA channel number to an IRQn number
#define GetIRQnForDMAChannel(x) ((IRQn_Type)(((x) == 0 ) ? DMA0_IRQn  : \
                                             ((x) == 1 ) ? DMA1_IRQn  : \
                                             ((x) == 2 ) ? DMA2_IRQn  : \
                                             ((x) == 3 ) ? DMA3_IRQn  : \
                                             ((x) == 4 ) ? DMA4_IRQn  : \
                                             ((x) == 5 ) ? DMA5_IRQn  : \
                                             ((x) == 6 ) ? DMA6_IRQn  : \
                                             ((x) == 7 ) ? DMA7_IRQn  : \
                                             ((x) == 8 ) ? DMA8_IRQn  : \
                                             ((x) == 9 ) ? DMA9_IRQn  : \
                                             ((x) == 10) ? DMA10_IRQn : \
                                             ((x) == 11) ? DMA11_IRQn : \
                                             ((x) == 12) ? DMA12_IRQn : \
                                             ((x) == 13) ? DMA13_IRQn : \
                                             ((x) == 14) ? DMA14_IRQn : \
                                                           DMA15_IRQn))

/* Buffer for debug messages written to UART. */
extern char aDebugString[150u];
void common_Fail(char* FailureReason);
void common_Perf(char* InfoString);

#define DEBUG_MESSAGE(...) \
  do { \
    sprintf(aDebugString,__VA_ARGS__); \
    common_Perf(aDebugString); \
  } while(0)

#define DEBUG_RESULT(s,result,expected_value) \
  do { \
    if ((result) != (expected_value)) { \
      sprintf(aDebugString,"%s  %d", __FILE__,__LINE__); \
      common_Fail(aDebugString); \
      sprintf(aDebugString,"%s Error Code: 0x%08X\n\rFailed\n\r",(s),(result)); \
      common_Perf(aDebugString); \
      exit(0); \
    } \
  } while (0)


#endif // __BOARDSUPPORT_H__
