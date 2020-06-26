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

#include "max32620.h"

#ifndef __IO_CONFIG_H__
#define __IO_CONFIG_H__

// DAP LED
#define PIN_DAP_LED_PORT 4
#define PIN_DAP_LED_PIN  2

// MSD LED
#define PIN_MSD_LED_PORT 4
#define PIN_MSD_LED_PIN  0

// CDC LED
#define PIN_CDC_LED_PORT 4
#define PIN_CDC_LED_PIN  1

// Non-Forwarded Reset In Pin
#define PIN_RESET_IN_NO_FWRD_PORT 5
#define PIN_RESET_IN_NO_FWRD_PIN  0

#define TRGT_PORT    1
#define SRST_PIN     0
#define RSTN_PIN     1
#define TDI_PIN      2
#define TCK_PIN      3
#define TMS_PIN      4
#define TDO_PIN      5
#define VDDIO_PIN    6

#define BUFFEN_PORT  6
#define BUFFEN_PIN   0

#define SWCLK_PIN   TCK_PIN
#define SWDIO_PIN   TMS_PIN

#define MXC_E_GPIO_OUT_MODE_TRISTATE              0
#define MXC_E_GPIO_OUT_MODE_OPEN_DRAIN_W_PULLUP   2
#define MXC_E_GPIO_OUT_MODE_NORMAL                5

#endif
