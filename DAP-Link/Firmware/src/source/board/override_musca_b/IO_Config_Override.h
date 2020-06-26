/**
 * @file    IO_Config_Override.h
 * @brief   Alternative IO for LPC11U35 based Hardware Interface Circuit
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
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

// This GPIO configuration is only valid for the LPC11U35 HIC
COMPILER_ASSERT(DAPLINK_HIC_ID == DAPLINK_HIC_ID_LPC11U35);

// Enable Musca B specific code
#define MUSCA_B_BOARD                   1

// Musca B board has PBON button
#define PBON_BUTTON                     1

// Peripheral register bit masks (used for pin inits)
#define FUNC_0                          0           // Primary function
#define FUNC_1                          1           // Secondary function
#define PULL_DOWN_ENABLED               (1 << 3)
#define PULL_UP_ENABLED                 (2 << 3)
#define OPENDRAIN                       (1 << 10)
#define DIGITAL_IN                      (1 << 7)

// DAP LED                              PIO0_21
#define PIN_DAP_LED_PORT                0
#define PIN_DAP_LED_BIT                 21
#define PIN_DAP_LED                     (1 << PIN_DAP_LED_BIT)
#define PIN_DAP_LED_IOCON               LPC_IOCON->PIO0_21
#define PIN_DAP_LED_IOCON_INIT          (FUNC_0 | PULL_UP_ENABLED)

// MSD LED                              PIO0_20
#define PIN_MSD_LED_PORT                0
#define PIN_MSD_LED_BIT                 20
#define PIN_MSD_LED                     (1 << PIN_MSD_LED_BIT)
#define PIN_MSD_LED_IOCON               LPC_IOCON->PIO0_20
#define PIN_MSD_LED_IOCON_INIT          (FUNC_0 | PULL_UP_ENABLED)

// CDC LED                              PIO0_11
#define PIN_CDC_LED_PORT                0
#define PIN_CDC_LED_BIT                 11
#define PIN_CDC_LED                     (1 << PIN_CDC_LED_BIT)
#define PIN_CDC_LED_IOCON               LPC_IOCON->TDI_PIO0_11
#define PIN_CDC_LED_IOCON_INIT          (FUNC_1 | PULL_UP_ENABLED)

// CM_nISP / Non-Forwarded Reset in PIN PIO0_1
#define PIN_RESET_IN_PORT               0
#define PIN_RESET_IN_BIT                1
#define PIN_RESET_IN                    (1 << PIN_RESET_IN_BIT)
#define PIN_RESET_IN_IOCON              LPC_IOCON->PIO0_1
#define PIN_RESET_IN_IOCON_INIT         (FUNC_0 | OPENDRAIN | PULL_UP_ENABLED)

// Forwarded Reset in PIN               PIO1_19
#define PIN_RESET_IN_FWRD_PORT          0 //1
#define PIN_RESET_IN_FWRD_BIT           1 //19
#define PIN_RESET_IN_FWRD               (1 << PIN_RESET_IN_FWRD_BIT)
#define PIN_RESET_IN_FWRD_IOCON         LPC_IOCON->PIO0_1 //1_19
#define PIN_RESET_IN_FWRD_IOCON_INIT    (FUNC_0 | OPENDRAIN | PULL_UP_ENABLED)

// CS_nSRST / nRESET OUT Pin            PIO0_2
#define PIN_nRESET_PORT                 0
#define PIN_nRESET_BIT                  2
#define PIN_nRESET                      (1 << PIN_nRESET_BIT)
#define PIN_nRESET_IOCON                LPC_IOCON->PIO0_2
#define PIN_nRESET_IOCON_INIT           (FUNC_0 | OPENDRAIN | PULL_UP_ENABLED)

// SWCLK                                PIO0_7
#define PIN_SWCLK_PORT                  0
#define PIN_SWCLK_BIT                   7
#define PIN_SWCLK                       (1 << PIN_SWCLK_BIT)
#define PIN_SWCLK_TCK_IOCON             LPC_IOCON->PIO0_7
#define PIN_SWCLK_TCK_IOCON_INIT        (FUNC_0 | PULL_UP_ENABLED)

// SWDIO In/Out Pin                     PIO0_8
#define PIN_SWDIO_PORT                  0
#define PIN_SWDIO_BIT                   8
#define PIN_SWDIO                       (1 << PIN_SWDIO_BIT)
#define PIN_SWDIO_TMS_IOCON             LPC_IOCON->PIO0_8
#define PIN_SWDIO_TMS_IOCON_INIT        (FUNC_0 | PULL_UP_ENABLED)

// Reset and Power control pins
// PBON                                 PIO0_16
#define PIN_PBON_PORT                   0
#define PIN_PBON_BIT                    16
#define PIN_PBON                        (1 << PIN_PBON_BIT)
#define PIN_PBON_IOCON                  LPC_IOCON->PIO0_16
#define PIN_PBON_IOCON_INIT             (FUNC_0 | DIGITAL_IN)

// PWROK Pin                            PIO0_17
#define PIN_PWROK_PORT                  0
#define PIN_PWROK_BIT                   17
#define PIN_PWROK                       (1 << PIN_PWROK_BIT)
#define PIN_PWROK_IOCON                 LPC_IOCON->PIO0_17
#define PIN_PWROK_IOCON_INIT            (FUNC_0 | PULL_UP_ENABLED)

// SHUTDOWN                             PIO0_15
#define PIN_SHUTDOWN_PORT               0
#define PIN_SHUTDOWN_BIT                15
#define PIN_SHUTDOWN                    (1 << PIN_SHUTDOWN_BIT)
#define PIN_SHUTDOWN_IOCON              LPC_IOCON->SWDIO_PIO0_15
#define PIN_SHUTDOWN_IOCON_INIT         (FUNC_1 | DIGITAL_IN | PULL_UP_ENABLED)

// CB_nRST OUT Pin                      PIO0_4
#define PIN_CB_nRST_PORT                0
#define PIN_CB_nRST_BIT                 4
#define PIN_CB_nRST                     (1 << PIN_CB_nRST_BIT)
#define PIN_CB_nRST_IOCON               LPC_IOCON->PIO0_4
#define PIN_CB_nRST_IOCON_INIT          (FUNC_0 | PULL_UP_ENABLED)

// CFG_nRST OUT Pin                     PIO0_5
#define PIN_CFG_nRST_PORT               0
#define PIN_CFG_nRST_BIT                5
#define PIN_CFG_nRST                    (1 << PIN_CFG_nRST_BIT)
#define PIN_CFG_nRST_IOCON              LPC_IOCON->PIO0_5
#define PIN_CFG_nRST_IOCON_INIT         (FUNC_0 | PULL_UP_ENABLED)

// CS_nDET detect for external debug to SoC
#define PIN_CS_nDET_PORT                1
#define PIN_CS_nDET_BIT                 15
#define PIN_CS_nDET                     (1 << PIN_CS_nDET_BIT)
#define PIN_CS_nDET_IOCON               LPC_IOCON->PIO1_15
#define PIN_CS_nDET_IOCON_INIT          (FUNC_0 | DIGITAL_IN | PULL_UP_ENABLED)

// SCC Configuration Pins
// SCC_LOAD                             PIO0_9
#define PIN_SCC_LOAD_PORT               0
#define PIN_SCC_LOAD_BIT                9
#define PIN_SCC_LOAD                    (1 << PIN_SCC_LOAD_BIT)
#define PIN_SCC_LOAD_IOCON              LPC_IOCON->PIO0_9
#define PIN_SCC_LOAD_IOCON_INIT         (FUNC_0 | PULL_UP_ENABLED)

// SCC_WNR                              PIO0_10
#define PIN_SCC_WNR_PORT                0
#define PIN_SCC_WNR_BIT                 10
#define PIN_SCC_WNR                     (1 << PIN_SCC_WNR_BIT)
#define PIN_SCC_WNR_IOCON               LPC_IOCON->SWCLK_PIO0_10
#define PIN_SCC_WNR_IOCON_INIT          (FUNC_1 | PULL_UP_ENABLED)

// SCC_DATAIN                           PIO0_12
#define PIN_SCC_DATAIN_PORT             0
#define PIN_SCC_DATAIN_BIT              12
#define PIN_SCC_DATAIN                  (1 << PIN_SCC_DATAIN_BIT)
#define PIN_SCC_DATAIN_IOCON            LPC_IOCON->TMS_PIO0_12
#define PIN_SCC_DATAIN_IOCON_INIT       (FUNC_1 | PULL_UP_ENABLED)

// SCC_CLK                              PIO0_13
#define PIN_SCC_CLK_PORT                0
#define PIN_SCC_CLK_BIT                 13
#define PIN_SCC_CLK                     (1 << PIN_SCC_CLK_BIT)
#define PIN_SCC_CLK_IOCON               LPC_IOCON->TDO_PIO0_13
#define PIN_SCC_CLK_IOCON_INIT          (FUNC_1 | PULL_UP_ENABLED)

// SCC_DATAOUT                          PIO0_14
#define PIN_SCC_DATAOUT_PORT            0
#define PIN_SCC_DATAOUT_BIT             14
#define PIN_SCC_DATAOUT                 (1 << PIN_SCC_DATAOUT_BIT)
#define PIN_SCC_DATAOUT_IOCON           LPC_IOCON->TRST_PIO0_14
#define PIN_SCC_DATAOUT_IOCON_INIT      (FUNC_1 | DIGITAL_IN | PULL_UP_ENABLED)

// I2C_SCL                              PIO0_22
#define PIN_I2C_SCL_PORT                0
#define PIN_I2C_SCL_BIT                 22
#define PIN_I2C_SCL                     (1 << PIN_I2C_SCL_BIT)
#define PIN_I2C_SCL_IOCON               LPC_IOCON->PIO0_22
#define PIN_I2C_SCL_IOCON_INIT          (FUNC_0 | OPENDRAIN | PULL_UP_ENABLED)

// I2C_SDA                              PIO0_23
#define PIN_I2C_SDA_PORT                0
#define PIN_I2C_SDA_BIT                 23
#define PIN_I2C_SDA                     (1 << PIN_I2C_SDA_BIT)
#define PIN_I2C_SDA_IOCON               LPC_IOCON->PIO0_23
#define PIN_I2C_SDA_IOCON_INIT          (FUNC_0 | OPENDRAIN | PULL_UP_ENABLED)

// USART                                PIO0_18
#define PIN_USART_PORT                  0
#define PIN_USART_BIT                   18
#define PIN_USART                       (1 << PIN_USART_BIT)
#define PIN_USART_IOCON                 LPC_IOCON->PIO0_18
#define PIN_USART_IOCON_INIT            (FUNC_1 | PULL_DOWN_ENABLED)

#endif
