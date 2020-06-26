/**
 * @file    target_reset_Kseries.c
 * @brief   Target reset for the Kinetis K series
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

#define MDM_STATUS  0x01000000
#define MDM_CTRL    0x01000004
#define MDM_IDR     0x010000fc
#define MDM_ID      0x001c0000 // K64, K22 (K series)

static void target_before_init_debug(void)
{
    swd_set_target_reset(1);
}

static uint8_t target_unlock_sequence(void)
{
    uint32_t val;

    // read the device ID
    if (!swd_read_ap(MDM_IDR, &val)) {
        return 0;
    }

    // verify the result
    if (val != MDM_ID) {
        return 0;
    }

    if (!swd_read_ap(MDM_STATUS, &val)) {
        return 0;
    }

    // flash in secured mode
    if (val & (1 << 2)) {
        // hold the device in reset
        swd_set_target_reset(1);

        // write the mass-erase enable bit
        if (!swd_write_ap(MDM_CTRL, 1)) {
            return 0;
        }

        while (1) {
            // wait until mass erase is started
            if (!swd_read_ap(MDM_STATUS, &val)) {
                return 0;
            }

            if (val & 1) {
                break;
            }
        }

        // mass erase in progress
        while (1) {
            // keep reading until procedure is complete
            if (!swd_read_ap(MDM_CTRL, &val)) {
                return 0;
            }

            if (val == 0) {
                break;
            }
        }
    }

    return 1;
}

// Check Flash Configuration Field bytes at address 0x400-0x40f to ensure that flash security
// won't be enabled.
//
// FCF bytes:
// [0x0-0x7]=backdoor key
// [0x8-0xb]=flash protection bytes
// [0xc]=FSEC:
//      [7:6]=KEYEN (2'b10 is backdoor key enabled, all others backdoor key disabled)
//      [5:4]=MEEN (2'b10 mass erase disabled, all other mass erase enabled)
//      [3:2]=FSLACC (2'b00 and 2'b11 factory access enabled, 2'b01 and 2'b10 factory access disabled)
//      [1:0]=SEC (2'b10 flash security disabled, all other flash security enabled)
// [0xd]=FOPT
// [0xe]=EEPROM protection bytes (FlexNVM devices only)
// [0xf]=data flash protection bytes (FlexNVM devices only)
//
// This function checks that:
// - FSEC does not disable mass erase or secure the device.
//
static uint8_t security_bits_set(uint32_t addr, uint8_t *data, uint32_t size)
{
    const uint32_t fsec_addr = 0x40C;

    if ((addr <= fsec_addr) && (addr + size) > fsec_addr) {
        uint8_t fsec = data[fsec_addr - addr];

        // make sure we can unsecure the device or dont program at all
        if ((fsec & 0x30) == 0x20) {
            // Dont allow programming mass-erase disabled state
            return 1;
        }

        // Security is OK long as we can mass-erase (comment the following out to enable target security)
        if ((fsec & 0x03) != 0x02) {
            return 1;
        }
    }

    return 0;
}

const target_family_descriptor_t g_nxp_kinetis_kseries = {
    .family_id = kNXP_KinetisK_FamilyID,
    .default_reset_type = kHardwareReset,
    .target_before_init_debug = target_before_init_debug,
    .target_unlock_sequence = target_unlock_sequence,
    .security_bits_set = security_bits_set,
};
