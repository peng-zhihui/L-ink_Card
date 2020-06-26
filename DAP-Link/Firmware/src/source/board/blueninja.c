/**
 * @file    blueninja_sb.c
 * @brief   board ID for the Cerevo BlueNinja sub board & specific functions.
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
#include "cmsis_os2.h"
#include "IO_Config.h"
#include "swd_host.h"
#include "target_family.h"
#include "target_board.h"

static uint8_t target_set_state_by_board(target_state_t state)
{
    
    if (RESET_PROGRAM == state) {
        LPC_GPIO->SET[PIN_PWH_PORT] = PIN_PWH;
        osDelay(10);
    }
    return 1;
}

const board_info_t g_board_info = {
    .info_version = kBoardInfoVersion,
    .board_id = "7010",
    .family_id = kToshiba_Tz_FamilyID,
    .target_set_state = target_set_state_by_board,
    .target_cfg = &target_device,
};
