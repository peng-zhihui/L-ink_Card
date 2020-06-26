/**
 * @file    daplink.h
 * @brief   Information about different Hardware Interface Circuits the firmware runs
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

#ifndef DAPLINK_H
#define DAPLINK_H

#include <stdbool.h>
#include <stdint.h>

#include "daplink_addr.h"
#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

// Assert that regions are sequential with no gaps
// and that they take up all available space

// ROM check
COMPILER_ASSERT(DAPLINK_ROM_BL_START == DAPLINK_ROM_START);
COMPILER_ASSERT(DAPLINK_ROM_BL_START + DAPLINK_ROM_BL_SIZE == DAPLINK_ROM_CONFIG_ADMIN_START);
COMPILER_ASSERT(DAPLINK_ROM_CONFIG_ADMIN_START + DAPLINK_ROM_CONFIG_ADMIN_SIZE == DAPLINK_ROM_IF_START);
COMPILER_ASSERT(DAPLINK_ROM_IF_START + DAPLINK_ROM_IF_SIZE == DAPLINK_ROM_CONFIG_USER_START);
COMPILER_ASSERT(DAPLINK_ROM_CONFIG_USER_START + DAPLINK_ROM_CONFIG_USER_SIZE == DAPLINK_ROM_START + DAPLINK_ROM_SIZE);

// RAM check
COMPILER_ASSERT(DAPLINK_RAM_APP_START == DAPLINK_RAM_START);
COMPILER_ASSERT(DAPLINK_RAM_APP_START + DAPLINK_RAM_APP_SIZE == DAPLINK_RAM_SHARED_START);
COMPILER_ASSERT(DAPLINK_RAM_SHARED_START + DAPLINK_RAM_SHARED_SIZE == DAPLINK_RAM_START + DAPLINK_RAM_SIZE);

#define DAPLINK_BUILD_KEY_IF        0x9B939E8F
#define DAPLINK_BUILD_KEY_BL        0x9B939D93

//! @name HIC IDs
//@{
#define DAPLINK_HIC_ID_K20DX        0x97969900
#define DAPLINK_HIC_ID_KL26         0x97969901
#define DAPLINK_HIC_ID_LPC11U35     0x97969902
#define DAPLINK_HIC_ID_SAM3U2C      0x97969903
#define DAPLINK_HIC_ID_MAX32620     0x97969904
#define DAPLINK_HIC_ID_LPC4322      0x97969905
#define DAPLINK_HIC_ID_MAX32625     0x97969906
#define DAPLINK_HIC_ID_MAX32550     0x97969907
#define DAPLINK_HIC_ID_STM32F103XB  0x97969908
#define DAPLINK_HIC_ID_K26F         0x97969909
#define DAPLINK_HIC_ID_K22F         0x9796990A
#define DAPLINK_HIC_ID_KL27Z        0x9796990B // reserving for future use
#define DAPLINK_HIC_ID_LPC54606     0x9796990C // reserving for future use
#define DAPLINK_HIC_ID_STM32F723IE  0x9796990D // reserving for future use
#define DAPLINK_HIC_ID_LPC55S69     0x97969920 // reserving for future use
#define DAPLINK_HIC_ID_M48SSIDAE    0x97969921
#define DAPLINK_HIC_ID_PSOC5        0x2E127069
//@}

#define DAPLINK_INFO_OFFSET         0x20

typedef struct {
    uint32_t build_key;
    uint32_t hic_id;
    uint32_t version;
} daplink_info_t;

bool daplink_is_bootloader(void);
bool daplink_is_interface(void);

#ifdef __cplusplus
}
#endif

#endif
