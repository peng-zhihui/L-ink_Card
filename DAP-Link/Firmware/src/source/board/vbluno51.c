/**
 * @file    vbluno51.c
 * @brief   Board ID for the VBLUno51 board (VNG Bluetooth Low Energy UNO nRF51822 board)
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

#include "uart.h"
#include "target_board.h"
#include "target_family.h"

static void vbluno_prerun_board_config()
{
    uart_enable_flow_control(false);
}

const board_info_t g_board_info = {
    .info_version = kBoardInfoVersion,
    .board_id = "C006",
    .family_id = kNordic_Nrf51_FamilyID,
    .daplink_target_url = "https://os.mbed.com/platforms/VBLUNO51/",
    .prerun_board_config = vbluno_prerun_board_config,
    .target_cfg = &target_device,
};
