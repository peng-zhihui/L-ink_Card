/**
 * @file    target_reset.c
 * @brief   Target reset for the W7500
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
#include "swd_host.h"
#include "target_family.h"
#include "target_board.h"

static uint8_t target_set_state_wiznet(target_state_t state) {
    uint8_t status;

    if( state == RESET_RUN )
    {
        do
        {
            swd_set_target_reset(1);
            osDelay(2);
            swd_set_target_reset(0);
            osDelay(2);
        } while(!swd_init_debug());

        swd_off();
        status = 1;
    }
    else
    {
        status = swd_set_target_state_sw(state);
    }

    return status;
}


const target_family_descriptor_t g_wiznet_family = {
    .family_id = kWiznet_W7500_FamilyID,
    .target_set_state = target_set_state_wiznet,
};
