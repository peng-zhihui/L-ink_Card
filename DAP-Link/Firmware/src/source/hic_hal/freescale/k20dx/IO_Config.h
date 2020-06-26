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

#include "MK20D5.h"
#include "compiler.h"
#include "daplink.h"

// This GPIO configuration is only valid for the K20DX HIC
COMPILER_ASSERT(DAPLINK_HIC_ID == DAPLINK_HIC_ID_K20DX);


// Debug Port I/O Pins

// SWCLK Pin                    PTC5
#define PIN_SWCLK_PORT          PORTC
#define PIN_SWCLK_GPIO          PTC
#define PIN_SWCLK_BIT           5

// SWDIO Out Pin                PTC6
#define PIN_SWDIO_OUT_PORT      PORTC
#define PIN_SWDIO_OUT_GPIO      PTC
#define PIN_SWDIO_OUT_BIT       6

// SWDIO In Pin                 PTC7
#define PIN_SWDIO_IN_PORT       PORTC
#define PIN_SWDIO_IN_GPIO       PTC
#define PIN_SWDIO_IN_BIT        7

// SWDIO Neg. Output Enable Pin PTB0
#define PIN_SWDIO_NOE_PORT      PORTB
#define PIN_SWDIO_NOE_GPIO      PTB
#define PIN_SWDIO_NOE_BIT       0

// SWD Neg. Enable Pin          PTA4
#define PIN_SWD_NOE_PORT        PORTA
#define PIN_SWD_NOE_GPIO        PTA
#define PIN_SWD_NOE_BIT         4

// nRESET Pin                   PTB1
#define PIN_nRESET_PORT         PORTB
#define PIN_nRESET_GPIO         PTB
#define PIN_nRESET_BIT          1
#define PIN_nRESET              (1 << PIN_nRESET_BIT)

// Power and fault detection

// PWR_REG_EN PTD2              PTD6
#define PIN_POWER_EN_PORT       PORTD
#define PIN_POWER_EN_GPIO       PTD
#define PIN_POWER_EN_BIT        (6)
#define PIN_POWER_EN            (1<<PIN_POWER_EN_BIT)

// VTRG_FAULT_B PTD7
#define PIN_VTRG_FAULT_B_PORT   PORTD
#define PIN_VTRG_FAULT_B_GPIO   PTD
#define PIN_VTRG_FAULT_B_BIT    (7)

// Debug Unit LEDs

// Connected LED                PTD4
#define LED_CONNECTED_PORT      PORTD
#define LED_CONNECTED_GPIO      PTD
#define LED_CONNECTED_BIT       4

// Target Running LED           Not available

// Debug Unit LEDs

// HID_LED PTD4
#define PIN_HID_LED_PORT        PORTD
#define PIN_HID_LED_GPIO        PTD
#define PIN_HID_LED_BIT         (4)
#define PIN_HID_LED             (1<<PIN_HID_LED_BIT)

// MSC_LED PTD4
#define PIN_MSC_LED_PORT        PORTD
#define PIN_MSC_LED_GPIO        PTD
#define PIN_MSC_LED_BIT         (4)
#define PIN_MSC_LED             (1<<PIN_HID_LED_BIT)

// CDC_LED PTD4
#define PIN_CDC_LED_PORT        PORTD
#define PIN_CDC_LED_GPIO        PTD
#define PIN_CDC_LED_BIT         (4)
#define PIN_CDC_LED             (1<<PIN_HID_LED_BIT)

// SW RESET BUTTON PTB1
#define PIN_SW_RESET_PORT       PORTB
#define PIN_SW_RESET_GPIO       PTB
#define PIN_SW_RESET_BIT        (1)
#define PIN_SW_RESET            (1<<PIN_SW_RESET_BIT)

#endif
