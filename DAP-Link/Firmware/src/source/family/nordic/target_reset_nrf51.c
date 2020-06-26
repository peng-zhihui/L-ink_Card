/**
 * @file    target_reset.c
 * @brief   Target reset for the nrf51
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
    if (asserted) {
        swd_init_debug();

        //Set POWER->RESET on NRF to 1
        if (!swd_write_ap(AP_TAR, 0x40000000 + 0x544)) {
            return;
        }

        if (!swd_write_ap(AP_DRW, 1)) {
            return;
        }

        //Hold RESET and SWCLK low for a minimum of 100us
        if(g_board_info.swd_set_target_reset){
            g_board_info.swd_set_target_reset(asserted);
        }
        PIN_SWCLK_TCK_CLR();
        PIN_SWDIO_TMS_CLR();
        //osDelay(1);
    } else {
        PIN_SWCLK_TCK_SET();
        PIN_SWDIO_TMS_SET();
        if(g_board_info.swd_set_target_reset){
            g_board_info.swd_set_target_reset(asserted);
        }
    }
}

const target_family_descriptor_t g_nordic_nrf51 = {
    .family_id = kNordic_Nrf51_FamilyID,
    .default_reset_type = kSoftwareReset,
    .soft_reset_type = SYSRESETREQ,
    .swd_set_target_reset = swd_set_target_reset_nrf,
};
