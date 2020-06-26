/**
 * @file    util.c
 * @brief   Implementation of util.h
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

#include <string.h>

#include "util.h"
#include "settings.h"
#include "cortex_m.h"

//remove dependency from vfs_manager
__attribute__((weak)) void vfs_mngr_fs_remount(void) {}

uint32_t util_write_hex8(char *str, uint8_t value)
{
    static const char nybble_chars[] = "0123456789abcdef";
    *(str + 0) = nybble_chars[(value >> 4) & 0x0F ];
    *(str + 1) = nybble_chars[(value >> 0) & 0x0F ];
    return 2;
}

uint32_t util_write_hex16(char *str, uint16_t value)
{
    uint32_t pos = 0;
    pos += util_write_hex8(str + pos, (value >> 8) & 0xFF);
    pos += util_write_hex8(str + pos, (value >> 0) & 0xFF);
    return pos;
}

uint32_t util_write_hex32(char *str, uint32_t value)
{
    uint32_t pos = 0;
    pos += util_write_hex8(str + pos, (value >> 0x18) & 0xFF);
    pos += util_write_hex8(str + pos, (value >> 0x10) & 0xFF);
    pos += util_write_hex8(str + pos, (value >> 0x08) & 0xFF);
    pos += util_write_hex8(str + pos, (value >> 0x00) & 0xFF);
    return pos;
}

uint32_t util_write_uint32(char *str, uint32_t value)
{
    uint32_t temp_val;
    uint64_t digits;
    uint32_t i;
    // Count the number of digits
    digits = 0;
    temp_val = value;

    while (temp_val > 0) {
        temp_val /= 10;
        digits += 1;
    }

    if (digits <= 0) {
        digits = 1;
    }

    // Write the number
    for (i = 0; i < digits; i++) {
        str[digits - i - 1] = '0' + (value % 10);
        value /= 10;
    }

    return digits;
}

uint32_t util_write_uint32_zp(char *str, uint32_t value, uint16_t total_size)
{
    uint32_t size;
    // Get the size of value
    size = util_write_uint32(str, value);

    if (size >= total_size) {
        return size;
    }

    // Zero fill
    memset(str, '0', total_size);
    // Write value
    util_write_uint32(str + (total_size - size), value);
    return total_size;
}

uint32_t util_write_string(char *str, const char *data)
{
    uint32_t pos = 0;

    while (0 != data[pos]) {
        str[pos] = data[pos];
        pos++;
    }

    return pos;
}

uint32_t util_div_round_up(uint32_t dividen, uint32_t divisor)
{
    return (dividen + divisor - 1) / divisor;
}

uint32_t util_div_round_down(uint32_t dividen, uint32_t divisor)
{
    return dividen / divisor;
}

uint32_t util_div_round(uint32_t dividen, uint32_t divisor)
{
    return (dividen + divisor / 2) / divisor;
}

void _util_assert(bool expression, const char *filename, uint16_t line)
{
    bool assert_set;
    cortex_int_state_t int_state;

    if (expression) {
        return;
    }

    int_state = cortex_int_get_and_disable();
    // Only write the assert if there is not already one
    assert_set = config_ram_get_assert(0, 0, 0, 0);

    if (!assert_set) {
        config_ram_set_assert(filename, line);
    }

    cortex_int_restore(int_state);

    // Start a remount if this is the first assert
    // Do not call vfs_mngr_fs_remount from an ISR!
    if (!assert_set && !cortex_in_isr()) {
        vfs_mngr_fs_remount();
    }
}

void util_assert_clear()
{
    cortex_int_state_t int_state;
    int_state = cortex_int_get_and_disable();
    config_ram_clear_assert();
    cortex_int_restore(int_state);
}
