/**
 * @file    bootloader_update.c
 * @brief   Logic to perform a bootloader update when enabled
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2016-2019, ARM Limited, All Rights Reserved
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

#ifdef DRAG_N_DROP_SUPPORT
#include <stdbool.h>
#include <string.h>
#include "flash_manager.h"
#include "util.h"
#include "bootloader.h"
#include "info.h"
#include "daplink.h"
#include "crc.h"

// Supress the warning 'null argument provided for parameter marked with attribute "nonnull"'
// since the vector table is at address 0
#pragma diag_suppress 2748

#if !defined(DAPLINK_BOOTLOADER_UPDATE)
    #define DAPLINK_BOOTLOADER_UPDATE       0
#endif

#if DAPLINK_BOOTLOADER_UPDATE
    // The bootloader must be built first or this header will not be found
    #include "bootloader_image.c"
#else //DAPLINK_BOOTLOADER_UPDATE
    static const unsigned int image_start = 0;
    static const unsigned int image_size = 0;
    static const char image_data[1];
#endif //DAPLINK_BOOTLOADER_UPDATE

static bool interface_image_valid()
{
    uint32_t stored_crc;
    uint32_t computed_crc;
    
    stored_crc = *(uint32_t *)(DAPLINK_ROM_IF_START + DAPLINK_ROM_IF_SIZE - 4);
    computed_crc = crc32((void *)DAPLINK_ROM_IF_START, DAPLINK_ROM_IF_SIZE - 4);
    return computed_crc == stored_crc;
}

void bootloader_check_and_update(void)
{
    int same;
    error_t ret;
    bool update_present = image_size > 0;

    if (!update_present) {
        return;
    }

    if (info_get_bootloader_present() &&
            (info_get_bootloader_version() > DAPLINK_VERSION)) {
        // Bootloader is more recent than the one we have so
        // don't change it
        return;
    }

    if (!interface_image_valid()) {
        // The interface is corrupt so don't attempt
        // to apply the update
        util_assert(0);
        return;
    }

    same = memcmp((void*)image_start, image_data, image_size) == 0;
    if (!same) {
        ret = flash_manager_init(flash_intf_iap_protected);
        if (ret != ERROR_SUCCESS) {
            util_assert(0);
            return;
        }

        ret = flash_manager_data(image_start, (const uint8_t*)image_data, image_size);
        if (ret != ERROR_SUCCESS) {
            flash_manager_uninit();
            util_assert(0);
            return;
        }

        ret = flash_manager_uninit();
        if (ret != ERROR_SUCCESS) {
            util_assert(0);
            return;
        }
    }
}
#else //DRAG_N_DROP_SUPPORT

void bootloader_check_and_update(void) {}

#endif //DRAG_N_DROP_SUPPORT
