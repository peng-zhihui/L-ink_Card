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

#include "fsl_device_registers.h"
#include "compiler.h"
#include "daplink.h"

// This GPIO configuration is only valid for the K26F HIC
COMPILER_ASSERT(DAPLINK_HIC_ID == DAPLINK_HIC_ID_K26F);


// Debug Port I/O Pins

// SWCLK Pin                    PTC5
// (SDA_SWD_SCK on schematic)
#define PIN_SWCLK_PORT          PORTC
#define PIN_SWCLK_GPIO          PTC
#define PIN_SWCLK_BIT           5

// SWDIO Out Pin                PTC6
// (SDA_SWD_DOUT on schematic)
#define PIN_SWDIO_OUT_PORT      PORTC
#define PIN_SWDIO_OUT_GPIO      PTC
#define PIN_SWDIO_OUT_BIT       6

// SWDIO In Pin                 PTC7
// (SDA_SWD_DIN on schematic)
#define PIN_SWDIO_IN_PORT       PORTC
#define PIN_SWDIO_IN_GPIO       PTC
#define PIN_SWDIO_IN_BIT        7

// SWDIO Output Enable Pin     PTA5
// (SDA_SWD_OE on schematic)
#define PIN_SWDIO_OE_PORT      PORTA
#define PIN_SWDIO_OE_GPIO      PTA
#define PIN_SWDIO_OE_BIT       5

// SWD Enable Pin              PTA4
// (SDA_SWD_EN on schematic)
#define PIN_SWD_OE_PORT        PORTA
#define PIN_SWD_OE_GPIO        PTA
#define PIN_SWD_OE_BIT         9

// SWO Input Pin               PTC3
// (SDA_SWD_SWO on schematic)
#define PIN_SWO_RX_PORT        PORTC
#define PIN_SWO_RX_GPIO        PTC
#define PIN_SWO_RX_BIT         3

// nRESET Pin                   PTA7
#define PIN_nRESET_PORT         PORTA
#define PIN_nRESET_GPIO         PTA
#define PIN_nRESET_BIT          7
#define PIN_nRESET              (1 << PIN_nRESET_BIT)

// nRESET Pin Level Shifter Enable PTA6
// (SDA_LVLRST_EN on schematic)
#define PIN_nRESET_EN_PORT      PORTA
#define PIN_nRESET_EN_GPIO      PTA
#define PIN_nRESET_EN_BIT       6
#define PIN_nRESET_EN           (1 << PIN_nRESET_EN_BIT)

// SWD Detect Pin               PTA8
// (x_SWD_DETECT on schematic)
#define PIN_SWD_DETECT_PORT     PORTA
#define PIN_SWD_DETECTGPIO      PTA
#define PIN_SWD_DETECT_BIT      8
#define PIN_SWD_DETECT          (1 << PIN_SWD_DETECT_BIT)


// Power monitor

// SDA_G1 Pin                PTE17
#define PIN_G1_PORT          PORTE
#define PIN_G1_GPIO          PTE
#define PIN_G1_BIT           17
#define PIN_G1               (1 << PIN_G1_BIT)

// SDA_G2 Pin                PTE18
#define PIN_G2_PORT          PORTE
#define PIN_G2_GPIO          PTE
#define PIN_G2_BIT           18
#define PIN_G2               (1 << PIN_G2_BIT)

// SDA_LOW_RANGE_EN Pin      PTE19
#define PIN_LOW_RANGE_EN_PORT          PORTE
#define PIN_LOW_RANGE_EN_GPIO          PTE
#define PIN_LOW_RANGE_EN_BIT           19
#define PIN_LOW_RANGE_EN               (1 << PIN_LOW_RANGE_EN_BIT)

// SDA_CAL_EN Pin            PTE24
#define PIN_CAL_EN_PORT      PORTE
#define PIN_CAL_EN_GPIO      PTE
#define PIN_CAL_EN_BIT       24
#define PIN_CAL_EN           (1 << PIN_CAL_EN_BIT)

// SDA_CTRL0 Pin             PTE25
#define PIN_CTRL0_PORT       PORTE
#define PIN_CTRL0_GPIO       PTE
#define PIN_CTRL0_BIT        25
#define PIN_CTRL0            (1 << PIN_CTRL0_BIT)

// SDA_CTRL1 Pin             PTE26
#define PIN_CTRL1_PORT       PORTE
#define PIN_CTRL1_GPIO       PTE
#define PIN_CTRL1_BIT        26
#define PIN_CTRL1            (1 << PIN_CTRL1_BIT)

// SDA_CTRL2 Pin             PTE27
#define PIN_CTRL2_PORT       PORTE
#define PIN_CTRL2_GPIO       PTE
#define PIN_CTRL2_BIT        27
#define PIN_CTRL2            (1 << PIN_CTRL2_BIT)

// SDA_CTRL3 Pin             PTE28
#define PIN_CTRL3_PORT       PORTE
#define PIN_CTRL3_GPIO       PTE
#define PIN_CTRL3_BIT        28
#define PIN_CTRL3            (1 << PIN_CTRL3_BIT)


// Misc target connections

// SDA_GPIO0_B Pin           PTB22
#define PIN_GPIO0_B_PORT     PORTB
#define PIN_GPIO0_B_GPIO     PTB
#define PIN_GPIO0_B_BIT      22
#define PIN_GPIO0_B          (1 < PIN_GPIO0_B_BIT)

// SDA_CLKOUT_B Pin          PTC
#define PIN_CLKOUT_B_PORT    PORTC
#define PIN_CLKOUT_B_GPIO    PTC
#define PIN_CLKOUT_B_BIT     3
#define PIN_CLKOUT_B         (1 << PIN_CLKOUT_B_BIT)


// Power and fault detection

// PWR_REG_EN                   PTE12
#define PIN_POWER_EN_PORT       PORTE
#define PIN_POWER_EN_GPIO       PTE
#define PIN_POWER_EN_BIT        12
#define PIN_POWER_EN            (1 << PIN_POWER_EN_BIT)

// VTRG_FAULT_B                 PTE11
#define PIN_VTRG_FAULT_B_PORT   PORTE
#define PIN_VTRG_FAULT_B_GPIO   PTE
#define PIN_VTRG_FAULT_B_BIT    11

// Debug Unit LEDs

// Connected LED                PTD4
#define LED_CONNECTED_PORT      PORTD
#define LED_CONNECTED_GPIO      PTD
#define LED_CONNECTED_BIT       4
#define LED_CONNECTED           (1 << LED_CONNECTED_BIT)

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
