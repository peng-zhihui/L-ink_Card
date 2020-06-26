/**
 * @file    lpc43xx_scu.h
 * @brief   
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Peripheral group ----------------------------------------------------------- */
/** @defgroup SCU	SCU (System Control Unit)
 * @ingroup LPC4300CMSIS_FwLib_Drivers
 * @{
 */

#ifndef __SCU_H
#define __SCU_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Private macros ------------------------------------------------------------- */
/** @defgroup SCT_Private_Macros SCT Private Macros
 * @{
 */

/** Port offset definition */
#define PORT_OFFSET 	0x80
/** Pin offset definition */
#define PIN_OFFSET  	0x04

/* Pin mode defines, following partly a definition from older chip architectures */
#define MD_PUP  (0x0 << 3)
#define MD_BUK  (0x1 << 3)
#define MD_PLN  (0x2 << 3)
#define MD_PDN  (0x3 << 3)
#define MD_EHS  (0x1 << 5)
#define MD_EZI  (0x1 << 6)
#define MD_ZI   (0x1 << 7)
#define MD_EHD0 (0x1 << 8)
#define MD_EHD1 (0x1 << 9)
#define MD_EHD2 (0x3 << 8)
#define MD_PLN_FAST (MD_PLN | MD_EZI | MD_ZI | MD_EHS)


/* Pin mode defines, more in line with the definitions in the LPC1800/4300 user manual */
/* Defines for SFSPx_y pin configuration registers                                     */
#define PDN_ENABLE        (1 << 3)	// Pull-down enable
#define PDN_DISABLE       (0 << 3)      // Pull-down disable
#define PUP_ENABLE        (0 << 4)      // Pull-up enable
#define PUP_DISABLE       (1 << 4)	// Pull-up disable
#define SLEWRATE_SLOW	  (0 << 5)	// Slew rate for low noise with medium speed
#define SLEWRATE_FAST	  (1 << 5)	// Slew rate for medium noise with fast speed
#define INBUF_ENABLE	  (1 << 6)	// Input buffer
#define INBUF_DISABLE	  (0 << 6)	// Input buffer
#define FILTER_ENABLE	  (0 << 7)	// Glitch filter (for signals below 30MHz)
#define FILTER_DISABLE	  (1 << 7)	// No glitch filter (for signals above 30MHz)
#define DRIVE_8MA         (1 << 8)	// Drive strength of 8mA
#define DRIVE_14MA        (1 << 9)	// Drive strength of 14mA
#define DRIVE_20MA        (3 << 8)	// Drive strength of 20mA


/* Configuration examples for various I/O pins */
#define EMC_IO	        (PUP_ENABLE  | PDN_ENABLE  | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)
#define LCD_PINCONFIG   (PUP_DISABLE | PDN_DISABLE | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)
#define CLK_IN	        (PUP_ENABLE  | PDN_ENABLE  | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)
#define CLK_OUT	        (PUP_ENABLE  | PDN_ENABLE  | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)
#define GPIO_PUP	(PUP_ENABLE  | PDN_DISABLE | SLEWRATE_SLOW | INBUF_ENABLE  | FILTER_ENABLE )
#define GPIO_PDN	(PUP_DISABLE | PDN_ENABLE  | SLEWRATE_SLOW | INBUF_ENABLE  | FILTER_ENABLE )
#define GPIO_NOPULL	(PUP_DISABLE | PDN_DISABLE | SLEWRATE_SLOW | INBUF_ENABLE  | FILTER_ENABLE )
#define UART_RX_TX	(PUP_DISABLE | PDN_ENABLE  | SLEWRATE_SLOW | INBUF_ENABLE  | FILTER_ENABLE )
#define SSP_IO	        (PUP_ENABLE  | PDN_ENABLE  | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)


/* Pin function */
#define FUNC0 			0x0				/** Function 0 	*/
#define FUNC1 			0x1				/** Function 1 	*/
#define FUNC2 			0x2				/** Function 2	*/
#define FUNC3 			0x3				/** Function 3	*/
#define FUNC4			0x4
#define FUNC5			0x5
#define FUNC6			0x6
#define FUNC7			0x7
/**
 * @}
 */

#define LPC_SCU_PIN(po, pi)   (*(volatile int         *) (LPC_SCU_BASE + ((po) * 0x80) + ((pi) * 0x4))    )
#define LPC_SCU_CLK(c)        (*(volatile int         *) (LPC_SCU_BASE + 0xC00 + ((c) * 0x4))    )

/* Public Functions ----------------------------------------------------------- */
/** @defgroup SCU_Public_Functions SCU Public Functions
 * @{
 */

void scu_pinmux(uint8_t port, uint8_t pin, uint8_t mode, uint8_t func);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* end __SCU_H */

/**
 * @}
 */

