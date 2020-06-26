/**
 * @file    util.h
 * @brief   useful things
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2020, ARM Limited, All Rights Reserved
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

#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdint.h>

// Protect commonly-defined macros with ifdefs, to prevent conflicts if redefined
// in imported sources (mostly vendor SDKs).

#if !defined(ARRAY_SIZE)
//! @brief Get number of elements in the array.
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

#if !defined(MIN)
#define MIN(a,b)                        ((a) < (b) ? (a) : (b))
#endif

#if !defined(MAX)
#define MAX(a,b)                        ((a) > (b) ? (a) : (b))
#endif

#define MB(size)                        ((size) * 1024 * 1024)

#define KB(size)                        ((size) * 1024)

#define ROUND_UP(value, boundary)       ((value) + ((boundary) - (value)) % (boundary))

#define ROUND_DOWN(value, boundary)     ((value) - ((value) % (boundary)))

#ifdef __cplusplus
extern "C" {
#endif

// Write the value to the address specified and return the size
uint32_t util_write_hex8(char *str, uint8_t value);
uint32_t util_write_hex16(char *str, uint16_t value);
uint32_t util_write_hex32(char *str, uint32_t value);
uint32_t util_write_uint32(char *str, uint32_t value);
uint32_t util_write_uint32(char *str, uint32_t value);
uint32_t util_write_uint32_zp(char *str, uint32_t value, uint16_t total_size);
uint32_t util_write_string(char *str, const char *data);

uint32_t util_div_round_up(uint32_t dividen, uint32_t divisor);
uint32_t util_div_round_down(uint32_t dividen, uint32_t divisor);
uint32_t util_div_round(uint32_t dividen, uint32_t divisor);

#if !(defined(DAPLINK_NO_ASSERT_FILENAMES) && defined(DAPLINK_BL))
// With the filename enabled.
#define util_assert(expression) _util_assert((expression), __FILE__, __LINE__)
#else
// Filename disabled to save code size.
#define util_assert(expression) _util_assert((expression), "(file)", __LINE__)
#endif
void _util_assert(bool expression, const char *filename, uint16_t line);

void util_assert_clear(void);

#ifdef __cplusplus
}
#endif

#endif
