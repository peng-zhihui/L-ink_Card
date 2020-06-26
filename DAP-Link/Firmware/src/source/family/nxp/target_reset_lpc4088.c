/**
 * @file    target_reset.c
 * @brief   Target reset for the lpc4088
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

#include "swd_host.h"
#include "gpio.h"
#include "target_family.h"

#define PIN_ISPCTRL1      (1<<12)
#define PIN_ISPCTRL2      (1<<15)

static void gpio_set_isp_pin(uint8_t state) {
    if (state) {
        // High => Both pins are inputs
        LPC_GPIO->DIR[0] &= ~(PIN_ISPCTRL1);
        LPC_GPIO->DIR[1] &= ~(PIN_ISPCTRL2);
    } else {
        // Low => Both pins are outputs with 0
        LPC_GPIO->CLR[0] = (PIN_ISPCTRL1);
        LPC_GPIO->CLR[1] = (PIN_ISPCTRL2);
        LPC_GPIO->DIR[0] |= (PIN_ISPCTRL1);
        LPC_GPIO->DIR[1] |= (PIN_ISPCTRL2);
    }
}

static void prerun_target_config(void)
{
    // Configure ISPCTRL as output and high
    // We use two ISP pins to cover different hardware versions
    LPC_IOCON->TMS_PIO0_12 |= 0x01;
    LPC_IOCON->PIO1_15 &= ~0x07;
    gpio_set_isp_pin(1);
}


static uint8_t lpc4088_target_set_state(target_state_t state)
{
    //return swd_set_target_state_hw(state);
    uint8_t res;
    if (state == RESET_PROGRAM)
    {
        gpio_set_isp_pin(0);
        res = swd_set_target_state_hw(state);
        gpio_set_isp_pin(1);
    }
    else
    {
        gpio_set_isp_pin(1);
        res = swd_set_target_state_hw(state);
    }
    return res;
}

const target_family_descriptor_t g_nxp_lpc4088 = {
    .family_id = VENDOR_TO_FAMILY(kNXP_VendorID, 0), //ID not maching the predefined family ids
    .prerun_target_config = prerun_target_config,
    .target_set_state = lpc4088_target_set_state,
};

const target_family_descriptor_t *g_target_family = &g_nxp_lpc4088;

