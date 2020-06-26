
/**
 * @file    target.c
 * @brief   Target information for the Musca B qspi
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
#include "region_defs.h"

// The file flash_blob.c must only be included in target.c
#include "flash_blob.c"


// target information
target_cfg_t target_device = {
    .sectors_info                = sectors_info,
    .sector_info_length          = (sizeof(sectors_info))/(sizeof(sector_info_t)),

    .flash_regions[0].start      = (MUSCA_B_NS_FLASH_START),
    .flash_regions[0].end        = (MUSCA_B_NS_FLASH_START + MUSCA_B_FLASH_SIZE),
#if (defined MUSCA_B_BOOT_QSPI_FLASH)
    .flash_regions[0].flags      = kRegionIsDefault,
#endif
    .flash_regions[0].flash_algo = (program_target_t *) &qspi_flash,

    .flash_regions[1].start      = (MUSCA_B_S_FLASH_START),
    .flash_regions[1].end        = (MUSCA_B_S_FLASH_START + MUSCA_B_FLASH_SIZE),
    .flash_regions[1].flash_algo = (program_target_t *) &qspi_flash,

    .flash_regions[2].start      = (MUSCA_B_NS_EFLASH0_START),
    .flash_regions[2].end        = (MUSCA_B_NS_EFLASH0_START + MUSCA_B_EFLASH0_SIZE),
    .flash_regions[2].flash_algo = (program_target_t *) &eflash,
#if (defined MUSCA_B_BOOT_EFLASH0)
    .flash_regions[2].flags      = kRegionIsDefault,
#endif

    .flash_regions[3].start      = (MUSCA_B_S_EFLASH0_START),
    .flash_regions[3].end        = (MUSCA_B_S_EFLASH0_START + MUSCA_B_EFLASH0_SIZE),
    .flash_regions[3].flash_algo = (program_target_t *) &eflash,

    .flash_regions[4].start      = (MUSCA_B_NS_EFLASH1_START),
    .flash_regions[4].end        = (MUSCA_B_NS_EFLASH1_START + MUSCA_B_EFLASH1_SIZE),
    .flash_regions[4].flash_algo = (program_target_t *) &eflash,
#if (defined MUSCA_B_BOOT_EFLASH1)
    .flash_regions[4].flags      = kRegionIsDefault,
#endif

    .flash_regions[5].start      = (MUSCA_B_S_EFLASH1_START),
    .flash_regions[5].end        = (MUSCA_B_S_EFLASH1_START + MUSCA_B_EFLASH1_SIZE),
    .flash_regions[5].flash_algo = (program_target_t *) &eflash,

    .flash_regions[6].start      = (MUSCA_B_NS_CODE_SRAM_START),
    .flash_regions[6].end        = (MUSCA_B_NS_CODE_SRAM_START + MUSCA_B_CODE_SRAM_SIZE),
#if (defined MUSCA_B_BOOT_CODE_SRAM)
    .flash_regions[6].flags      = kRegionIsDefault,
#endif

    .flash_regions[7].start      = (MUSCA_B_S_CODE_SRAM_START),
    .flash_regions[7].end        = (MUSCA_B_S_CODE_SRAM_START + MUSCA_B_CODE_SRAM_SIZE),

    .ram_regions[0].start        = (MUSCA_B_NS_RAM_START),
    .ram_regions[0].end          = (MUSCA_B_NS_RAM_START + MUSCA_B_RAM_SIZE),
    .ram_regions[1].start        = (MUSCA_B_S_RAM_START),
    .ram_regions[1].end          = (MUSCA_B_S_RAM_START + MUSCA_B_RAM_SIZE),
};
