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

// Override all defines if IO_CONFIG_OVERRIDE is defined

#ifndef __IO_CONFIG_OVERRIDE_H__
#define __IO_CONFIG_OVERRIDE_H__

#include "MKL26Z4.h"
#include "compiler.h"
#include "daplink.h"

// This GPIO configuration is only valid for the KL26 HIC
COMPILER_ASSERT(DAPLINK_HIC_ID == DAPLINK_HIC_ID_KL26);


// Debug Port I/O Pins

// SWCLK Pin                    PTC5(C5)
#define PIN_SWCLK_PORT          PORTC
#define PIN_SWCLK_GPIO          PTC
#define PIN_SWCLK_BIT           (5)
#define PIN_SWCLK               (1<<PIN_SWCLK_BIT)

// SWDIO Pin                    PTC6(C6)
#define PIN_SWDIO_PORT          PORTC
#define PIN_SWDIO_GPIO          PTC
#define PIN_SWDIO_BIT           (6)
#define PIN_SWDIO               (1<<PIN_SWDIO_BIT)

// nRESET Pin                   PTC8(C8)
#define PIN_nRESET_PORT         PORTC
#define PIN_nRESET_GPIO         PTC
#define PIN_nRESET_BIT          (1/*8 DUE TO SMALLER PACKAGE*/)
#define PIN_nRESET              (1<<PIN_nRESET_BIT)

// PWR_REG_EN PTD2 - Not connected
#define PIN_POWER_EN_PORT       PORTD
#define PIN_POWER_EN_GPIO       PTD
#define PIN_POWER_EN_BIT        (2)
#define PIN_POWER_EN            (1<<PIN_POWER_EN_BIT)

// VTRG_FAULT_B PTD3 - Not connected
#define PIN_VTRG_FAULT_B_PORT   PORTD
#define PIN_VTRG_FAULT_B_GPIO   PTD
#define PIN_VTRG_FAULT_B_BIT    (7)
#define PIN_VTRG_FAULT_B_EN     (1<<PIN_VTRG_FAULT_B_BIT)

// Debug Unit LEDs

// HID_LED PTD4
#define PIN_HID_LED_PORT        PORTD
#define PIN_HID_LED_GPIO        PTD
#define PIN_HID_LED_BIT         (4) 
#define PIN_HID_LED             (1<<PIN_HID_LED_BIT)

// MSC_LED PTD5
#define PIN_MSC_LED_PORT        PORTD
#define PIN_MSC_LED_GPIO        PTD
#define PIN_MSC_LED_BIT         (4)
#define PIN_MSC_LED             (1<<PIN_MSC_LED_BIT)

// CDC_LED PTD6
#define PIN_CDC_LED_PORT        PORTD
#define PIN_CDC_LED_GPIO        PTD
#define PIN_CDC_LED_BIT         (4)
#define PIN_CDC_LED             (1<<PIN_CDC_LED_BIT)

// SW RESET BUTTON PTB1
#define PIN_SW_RESET_PORT       PORTB
#define PIN_SW_RESET_GPIO       PTB
#define PIN_SW_RESET_BIT        (1)
#define PIN_SW_RESET            (1<<PIN_SW_RESET_BIT)

// Connected LED                Not available

// Target Running LED           Not available

// UART
#define UART_PORT               PORTC
#define UART_NUM                (1)
// RX PTC3
#define PIN_UART_RX_GPIO        PTC
#define PIN_UART_RX_BIT         (3)
#define PIN_UART_RX             (1<<PIN_UART_RX_BIT)
#define PIN_UART_RX_MUX_ALT     (3)
// TX PTC4
#define PIN_UART_TX_GPIO        PTC
#define PIN_UART_TX_BIT         (4)
#define PIN_UART_TX             (1<<PIN_UART_TX_BIT)
#define PIN_UART_TX_MUX_ALT     (3)

#define UART                    UART1
#define UART_RX_TX_IRQn         UART1_IRQn
#define UART_RX_TX_IRQHandler   UART1_IRQHandler

#endif
