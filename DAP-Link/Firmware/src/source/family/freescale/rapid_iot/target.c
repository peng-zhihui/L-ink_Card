/**
 * @file    target.c
 * @brief   Target information for the rapid-iot
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

#include "target_config.h"

// The file flash_blob.c must only be included in target.c
#include "flash_blob_k64.c"
#include "flash_blob_kw40.c"
#include "flash_blob_kw41.c"

// default target information is k64f
target_cfg_t target_device = {
    .sectors_info                   = sectors_info_k64,
    .sector_info_length             = (sizeof(sectors_info_k64))/(sizeof(sector_info_t)),
#ifdef BOARD_RAPID_IOT
    .flash_regions[0].start         = 0x00014000,
#else
    .flash_regions[0].start         = 0x00000000,
#endif    
    .flash_regions[0].end           = MB(1),
    .flash_regions[0].flags         = kRegionIsDefault,
    .flash_regions[0].flash_algo    = (program_target_t *) &flash_k64,    
    .ram_regions[0].start           = 0x20000000,
    .ram_regions[0].end             = 0x20030000,
};

target_cfg_t target_device_kw40 = {
    .sectors_info                   = sectors_info_kw40,
    .sector_info_length             = (sizeof(sectors_info_kw40))/(sizeof(sector_info_t)),
    .flash_regions[0].start         = 0,
    .flash_regions[0].end           = KB(160),
    .flash_regions[0].flags         = kRegionIsDefault,
    .flash_regions[0].flash_algo    = (program_target_t *) &flash_kw40,    
    .ram_regions[0].start           = 0x1FFFF000,
    .ram_regions[0].end             = 0x20004000,
};

target_cfg_t target_device_kw41 = {
    .sectors_info                   = sectors_info_kw41,
    .sector_info_length             = (sizeof(sectors_info_kw41))/(sizeof(sector_info_t)),
    .flash_regions[0].start         = 0x4000,
    .flash_regions[0].end           = KB(512),
    .flash_regions[0].flags         = kRegionIsDefault,
    .flash_regions[0].flash_algo    = (program_target_t *) &flash_kw41,    
    .ram_regions[0].start           = 0x1FFF8000,
    .ram_regions[0].end             = 0x20018000,
};
