/**
 * @file    target_config.h
 * @brief
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

#ifndef TARGET_CONFIG_H
#define TARGET_CONFIG_H

#include <stddef.h>
#include <stdint.h>

#include "flash_blob.h"
#include "util.h"

//! This can vary from target to target and should be in the structure or flash blob
#define TARGET_AUTO_INCREMENT_PAGE_SIZE    (1024)

//! Additional flash and ram regions
#define MAX_REGIONS (10)

//! @brief Option flags for memory regions.
enum _region_flags {
    kRegionIsDefault = (1 << 0), /*!< Out of bounds regions will use the same flash algo if this is set */
    kRegionIsSecure  = (1 << 1), /*!< The region can only be accessed from the secure world. Only applies for TrustZone-enabled targets. */
};

/*!
 * @brief Details of a target flash or RAM memory region.
 */
typedef struct __attribute__((__packed__)) region_info {
    uint32_t start;                 /*!< Region start address. */
    uint32_t end;                   /*!< Region end address. */
    uint32_t flags;                 /*!< Flags for this region from the #_region_flags enumeration. */
    uint32_t alias_index;           /*!< Use with flags; will point to a different index if there is an alias region */
    program_target_t *flash_algo;   /*!< A pointer to the flash algorithm structure */
} region_info_t;

/*!
 * @brief Information required to program target flash.
 */
typedef struct __attribute__((__packed__)) target_cfg {
    uint32_t version;                           /*!< Target configuration version */
    const sector_info_t* sectors_info;          /*!< Sector start and length list */
    uint32_t sector_info_length;                /*!< Number of entries in the sectors_info array */
    region_info_t flash_regions[MAX_REGIONS];   /*!< Flash regions */
    region_info_t ram_regions[MAX_REGIONS];     /*!< RAM regions  */
    const char *rt_board_id;                    /*!< If assigned, this is a flexible board ID */
    uint16_t rt_family_id;                      /*!< If assigned, this is a flexible family ID */
    uint8_t erase_reset;                        /*!< Reset after performing an erase */
    uint8_t pad;
} target_cfg_t;

extern target_cfg_t target_device;


#endif
