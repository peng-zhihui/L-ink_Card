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

#include "sam3u2c.h"
#include "daplink.h"

#ifdef IO_CONFIG_OVERRIDE
#include "IO_Config_Override.h"
#endif

// This GPIO configuration is only valid for the LPC11U35 HIC
COMPILER_ASSERT(DAPLINK_HIC_ID == DAPLINK_HIC_ID_SAM3U2C);

// DAP LED
#ifndef PIN_DAP_LED
#define PIN_DAP_LED_PORT        PIOA
#define PIN_DAP_LED_BIT         29
#define PIN_DAP_LED             (1UL << PIN_DAP_LED_BIT)
#endif

// MSD LED
#ifndef PIN_MSD_LED
#define PIN_MSD_LED_PORT        PIOA
#define PIN_MSD_LED_BIT         28
#define PIN_MSD_LED             (1UL << PIN_MSD_LED_BIT)
#endif

// CDC LED
#ifndef PIN_CDC_LED
#define PIN_CDC_LED_PORT        PIOA
#define PIN_CDC_LED_BIT         31
#define PIN_CDC_LED             (1UL << PIN_CDC_LED_BIT)
#endif

// Non-Forwarded Reset in PIN - Not used

// Forwarded Reset in PIN     
#ifndef PIN_RESET_IN_FWRD
#define PIN_RESET_IN_FWRD_PORT  PIOA
#define PIN_RESET_IN_FWRD_BIT   25
#define PIN_RESET_IN_FWRD       (1UL << PIN_RESET_IN_FWRD_BIT)
#endif

// nRESET OUT Pin
#ifndef PIN_nRESET
#define PIN_nRESET_PORT         PIOA
#define PIN_nRESET_BIT          4
#define PIN_nRESET              (1UL << PIN_nRESET_BIT)
#endif

// SWCLK/TCK Pin
#ifndef PIN_SWCLK
#define PIN_SWCLK_PORT          PIOA
#define PIN_SWCLK_BIT           17
#define PIN_SWCLK               (1UL << PIN_SWCLK_BIT)
#endif

// SWDIO/TMS In/Out Pin
#ifndef PIN_SWDIO
#define PIN_SWDIO_PORT          PIOA
#define PIN_SWDIO_BIT           18
#define PIN_SWDIO               (1UL << PIN_SWDIO_BIT)
#endif

// TDI Pin - Not used

// SWO/TDO Pin - Not used

#endif
