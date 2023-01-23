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

#ifndef __HAL_PORT_SPECIFIC_H__
#define __HAL_PORT_SPECIFIC_H__

#include "mxc_device.h"

#define PRAGMA(x) _Pragma(#x)
#define ATTRIBUTE(x) __attribute__((x))

#if defined (__GNUC__)
  /* Gcc uses attributes */
  #define HAL_ALIGNED_PRAGMA(num)
  #define HAL_ALIGNED_ATTRIBUTE(num) ATTRIBUTE(aligned(num))
  #define HAL_UNUSED_ATTRIBUTE ATTRIBUTE(unused)
#elif defined ( __ICCARM__ )
  /* IAR uses a pragma */
  #define HAL_ALIGNED_ATTRIBUTE(num)
  #define HAL_ALIGNED_PRAGMA(num) PRAGMA(data_alignment=num)
  #define HAL_UNUSED_ATTRIBUTE
#elif defined (__CC_ARM)
  /* Keil uses a decorator which is placed in the same position as pragmas */
  #define HAL_ALIGNED_ATTRIBUTE(num)
  #define HAL_ALIGNED_PRAGMA(num) __align(##num)
  #define HAL_UNUSED_ATTRIBUTE ATTRIBUTE(unused)
#else
#error "Toolchain not supported"
#endif

#define DMA_BUFFER_ALIGN(var, alignBytes)   HAL_ALIGNED_PRAGMA(alignBytes) var HAL_ALIGNED_ATTRIBUTE(alignBytes)

/*! Minimum transaction size above which DMA is enabled for a SPI transaction. */
/*  Interrupt-based transactions are used when the transaction size is less than this threshold. */
#define MIN_SIZE_FOR_DMA            (32)

// TODO - what value should be used here?
/*! Duration of an MDIO read, in microseconds. This is used to convert timeout */
/*  values expressed in milliseconds, to a number of iterations when polling.  */
#define ADI_HAL_MDIO_READ_DURATION  (1250)


#endif // __HAL_PORT_SPECIFIC_H__
