/**
 * @file    target_reset.c
 * @brief   Target reset for Musca A target
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

#include "target_family.h"
#include "target_config.h"  // for target_device
#include "swd_host.h"
#include "gpio.h"
#include "debug_cm.h"
#include "utils.h"
#include "power_ctrl.h"
#include "uart.h"

static void musca_a_target_before_init_debug(void)
{
    uint8_t buf[12];

    // go into controlled shutdown
    power_off_sequence();

    // Drive SCC signals
    LPC_GPIO->DIR[PIN_SCC_CLK_PORT]     |= PIN_SCC_CLK;
    LPC_GPIO->DIR[PIN_SCC_DATAIN_PORT]  |= PIN_SCC_DATAIN;
    LPC_GPIO->DIR[PIN_SCC_DATAOUT_PORT] &= ~PIN_SCC_DATAOUT;
    LPC_GPIO->DIR[PIN_SCC_WNR_PORT]     |= PIN_SCC_WNR;
    LPC_GPIO->DIR[PIN_SCC_LOAD_PORT]    |= PIN_SCC_LOAD;

    // Wait 10ms
    delay(10);

    // Release CFG_nRST to allow SCC config
    LPC_GPIO->SET[PIN_CFG_nRST_PORT] = PIN_CFG_nRST;

    // Wait 10ms
    delay(10);

    // Configure SCC
    configure_syscon(0x10000000);

    // Wait 10ms
    delay(10);

    // Creating branch to self in SRAM
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0x08;
    buf[3] = 0x30;
    buf[4] = 0x09;
    buf[5] = 0x00;
    buf[6] = 0x00;
    buf[7] = 0x10;
    buf[8] = 0xFE;
    buf[9] = 0xE7;
    buf[10] = 0xFE;
    buf[11] = 0xE7;

    swd_write_memory(0x10000000, (uint8_t *)buf, 12);

    // Wait 10ms
    delay(10);

    // Release SCC signals
    LPC_GPIO->DIR[PIN_SCC_CLK_PORT]     &= ~PIN_SCC_CLK;
    LPC_GPIO->DIR[PIN_SCC_DATAIN_PORT]  &= ~PIN_SCC_DATAIN;
    LPC_GPIO->DIR[PIN_SCC_DATAOUT_PORT] &= ~PIN_SCC_DATAOUT;
    LPC_GPIO->DIR[PIN_SCC_WNR_PORT]     &= ~PIN_SCC_WNR;
    LPC_GPIO->DIR[PIN_SCC_LOAD_PORT]    &= ~PIN_SCC_LOAD;

    // Wait 10ms
    delay(10);

    // Release CB_nRST (nPORESET)
    LPC_GPIO->SET[PIN_CB_nRST_PORT] = PIN_CB_nRST;

    // Wait 10ms
    delay(10);

    // Release CS_nSRST
    LPC_GPIO->SET[PIN_nRESET_PORT] = PIN_nRESET;

    // Wait 10ms
    delay(10);

    return;
}

static uint8_t musca_a_target_set_state(target_state_t state)
{
    if(state == RESET_RUN)
    {
        // go through controlled reset
        power_off_sequence();

        power_on_sequence();

        // Wait 10ms
        delay(10);

        swd_off();

        return 1;
    }
    if(state == SHUTDOWN)
    {
        // go through controlled shutdown
        power_off_sequence();

        // Remove PSUON
        LPC_GPIO->CLR[PIN_PSUON_PORT] = PIN_PSUON;

        // Wait 10ms
        delay(10);

        uart_reset();

        return 1;
    }
    if(state == POWER_ON)
    {
        // Switch on power supply - PSUON
        LPC_GPIO->SET[PIN_PSUON_PORT] = PIN_PSUON;

        // Wait 10ms
        delay(10);

        // power on the target
        power_on_sequence();

        // Wait 10ms
        delay(10);

        swd_off();
        return 1;
    }

    return swd_set_target_state_sw(state);
}

const target_family_descriptor_t g_target_family_musca_a = {
    .target_before_init_debug = musca_a_target_before_init_debug,
    .target_set_state = musca_a_target_set_state,
    .apsel = 0x01000000,
};

const target_family_descriptor_t *g_target_family = &g_target_family_musca_a;
