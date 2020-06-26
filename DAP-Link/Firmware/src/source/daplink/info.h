/**
 * @file    info.h
 * @brief   methods to get information about the board
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

#ifndef INFO_H
#define INFO_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void info_init(void);
void info_set_uuid_target(uint32_t *uuid_data);
void info_crc_compute(void);


// Get the 48 digit unique ID as a null terminated string.
// This is the string used as the USB serial number.
// This string is made up of the following:
// <board ID><family ID><host ID><hic ID>
const char *info_get_unique_id(void);

// Get the 4 digit board ID as a null terminated string
const char *info_get_board_id(void);

// Get the 32 digit ID of the processor running daplink as a null terminated string
const char *info_get_host_id(void);

// Get the 32 digit ID of the target processor as a null terminated string
const char *info_get_target_id(void);

// Get the 8 digit hic ID as a null terminated string
const char *info_get_hic_id(void);

// Get the 4 digit version ID as a null terminated string
const char *info_get_version(void);

// Get the 12 digit mac as a null terminated string
const char *info_get_mac(void);


// Get the string descriptor for for the unique_id
// This is in the format of a USB string descriptor
// offset 0, size 1, bLength
// offset 1, size 1, bDescriptorType always 3
// offset 2, size n, bString - unicode encoded unique id
const char *info_get_unique_id_string_descriptor(void);


// Check if various regions are present
bool info_get_bootloader_present(void);
bool info_get_interface_present(void);
bool info_get_config_admin_present(void);
bool info_get_config_user_present(void);

// Get the CRCs of various regions.
// The CRC returned is only valid if
// the given region is present.
uint32_t info_get_crc_bootloader(void);
uint32_t info_get_crc_interface(void);
uint32_t info_get_crc_config_admin(void);
uint32_t info_get_crc_config_user(void);

// Get version info as an integer
uint32_t info_get_bootloader_version(void);
uint32_t info_get_interface_version(void);

#ifdef __cplusplus
}
#endif

#endif
