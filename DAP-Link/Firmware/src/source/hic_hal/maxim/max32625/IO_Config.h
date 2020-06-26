/**
 * @file    IO_Config.h
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

#ifndef __IO_CONFIG_H__
#define __IO_CONFIG_H__

#include "max32625.h"

#define EN_VDDIOH_PORT      3
#define EN_VDDIOH_PIN       6

// The MAX14689 is powered with this pin
#define IOH_OW_EN_PORT      2
#define IOH_OW_EN_PIN       2

// Low selects NC pins HDR_OW and HDR_VIO
// High selcts NO pins SWD_OW and SWD_VIO
#define SWD_DIP_SEL_PORT    2
#define SWD_DIP_SEL_PIN     3

// UART Tx
#define PIN_TX_PORT         2
#define PIN_TX_PIN          0
#define PIN_DIP_TX_PORT     0
#define PIN_DIP_TX_PIN      0

// UART Rx
#define PIN_RX_PORT         2
#define PIN_RX_PIN          1
#define PIN_DIP_RX_PORT     0
#define PIN_DIP_RX_PIN      1

// DAP LED
#define PIN_DAP_LED_PORT    2
#define PIN_DAP_LED_PIN     5

// MSD LED
#define PIN_MSD_LED_PORT    2
#define PIN_MSD_LED_PIN     4

// CDC LED
#define PIN_CDC_LED_PORT    2
#define PIN_CDC_LED_PIN     6

// Non-Forwarded Reset In Pin
#define PIN_RESET_IN_NO_FWRD_PORT  2
#define PIN_RESET_IN_NO_FWRD_PIN   7

// nRESET
#define PIN_nRESET_PORT     3
#define PIN_nRESET_PIN      7
#define PIN_DIP_nRESET_PORT 0
#define PIN_DIP_nRESET_PIN  4

// SWCLK
#define PIN_SWCLK_PORT      3
#define PIN_SWCLK_PIN       2
#define PIN_DIP_SWCLK_PORT  0
#define PIN_DIP_SWCLK_PIN   2

// SWDIO
#define PIN_SWDIO_PORT      3
#define PIN_SWDIO_PIN       3
#define PIN_DIP_SWDIO_PORT  0
#define PIN_DIP_SWDIO_PIN   3

// 1-Wire master I/O
#define OWM_PORT            4
#define OWM_PIN             0
#define OWM_SUP_PORT        4
#define OWM_SUP_PIN         1

// ADC channels for target detection
#define SWD_VIO_CH          5
#define HDR_VIO_CH          4
#define SWD_GNDDET_CH       3
#define HDR_GNDDET_CH       2

typedef enum {
    IO_SWD_EXT,
    IO_DIP_EXT
} TARGET_INTERFACE;

#define CDC_ACM_UART_SWD 2
#define CDC_ACM_UART_DIP 0

#endif
