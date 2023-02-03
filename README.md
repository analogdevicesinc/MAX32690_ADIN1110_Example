# MAX32690 to ADIN1110 HAL Driver and Example

This repository provides hardware abstraction (HAL) code that interfaces the MAX32690 microcontroller to the ADIN1110 Ethernet MAC/PHY. A simple webserver example that demonstrates the use of the driver is also provided.

## Prerequisites
* Install [IAR's Embedded Workbench](https://www.iar.com/products/architectures/arm/iar-embedded-workbench-for-arm/).
* Install the [MAX32690 CMSIS pack file](https://www.mxim.net/microcontroller/pack/Maxim.MAX32690.1.2.1.pack) for IAR.  The pack file can also be installed via IAR's CMSIS-Pack Manager.
* Install the [ADIN1110 Non Operating System Driver](https://www.analog.com/en/license/licensing-agreement/adin1110.html).
* Obtain an [EVAL-ADIN1110EBZ board](https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/eval-adin1110.html).
* Obtain an [EVAL-ADIN1100EBZ board](https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/eval-adin1100.html).
* Obtain a MAX32690 EV Kit board.

## Running the Example

1. Connect the **+**, **-**, and **SH** signals on connector P101 of the EVAL-ADIN1110EBZ board to the **+**, **-**, and **SH** signals on connector P101 of the EVAL-ADIN1100EBZ board.
2. On the EVAL-ADIN1110EBZ board:
    * Move the shunt on jumper J301 to position 1-2 (GND) to hold the onboard microcontroller in reset.
    * Place all five switches of S201 in the OFF position.
3. On the EVAL-ADIN1100EBZ board:
    * Place switch #4 of S201 in the ON positions.  Place the remaining switches in the OFF position.
    * Place all four switches of S403 in the OFF position.
    * Connect an Ethernet cable (at P301) to a test network.
4. On the MAX32690 EV Kit remove resistors R73 through R76.  This disconnects the on-board audio codec that is on the same SPI bus used in this example.
5. Connect the MAX32690 EV Kit board to the EVAL-ADIN1110EBZ board.

| **Signal**          | **MAX32690 EV Kit Connector** | **VAL-ADIN1110EBZ Connector** |
|---------------------|:-----------------------------:|:-----------------------------:|
| Ground              | GND (JH5.7)                   | GND (P402.9 or P402.10)       |
| SCLK                | P2.29 (JH5.5)                 | SCLK (P402.7)                 |
| SSEL                | P2.26 (JH5.2)                 | CS_N (P402.1)                 |
| MOSI                | P2.28 (JH5.4)                 | MOSI (P402.3)                 |
| MISO                | P2.27 (JH5.3)                 | MISO (P402.5)                 |
| ADIN1110 INT        | P0.7 (JH1.2)                  | INT_T (P402.2)                |
| ADIN1110 RESET      | p0.8 (JH1.3)                  | RESET_N (P402.4)              |

6. Connect the CMSIS-DAP adapter to the MAX32690 EV Kit board (Connector J3).
7. Provide power to all boards.
8. Copy the contents of [this GitHub repository](https://github.com/Analog-Devices-MSDK/MAX32690_ADIN1110_Example) to the same location that the **ADIN1110 Non Operating System Driver** was installed.
9. Start IAR's Embedded Workbench
10. Select *File->Open Workspace* and open the <ADIN1110DeviceDriverInstall>\examples\adin1110\lwIPExample\MAX32690\lwIPExample.eww file.
11. Build, download, and run the lwIPExample project.
12. On the test network, open a browser and enter the IP address assigned to the ADIN1110. *(TODO: provide instructions for determining the IP address)*

## Files Included in This Repository
* [examples/adin1110/lwIPExample/MAX32690](./examples/adin1110/lwIPExample/MAX32690) - IAR specific project and workspace files
* [examples/bsp/MAX32690/boardsupport.h](./examples/bsp/MAX32690/boardsupport.h)
    - A top level include file that includes all other files required by the HAL.
    - Holds the implementations of the DEBUG print macros.
* [examples/bsp/MAX32690/config.h](./examples/bsp/MAX32690/config.h)
    - Configuration file for customizing the driver to a particular board design.
    - See [Customizing the MAX32690 Driver to Your Board](#customizing-the-hal-to-your-board).
* [examples/bsp/MAX32690/hal_port_specific.h](./examples/bsp/MAX32690/hal_port_specific.h)
    - A set of compiler/toolchain specific defines and macros used by the ADIN1110 code.
* [examples/bsp/MAX32690/max32690_hal.h](./examples/bsp/MAX32690/max32690_hal.h)
    - A set of defines and macros specific to the MAX32690.  If this HAL is ported to another of Analog Devices microcontrollers, these definitions will need to change.
    - Holds the function prototypes for all the functions required by the ADIN1110 Non Operating System Driver.
* [examples/bsp/MAX32690/max32690_hal.c](./examples/bsp/MAX32690/max32690_hal.c)
    - The C code implementation of the functions required by the ADIN1110 Non Operating System Driver.

## Customizing the HAL to Your Board
When building this HAL for a specific board, the code must be made aware of which connections and interface instances are used to connect the microcontroller to the ADIN1110.  This is handled by a set of definitions declared in the config.h file.  Change the valuje of the defines in this file to match the hardware connections.  The definitions are described in the following table.

| **Definition** | **Supported Values** | **Description** |
|----------------|:--------------------:|:---------------:|
| CFG_RESET_PIN_PORT           | [0-4]       | The MAX32690 GPIO port number that the ADIN1110's RESET pin is attached to. |
| CFG_RESET_PIN_IDX            | [0-31]      | The MAX32690 GPIO pin number that the ADIN1110's RESET pin is attached to. |
| CFG_RESET_PIN_USE_VDDIOH     | [0-1]       | The voltage level to use when driving the ADIN1110's RESET pin (0 = VDDIO, 1 = VDDIOH). |
| BSP_ADIN1110_RESET_TIME_USEC | [1-1000000] | The time, in microseconds, to hold the ADIN1110's RESET pin low when performing a reset. |
| CFG_HEARTBEAT_LED_IDX        | *Board Dependent* | The index of the board's LED used as the heartbeat LED (see 'led_pin' in board.c).  For the MAX32690, use a value of 0 for the D1 LED or a value of 1 for the D2 LED. |
| CFG_INT_PIN_PORT             | [0-3]       | The MAX32690 GPIO port number that the ADIN1110's INT pin is attached to. |
| CFG_INT_PIN_IDX              | [0-31]      | The MAX32690 GPIO pin number that the ADIN1110's INT pin is attached to. |
| CFG_INT_PIN_USE_VDDIOH       | [0-1]       | The voltage level to reference when reading the ADIN1110's INT pin (0 = VDDIO, 1 = VDDIOH). |
| CFG_SPI_INST                 | [0-4]       | The MAX32690 SPI instance that is connected to the ADIN1110's SPI interface. |
| CFG_SPI_PINS_USE_VDDIOH      | [0-1]       | The voltage level to use for the SPI interface (0 = VDDIO, 1 = VDDIOH). |
| CFG_SPI_SSEL_IDX             | [0-2]       | The MAX32690 SSEL index to use for SPI communications. |
| CFG_SPI_CLOCK_LOW*           | [0-255]     | The number of peripheral clocks to use for the SPI clock low time. |
| CFG_SPI_CLOCK_HIGH*          | [0-255]     | The number of peripheral clocks to use for the SPI clock high time. |
| CFG_SPI_RX_THRESHOLD         | [0-31]      | When the SPI RX FIFO count exceeds this value, an interrupt (or DMA transaction) will occur. |
| CFG_SPI_TX_THRESHOLD         | [0-31]      | When the SPI TX FIFO count equals or falls below this value, an interrupt (or DMA transaction) will occur. |

\* The SPI clock rate is determined by the following equations:\
SPI_CLOCK_RATE = PCLOCK_FREQ / (2 + CFG_SPI_CLOCK_LOW + CFG_SPI_CLOCK_HIGH)\
PCLOCK_FREQ = SYSTEM_CLOCK_FREQ / 2 for SPI instances 0-2.\
PCLOCK_FREQ = SYTEMM_CLOCK_FREQ for SPI instances 3-4.\
