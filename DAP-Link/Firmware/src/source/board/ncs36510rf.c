/* CMSIS-DAP Interface Firmware
 * Copyright (c) 2009-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "uart.h"
#include "target_board.h"
#include "target_family.h"

static void prerun_board_config()
{
    uart_enable_flow_control(false);
}

const board_info_t g_board_info = {
    .info_version = kBoardInfoVersion,
    .board_id = "1200",
    .family_id = kStub_SWSysReset_FamilyID,
    .prerun_board_config = prerun_board_config,
    .target_cfg = &target_device,
};
