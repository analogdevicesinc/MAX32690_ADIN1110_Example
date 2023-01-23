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

#ifndef __CONFIG_H__
#define __CONFIG_H__

// Configuration for the connection between the MAX32690 and the ADIN1110's RESET pin
#define CFG_RESET_PIN_PORT              0       // The MAX32690 GPIO port number
#define CFG_RESET_PIN_IDX               8       // The MAX32690 GPIO pin number
#define CFG_RESET_PIN_USE_VDDIOH        1       // The voltage level for the pin (0 = VDDIO, 1 = VDDIOH)

// The time, in microseconds, to hold the ADIN1110's RESET pin low when performing a reset
#define BSP_ADIN1110_RESET_TIME_USEC    1000

// The index of the board's LED used as the heartbeat LED (see 'led_pin' in board.c)
#define CFG_HEARTBEAT_LED_IDX           0

// Configuration for the connection between the MAX32690 and the ADIN1110's INT pin
#define CFG_INT_PIN_PORT                0       // The MAX32690 GPIO port number
#define CFG_INT_PIN_IDX                 7       // The MAX32690 GPIO pin number
#define CFG_INT_PIN_USE_VDDIOH          1       // The voltage level for the pin (0 = VDDIO, 1 = VDDIOH)

// Configuration for the SPI connection between the MAX32690 and the ADIN1110
#define CFG_SPI_INST                    0       // The SPI instance to use (0, 1, 2, 3, or 4)
#define CFG_SPI_PINS_USE_VDDIOH         1       // The voltage level for the SPI pins (0 = VDDIO, 1 = VDDIOH)
#define CFG_SPI_SSEL_IDX                1       // The SSEL index to use for SPI communications
#define CFG_SPI_CLOCK_LOW               1       // The number of peripheral clocks to use for the SPI clock low time
#define CFG_SPI_CLOCK_HIGH              1       // The number of peripheral clocks to use for the SPI clock high time
#define CFG_SPI_RX_THRESHOLD            0       // If the SPI RX FIFO count exceeds this value, an interrupt (pr DMA transaction) will occur
#define CFG_SPI_TX_THRESHOLD            31      // If the SPI TX FIFO count equals or falls below this value, an interrupt (or DMA transaction) will occur

#endif // __CONFIG_H__
