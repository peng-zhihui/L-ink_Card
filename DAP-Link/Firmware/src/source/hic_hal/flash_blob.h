/**
 * @file    flash_blob.h
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
 * Copyright 2019, Cypress Semiconductor Corporation 
 * or a subsidiary of Cypress Semiconductor Corporation.
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

#ifndef FLASH_BLOB_H
#define FLASH_BLOB_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Flags for program_target
enum { 
    kAlgoVerifyReturnsAddress = (1u << 0u),     /*!< Verify function returns address if bit set */
    kAlgoSingleInitType =       (1u << 1u),     /*!< The init function ignores the function code. */
    kAlgoSkipChipErase =        (1u << 2u),     /*!< Skip region when erase.act action triggers. */
};

typedef struct __attribute__((__packed__)) {
    uint32_t breakpoint;
    uint32_t static_base;
    uint32_t stack_pointer;
} program_syscall_t;

typedef struct __attribute__((__packed__)) {
    const uint32_t  init;
    const uint32_t  uninit;
    const uint32_t  erase_chip;
    const uint32_t  erase_sector;
    const uint32_t  program_page;
    const uint32_t  verify;
    const program_syscall_t sys_call_s;
    const uint32_t  program_buffer;
    const uint32_t  algo_start;
    const uint32_t  algo_size;
    const uint32_t *algo_blob;
    const uint32_t  program_buffer_size;
    const uint32_t  algo_flags;         /*!< Combination of kAlgoVerifyReturnsAddress, kAlgoSingleInitType and kAlgoSkipChipErase*/
} program_target_t;

typedef struct __attribute__((__packed__)) {
    const uint32_t start;
    const uint32_t size;
} sector_info_t;

#ifdef __cplusplus
}
#endif

#endif
