/**
 * @file    frdmk32w042.c
 * @brief   board ID for the NXP FRDM-K32W042 board
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2017-2019, ARM Limited, All Rights Reserved
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

#include "target_board.h"
#include "target_family.h"

const board_info_t g_board_info = {
    .info_version = kBoardInfoVersion,
    .board_id = "0225",
    .family_id = kNXP_KinetisK32W_FamilyID,
    .flags = kEnablePageErase,
    .daplink_url_name =   "PRODINFOHTM",
    .daplink_drive_name = "K32W042",
    .daplink_target_url = "http://www.nxp.com/frdm-k32w042",
    .target_cfg = &target_device,
};
