/**
 * @file    target_reset_K32W_series.c
 * @brief   Target reset for the Kinetis K32W series
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

#include "swd_host.h"
#include "info.h"
#include "target_family.h"

#define MDM_STATUS  0x01000000
#define MDM_CTRL    0x01000004
#define MDM_IDR     0x010000fc
#define MDM_ID      0x001c0040 // K32 series

#define MDM_STATUS_FLASH_MASS_ERASE_ACKNOWLEDGE (1 << 0)
#define MDM_STATUS_FLASH_READY (1 << 1)
#define MDM_STATUS_SYSTEM_SECURITY (1 << 2)
#define MDM_STATUS_MASS_ERASE_ENABLE (1 << 5)

#define MDM_CTRL_FLASH_MASS_ERASE_IN_PROGRESS (1 << 0)
#define MDM_CTRL_SYSTEM_RESET_REQUEST (1 << 3)

#define TIMEOUT_COUNT (1000000)

void target_before_init_debug(void)
{
    swd_set_target_reset(1);
}

uint8_t target_unlock_sequence(void)
{
    uint32_t val;
    uint32_t timeoutCounter = 0;

    // read the device ID
    if (!swd_read_ap(MDM_IDR, &val)) {
        return 0;
    }

    // verify the result
    if (val != MDM_ID) {
        return 0;
    }

    // Wait until flash is ready.
    do {
        if (!swd_read_ap(MDM_STATUS, &val)) {
            return 0;
        }

        if (++timeoutCounter > TIMEOUT_COUNT) {
            return 0;
        }
    } while (!(val & MDM_STATUS_FLASH_READY));

    // Check if security is enabled.
    if (!swd_read_ap(MDM_STATUS, &val)) {
        swd_set_target_reset(0);
        return 0;
    }

    // flash in secured mode
    if (val & MDM_STATUS_SYSTEM_SECURITY) {
        // Make sure mass erase is enabled.
        if (!(val & MDM_STATUS_MASS_ERASE_ENABLE)) {
            return 0;
        }

        // hold the device in reset
        swd_set_target_reset(1);

        // Write the mass-erase enable and system reset request bits.
        if (!swd_write_ap(MDM_CTRL, (MDM_CTRL_FLASH_MASS_ERASE_IN_PROGRESS | MDM_CTRL_SYSTEM_RESET_REQUEST))) {
            swd_set_target_reset(0);
            return 0;
        }

        // Verify mass erase has started.
        timeoutCounter = 0;
        do {
            // wait until mass erase is started
            if (!swd_read_ap(MDM_STATUS, &val)) {
                swd_set_target_reset(0);
                return 0;
            }

            if (++timeoutCounter > TIMEOUT_COUNT) {
                swd_write_ap(MDM_CTRL, 0);
                swd_set_target_reset(0);
                return 0;
            }
        } while (!(val & MDM_STATUS_FLASH_MASS_ERASE_ACKNOWLEDGE));

        // Wait until mass erase completes.
        timeoutCounter = 0;
        do {
            // keep reading until procedure is complete
            if (!swd_read_ap(MDM_CTRL, &val)) {
                swd_set_target_reset(0);
                return 0;
            }

            if (++timeoutCounter > TIMEOUT_COUNT) {
                swd_write_ap(MDM_CTRL, 0);
                swd_set_target_reset(0);
                return 0;
            }
        } while (val & MDM_CTRL_FLASH_MASS_ERASE_IN_PROGRESS);

        // Confirm the mass erase was successful.
        if (!swd_read_ap(MDM_STATUS, &val)) {
            swd_set_target_reset(0);
            return 0;
        }

        // Release the device from reset.
        swd_write_ap(MDM_CTRL, 0);
        swd_set_target_reset(0);

        if (val & MDM_STATUS_SYSTEM_SECURITY) {
            return 0;
        }
    }

    return 1;
}

const target_family_descriptor_t g_nxp_kinetis_k32w_series = {
    .family_id = kNXP_KinetisK32W_FamilyID,
    .default_reset_type = kHardwareReset,
    .target_before_init_debug = target_before_init_debug,
    .target_unlock_sequence = target_unlock_sequence,
};
