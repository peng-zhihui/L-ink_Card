/**
 * @file    validation.c
 * @brief   Implementation of validation.h
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
#include "validation.h"
#include "target_config.h"
#include "target_family.h"
#include "target_board.h"

static inline uint32_t test_range(const uint32_t test, const uint32_t min, const uint32_t max)
{
    return ((test < min) || (test > max)) ? 0 : 1;
}

uint8_t validate_bin_nvic(const uint8_t *buf)
{
    if (g_target_family && g_target_family->validate_bin_nvic) {
        return g_target_family && g_target_family->validate_bin_nvic(buf);
    } else if (g_board_info.target_cfg) {
        uint32_t i = 4, nvic_val = 0;
        uint8_t in_range = 0;
        // test the initial SP value
        memcpy(&nvic_val, buf + 0, sizeof(nvic_val));

        region_info_t * region = g_board_info.target_cfg->ram_regions;
        for (; region->start != 0 || region->end != 0; ++region) {
            if (1 == test_range(nvic_val, region->start, region->end)) {
                in_range = 1;
                break;
            }
        }

        if (in_range == 0) {
            return 0;
        }

        // Reset_Handler
        // NMI_Handler
        // HardFault_Handler
        for (; i <= 12; i += 4) {
            in_range = 0;
            memcpy(&nvic_val, buf + i, sizeof(nvic_val));
            region_info_t * region = g_board_info.target_cfg->flash_regions;
            for (; region->start != 0 || region->end != 0; ++region) {
                if (1 == test_range(nvic_val, region->start, region->end)) {
                    in_range = 1;
                    break;
                }
            }
            if (in_range == 0) {
                return 0;
            }
        }

        return 1;

    } else {
        return 0;
    }
}

uint8_t validate_hexfile(const uint8_t *buf)
{
    if (g_target_family && g_target_family->validate_hexfile) {
        return g_target_family->validate_hexfile(buf);
    } else {
        // look here for known hex records
        // add hex identifier b[0] == ':' && b[8] == {'0', '2', '3', '4', '5'}
        return ((buf[0] == ':') && ((buf[8] == '0') || (buf[8] == '2') || (buf[8] == '3') || (buf[8] == '4') || (buf[8] == '5'))) ? 1 : 0;
    }
}
