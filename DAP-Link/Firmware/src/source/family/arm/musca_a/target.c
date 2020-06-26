/**
 * @file    target.c
 * @brief   Target information for the Musca A qspi
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

#include "target_config.h"

// The file flash_blob.c must only be included in target.c
#include "flash_blob.c"

#define MUSCA_A_NS_CODE_SRAM_START  0x00000000
#define MUSCA_A_S_CODE_SRAM_START   0x10000000
#define MUSCA_A_CODE_SRAM_SIZE      MB(2)

#define MUSCA_A_NS_FLASH_START      0x00200000
#define MUSCA_A_S_FLASH_START       0x10200000
#define MUSCA_A_FLASH_SIZE          MB(8)

#define MUSCA_A_FLASH_SECTOR_SIZE   0x010000

#define MUSCA_A_NS_RAM_START        0x20000000
#define MUSCA_A_S_RAM_START         0x30000000
#define MUSCA_A_RAM_SIZE            KB(128)

// target information
target_cfg_t target_device = {
    .sectors_info                = sectors_info,
    .sector_info_length          = (sizeof(sectors_info))/(sizeof(sector_info_t)),
    .flash_regions[0].start      = (MUSCA_A_NS_FLASH_START),
    .flash_regions[0].end        = (MUSCA_A_NS_FLASH_START + MUSCA_A_FLASH_SIZE),
    .flash_regions[0].flags      = kRegionIsDefault,
    .flash_regions[0].flash_algo = (program_target_t *) &flash,

    .flash_regions[1].start      = (MUSCA_A_S_FLASH_START),
    .flash_regions[1].end        = (MUSCA_A_S_FLASH_START + MUSCA_A_FLASH_SIZE),
    .flash_regions[1].flash_algo = (program_target_t *) &flash,

    .flash_regions[2].start      = (MUSCA_A_NS_CODE_SRAM_START),
    .flash_regions[2].end        = (MUSCA_A_NS_CODE_SRAM_START + MUSCA_A_CODE_SRAM_SIZE),
    .flash_regions[3].start      = (MUSCA_A_S_CODE_SRAM_START),
    .flash_regions[3].end        = (MUSCA_A_S_CODE_SRAM_START + MUSCA_A_CODE_SRAM_SIZE),

    .ram_regions[0].start        = (MUSCA_A_NS_RAM_START),
    .ram_regions[0].end          = (MUSCA_A_NS_RAM_START + MUSCA_A_RAM_SIZE),
    .ram_regions[1].start        = (MUSCA_A_S_RAM_START),
    .ram_regions[1].end          = (MUSCA_A_S_RAM_START + MUSCA_A_RAM_SIZE),
};
