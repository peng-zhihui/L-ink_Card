/**
 * @file    IO_Config_Override.c
 * @brief   Alternative IO for LPC11U35 based Hardware Interface Circuit
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

#include "LPC11Uxx.h"
#include "daplink.h"

#define TARGET_POWER_HOLD
#define CONTROLLED_POWER_LED
#define INTEGRATED_STATUS_LED

// nRESET is FET drive.
#define PIN_nRESET_FET_DRIVE

// This GPIO configuration is only valid for the LPC11U35 HIC
COMPILER_ASSERT(DAPLINK_HIC_ID == DAPLINK_HIC_ID_LPC11U35);

// Peripheral register bit masks (used for pin inits)
#define FUNC_0                          0
#define FUNC_1                          1
#define PULL_DOWN_ENABLED               (1 << 3)
#define PULL_UP_ENABLED                 (2 << 3)
#define OPENDRAIN                       (1 << 10)

// POW LED                              
#define PIN_POW_LED_PORT                0
#define PIN_POW_LED_BIT                 15
#define PIN_POW_LED                     (1 << PIN_POW_LED_BIT)
#define PIN_POW_LED_IOCON               LPC_IOCON->SWDIO_PIO0_15
#define PIN_POW_LED_IOCON_INIT          (FUNC_1 | OPENDRAIN | PULL_UP_ENABLED)

// DAP LED                              PIO0_16
#define PIN_DAP_LED_PORT                0
#define PIN_DAP_LED_BIT                 16
#define PIN_DAP_LED                     (1 << PIN_DAP_LED_BIT)
#define PIN_DAP_LED_IOCON               LPC_IOCON->PIO0_16
#define PIN_DAP_LED_IOCON_INIT          (FUNC_0 | OPENDRAIN | PULL_UP_ENABLED)

// MSD LED                              PIO0_16
#define PIN_MSD_LED_PORT                0
#define PIN_MSD_LED_BIT                 16
#define PIN_MSD_LED                     (1 << PIN_MSD_LED_BIT)
#define PIN_MSD_LED_IOCON               LPC_IOCON->PIO0_16
#define PIN_MSD_LED_IOCON_INIT          (FUNC_0 | OPENDRAIN | PULL_UP_ENABLED)

// CDC LED                              PIO0_16
#define PIN_CDC_LED_PORT                0
#define PIN_CDC_LED_BIT                 16
#define PIN_CDC_LED                     (1 << PIN_CDC_LED_BIT)
#define PIN_CDC_LED_IOCON               LPC_IOCON->PIO0_16
#define PIN_CDC_LED_IOCON_INIT          (FUNC_0 | OPENDRAIN | PULL_UP_ENABLED)

// Non-Forwarded Reset in PIN           PIO1_19
#define PIN_RESET_IN_PORT               1
#define PIN_RESET_IN_BIT                19
#define PIN_RESET_IN                    (1 << PIN_RESET_IN_BIT)
#define PIN_RESET_IN_IOCON              LPC_IOCON->PIO1_19
#define PIN_RESET_IN_IOCON_INIT         (FUNC_0 | OPENDRAIN | PULL_UP_ENABLED)

// Forwarded Reset in PIN               PIO0_1
#define PIN_RESET_IN_FWRD_PORT          0
#define PIN_RESET_IN_FWRD_BIT           1
#define PIN_RESET_IN_FWRD               (1 << PIN_RESET_IN_FWRD_BIT)
#define PIN_RESET_IN_FWRD_IOCON         LPC_IOCON->PIO0_1
#define PIN_RESET_IN_FWRD_IOCON_INIT    (FUNC_0 | OPENDRAIN | PULL_UP_ENABLED)

// nRESET OUT Pin                       PIO0_2
#define PIN_nRESET_PORT                 0
#define PIN_nRESET_BIT                  2
#define PIN_nRESET                      (1 << PIN_nRESET_BIT)
#define PIN_nRESET_IOCON                LPC_IOCON->PIO0_2
#define PIN_nRESET_IOCON_INIT           (FUNC_0 | PULL_UP_ENABLED)

// SWCLK/TCK Pin                        PIO0_7
#define PIN_SWCLK_PORT                  0
#define PIN_SWCLK_BIT                   7
#define PIN_SWCLK                       (1 << PIN_SWCLK_BIT)
#define PIN_SWCLK_TCK_IOCON             LPC_IOCON->PIO0_7
#define PIN_SWCLK_TCK_IOCON_INIT        (FUNC_0 | PULL_UP_ENABLED)

// SWDIO/TMS In/Out Pin                 PIO0_8
#define PIN_SWDIO_PORT                  0
#define PIN_SWDIO_BIT                   8
#define PIN_SWDIO                       (1 << PIN_SWDIO_BIT)
#define PIN_SWDIO_TMS_IOCON             LPC_IOCON->PIO0_8
#define PIN_SWDIO_TMS_IOCON_INIT        (FUNC_0 | PULL_UP_ENABLED)

// TDI Pin                              PIO0_22
#define PIN_TDI_PORT                    0
#define PIN_TDI_BIT                     22
#define PIN_TDI                         (1 << PIN_TDI_BIT)
#define PIN_TDI_IOCON                   LPC_IOCON->PIO0_22
#define PIN_TDI_IOCON_INIT              (FUNC_0 | PULL_UP_ENABLED)

// SWO/TDO Pin                          PIO0_9
#define PIN_TDO_PORT                    0
#define PIN_TDO_BIT                     9
#define PIN_TDO                         (1 << PIN_TDO_BIT)
#define PIN_TDO_IOCON                   LPC_IOCON->PIO0_9
#define PIN_TDO_IOCON_INIT              (FUNC_0 | PULL_UP_ENABLED)

// PowerSW HOLD Pin                     PIO0_13
#define PIN_PWH_PORT                    0
#define PIN_PWH_BIT                     13
#define PIN_PWH                         (1 << PIN_PWH_BIT)
#define PIN_PWH_IOCON                   LPC_IOCON->TDO_PIO0_13
#define PIN_PWH_IOCON_INIT              (FUNC_1)

#endif
