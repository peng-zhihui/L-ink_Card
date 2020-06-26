/**
 * @file    target_reset.c
 * @brief   Target reset for the rza1h
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
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
#include "target_family.h"

static uint8_t target_set_state_rza(target_state_t state)
{
    return swd_set_target_state_hw(state);
}

#define BINARY_DETECTION    (0xE59FF000)

static uint8_t validate_bin_nvic_rza(const uint8_t *buf)
{
    // Very dirty hacking here for ARMv7-A (non Cortex-M) binary detection
    // This returns validated result when start instrunction
    // of the buffer is BINARY_DETECTION (LDR  PC, Label)
    // Compared with the high-order 3byte
    if ((buf[1] == ((BINARY_DETECTION >> 8)  & 0xFF))
     && (buf[2] == ((BINARY_DETECTION >> 16) & 0xFF))
     && (buf[3] == ((BINARY_DETECTION >> 24) & 0xFF))) {
        return 1;
    } else {
        return 0;
    }
}

static uint8_t validate_hexfile_rza(const uint8_t *buf)
{
    return 0;
}

const target_family_descriptor_t g_renesas_family = {
    .family_id = kRenesas_FamilyID,
    .target_set_state = target_set_state_rza,
    .validate_bin_nvic = validate_bin_nvic_rza,
    .validate_hexfile = validate_hexfile_rza,
};

