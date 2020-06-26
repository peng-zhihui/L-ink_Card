/**
 * @file    target_family.c
 * @brief   Implementation of target_family.h
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

#include "daplink.h"
#include "DAP_config.h"
#include "swd_host.h"
#include "target_family.h"
#include "target_board.h"

// Stub families
const target_family_descriptor_t g_hw_reset_family = {
    .family_id = kStub_HWReset_FamilyID,
    .default_reset_type = kHardwareReset,
};

const target_family_descriptor_t g_sw_vectreset_family = {
    .family_id = kStub_SWVectReset_FamilyID,
    .default_reset_type = kSoftwareReset,
    .soft_reset_type = VECTRESET,
};

const target_family_descriptor_t g_sw_sysresetreq_family = {
    .family_id = kStub_SWSysReset_FamilyID,
    .default_reset_type = kSoftwareReset,
    .soft_reset_type = SYSRESETREQ,
};

//Weakly define family
__attribute__((weak))
const target_family_descriptor_t g_nxp_kinetis_kseries = {0};
__attribute__((weak))
const target_family_descriptor_t g_nxp_kinetis_lseries = {0};
__attribute__((weak))
const target_family_descriptor_t g_nxp_kinetis_k32w_series = {0};
__attribute__((weak))
const target_family_descriptor_t g_nxp_mimxrt = {0};
__attribute__((weak))
const target_family_descriptor_t g_nxp_rapid_iot = {0};
__attribute__((weak))
const target_family_descriptor_t  g_nordic_nrf51  = {0};
__attribute__((weak))
const target_family_descriptor_t  g_nordic_nrf52  = {0};
__attribute__((weak))
const target_family_descriptor_t g_realtek_rtl8195am  = {0};
__attribute__((weak))
const target_family_descriptor_t g_ti_family  = {0};
__attribute__((weak))
const target_family_descriptor_t g_wiznet_family  = {0};
__attribute__((weak))
const target_family_descriptor_t g_renesas_family  = {0};
__attribute__((weak))
const target_family_descriptor_t g_toshiba_tz_family  = {0};

__attribute__((weak))
const target_family_descriptor_t *g_families[] = {
    &g_hw_reset_family,
    &g_sw_vectreset_family,
    &g_sw_sysresetreq_family,
    &g_nxp_kinetis_kseries,
    &g_nxp_kinetis_lseries,
    &g_nxp_kinetis_k32w_series,
    &g_nxp_mimxrt,
    &g_nxp_rapid_iot,
    &g_nordic_nrf51,
    &g_nordic_nrf52,
    &g_realtek_rtl8195am,
    &g_ti_family,
    &g_wiznet_family,
    &g_renesas_family,
    &g_toshiba_tz_family,
    0 // list terminator
};

__attribute__((weak))
const target_family_descriptor_t *g_target_family = NULL;


void init_family(void)
{
    uint8_t index = 0;
    uint16_t family_id = get_family_id();
    if (g_target_family != NULL){ //already set
        return;
    }

    while (g_families[index]!=0) {
        if (g_families[index]->family_id && (g_families[index]->family_id == family_id)) {
            g_target_family = g_families[index];
            break;
        }
        index++;
    }

    if(g_target_family == NULL){ //default family
        g_target_family = &g_hw_reset_family;
    }
}

uint8_t target_set_state(target_state_t state)
{
    if (g_board_info.target_set_state) { //target specific
        g_board_info.target_set_state(state);
    }
    if (g_target_family) {
        if (g_target_family->target_set_state) {
            //customize target state
            return g_target_family->target_set_state(state);
        } else {
            if (g_target_family->default_reset_type == kHardwareReset) {
                return swd_set_target_state_hw(state);
            } else if (g_target_family->default_reset_type == kSoftwareReset) {
                if (g_board_info.soft_reset_type) { //board has precedence
                    swd_set_soft_reset(g_board_info.soft_reset_type);
                } else if (g_target_family->soft_reset_type) {
                    swd_set_soft_reset(g_target_family->soft_reset_type);
                }
                return swd_set_target_state_sw(state);
            }else {
                return 1;
            }
        }
    }else{
        return 0;
    }
}

void swd_set_target_reset(uint8_t asserted)
{
    if (g_target_family && g_target_family->swd_set_target_reset) {
        g_target_family->swd_set_target_reset(asserted);
    }else {
        (asserted) ? PIN_nRESET_OUT(0) : PIN_nRESET_OUT(1);
    }
}

uint32_t target_get_apsel()
{
    if (g_target_family && g_target_family->apsel) {
        return g_target_family->apsel;
    } else {
        return 0;
    }
}
