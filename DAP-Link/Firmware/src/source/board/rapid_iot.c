/**
 * @file    rapiod_iot.c
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2018, ARM Limited, All Rights Reserved
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
#include "target_board.h"
#include "target_family.h"

char *board_id_kw41z = "0234";

#define MDM_STATUS  0x01000000
#define MDM_CTRL    0x01000004
#define MDM_IDR     0x010000fc
#define MDM_ID_K64  0x001c0000 // K64
#define MDM_ID_KW4  0x001c0020 // KW4

/* Kinetis series ID */
#define K_SERIES     0
#define KW_SERIES    5

/* KW4 subfamily defines */
#define KW40         0
#define KW41         1


static void prerun_board_config(void);


const board_info_t g_board_info = {
    .info_version = kBoardInfoVersion,
    .board_id = "0228",
    .family_id = kNXP_RapidIot_FamilyID,
    .flags = kEnablePageErase,
    .daplink_url_name =       "PRODINFOHTM",
    .daplink_drive_name = 		"IOT-DAPLINK",
    .daplink_target_url = "http://www.nxp.com/rapid-iot",
    .target_cfg = &target_device,
    .prerun_board_config = prerun_board_config,
};

static void prerun_board_config(void)
{
    // SIM peripheral   0x40047000
    // address offset   0x    1024
    uint32_t SDID = 0x40048024;
    uint32_t sdid;

    // get a hold of the target
    if (target_set_state(HALT) == 0) {
        /*
         * When the Kinetis flash is empty the reset line toggles. This causes failures
         * when trying to halt the target. Use the reset halt method in this case.
         */
        target_set_state(RESET_PROGRAM);
    }

    // Read the system identification register
    swd_read_memory(SDID, (uint8_t *)&sdid, 4);

    // Set the target flash algorithm
    if (((sdid >> 20) & 0xF) == KW_SERIES) {
        //mdm_id = MDM_ID_KW4;

        if (((sdid >> 24) & 0x3) == KW40) {
            // Program to the KW40 flash
            extern target_cfg_t target_device_kw40;
            target_device = target_device_kw40;
        } else {
            // Program to the KW41 flash
            extern target_cfg_t target_device_kw41;
            target_device = target_device_kw41;
            target_device.rt_board_id = board_id_kw41z;
        }
    }

    // Let the target run
    target_set_state(RUN);
}
