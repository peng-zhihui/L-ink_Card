/**
 * @file    target_board.c
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

#include <string.h>
#include "target_board.h"

// Default empty board info.
__attribute__((weak))
const board_info_t g_board_info = {
		.info_version = kBoardInfoVersion,
		.board_id = "0000",
		.daplink_url_name =       "MBED    HTM",
		.daplink_drive_name = 		"DAPLINK    ",
		.daplink_target_url = "https://mbed.org/device/?code=@U?version=@V?target_id=@T",
};


const char * get_board_id(void)
{
    if (g_board_info.target_cfg && g_board_info.target_cfg->rt_board_id) {
        return g_board_info.target_cfg->rt_board_id; //flexible board id
    }else{
        return g_board_info.board_id;
    }
}

uint16_t get_family_id(void)
{
    if (g_board_info.target_cfg && g_board_info.target_cfg->rt_family_id) {
        return g_board_info.target_cfg->rt_family_id; //flexible family id
    }else{
        return g_board_info.family_id;
    }
}

#if (defined(__ICCARM__))
#pragma optimize = none
uint8_t flash_algo_valid(void)
#elif (defined(__CC_ARM))
#pragma push
#pragma O0
uint8_t flash_algo_valid(void)
#elif (!defined(__GNUC__))
/* #pragma GCC push_options */
/* #pragma GCC optimize("O0") */
uint8_t __attribute__((optimize("O0"))) flash_algo_valid(void)
#else
#error "Unknown compiler"
#endif
{
    return (g_board_info.target_cfg != 0);
}

#if (defined(__CC_ARM))
#pragma pop
#endif
#if (defined(__GNUC__))
/* #pragma GCC pop_options */
#endif
