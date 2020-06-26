/**
 * @file    target_reset_nrf52.c
 * @brief   Target reset for the nrf52
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
#include "DAP_config.h"
#include "target_family.h"
#include "target_board.h"

static void swd_set_target_reset_nrf(uint8_t asserted)
{
    uint32_t ap_index_return;

    if (asserted) {
        swd_init_debug();

        swd_read_ap(0x010000FC, &ap_index_return);
        if (ap_index_return == 0x02880000) {
            // Have CTRL-AP
            swd_write_ap(0x01000000, 1);  // CTRL-AP reset hold
        }
        else {
            // No CTRL-AP - Perform a soft reset
            // 0x05FA0000 = VECTKEY, 0x4 = SYSRESETREQ
            uint32_t swd_mem_write_data = 0x05FA0000 | 0x4;
            swd_write_memory(0xE000ED0C, (uint8_t *) &swd_mem_write_data, 4);
        }
        if(g_board_info.swd_set_target_reset){ //aditional reset
            g_board_info.swd_set_target_reset(asserted);
        }
    } else {
        swd_read_ap(0x010000FC, &ap_index_return);
        if (ap_index_return == 0x02880000) {
            // Device has CTRL-AP
            swd_write_ap(0x01000000, 0);  // CTRL-AP reset release
        }
        else {
            // No CTRL-AP - Soft reset has been performed
        }
        if(g_board_info.swd_set_target_reset){
            g_board_info.swd_set_target_reset(asserted);
        }
    }
}

const target_family_descriptor_t g_nordic_nrf52 = {
    .family_id = kNordic_Nrf52_FamilyID,
    .default_reset_type = kSoftwareReset,
    .soft_reset_type = SYSRESETREQ,
    .swd_set_target_reset = swd_set_target_reset_nrf,
};
