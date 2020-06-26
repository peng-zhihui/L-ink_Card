/**
 * @file    target_reset_mimxrt.c
 * @brief   Target reset for the i.MX RT series
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
#include "info.h"
#include "target_family.h"
#include "cmsis_os2.h"

static void target_before_init_debug(void)
{
    // This is for the hardware conflict (the EVK are not consider >2 debugger connection
    // situation) with another external debugger(such as JLINK). Before drag&pull, issue a
    // hardware reset to bring the platform to a known state and also force
    // RESET pin to high state ensure a successfully access.
    swd_set_target_reset(1);
    osDelay(5);
    swd_set_target_reset(0);
    osDelay(5);
}

static void prerun_target_config(void)
{
    // In some case the CPU will enter "cannot debug" state (low power, SWD pin mux changed, etc.).
    // Doing a hardware reset will clear those states (probably, depends on app). Also, if the
    // external flash's data is not a valid bootable image, DAPLink cannot attached to target. A
    // hardware reset will increase the chance to connect in this situation.
    target_set_state(RESET_RUN);
}

static uint8_t validate_bin_nvic(const uint8_t *buf)
{
    if(buf[0] == 'F' && buf[1] == 'C' && buf[2] == 'F' && buf[3] == 'B')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

const target_family_descriptor_t g_nxp_mimxrt = {
    .family_id = kNXP_Mimxrt_FamilyID,
    .default_reset_type = kSoftwareReset,
    .soft_reset_type = VECTRESET,
    .target_before_init_debug = target_before_init_debug,
    .prerun_target_config = prerun_target_config,
    .validate_bin_nvic = validate_bin_nvic,
};
