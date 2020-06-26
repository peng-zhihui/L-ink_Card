/* CMSIS-DAP Interface Firmware
 * Copyright (c) 2015-2019 Realtek Semiconductor Corp.
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
#include <string.h>
#include "swd_host.h"
#include "DAP_config.h"
#include "target_family.h"

static void swd_set_target_reset_realtek(uint8_t asserted)
{
    if (asserted) {
        swd_write_word(0xE000ED0C, 0x05FA0004);
    }
}

static uint8_t validate_bin_nvic_realtek(const uint8_t *buf)
{
    const char header[] = {0x99, 0x99, 0x96, 0x96, 0x3F, 0xCC, 0x66, 0xFC,
                           0xC0, 0x33, 0xCC, 0x03, 0xE5, 0xDC, 0x31, 0x62};

    return !memcmp(header, buf, sizeof(header));
}

const target_family_descriptor_t g_realtek_rtl8195am = {
    .family_id = kRealtek_Rtl8195am_FamilyID,
    .default_reset_type = kHardwareReset,
    .swd_set_target_reset = swd_set_target_reset_realtek,
    .validate_bin_nvic = validate_bin_nvic_realtek,
};
