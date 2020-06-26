/**
 * @file    ublox_evk_nina_b1.c
 * @brief   board ID for the UBLOX_EVK_NINA_B1
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

#include "DAP_config.h"
#include "target_board.h"
#include "target_family.h"

static void nina_swd_set_target_reset(uint8_t asserted){
    if(!asserted) {
        PIOA->PIO_MDER = PIN_SWDIO | PIN_SWCLK | PIN_nRESET;
    }
}

const board_info_t g_board_info = {
    .info_version = kBoardInfoVersion,
    .board_id = "1237",
    .family_id = kNordic_Nrf52_FamilyID,
    .swd_set_target_reset = nina_swd_set_target_reset,
    .target_cfg = &target_device,
};
