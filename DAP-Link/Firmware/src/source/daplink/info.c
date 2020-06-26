/**
 * @file    info.c
 * @brief   Implementation of info.h
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

#include <string.h>
#include "main.h"
#include "info.h"
#include "target_config.h"
#include "read_uid.h"
#include "util.h"
#include "crc.h"
#include "daplink.h"
#include "settings.h"
#include "target_board.h"

static char hex_to_ascii(uint8_t x)
{
    return ('0' + (x>9 ? x+0x27 : x));
}

// Constant variables
static const daplink_info_t *const info_bl = (daplink_info_t *)(DAPLINK_ROM_BL_START + DAPLINK_INFO_OFFSET);
static const daplink_info_t *const info_if = (daplink_info_t *)(DAPLINK_ROM_IF_START + DAPLINK_INFO_OFFSET);

// Raw variables
static uint32_t host_id[4];
static uint32_t target_id[4];
static uint32_t hic_id = DAPLINK_HIC_ID;

static uint32_t crc_bootloader;
static uint32_t crc_interface;
static uint32_t crc_config_admin;
static uint32_t crc_config_user;

// Strings
static char string_unique_id[48 + 1];
static char string_mac[12 + 1];
static char string_board_id[4 + 1];
static char string_family_id[4 + 1];
static char string_host_id[32 + 1];
static char string_target_id[32 + 1];
static char string_hic_id[8 + 1];
static char string_version[4 + 1];

static char usb_desc_unique_id[2 + sizeof(string_unique_id) * 2];


const char *info_get_unique_id(void)
{
    return string_unique_id;
}

const char *info_get_board_id(void)
{
    return string_board_id;
}

const char *info_get_host_id(void)
{
    return string_host_id;
}

const char *info_get_target_id(void)
{
    return string_target_id;
}

const char *info_get_hic_id(void)
{
    return string_hic_id;
}
const char *info_get_version(void)
{
    return string_version;
}
const char *info_get_mac(void)
{
    return string_mac;
}

const char *info_get_unique_id_string_descriptor(void)
{
    return usb_desc_unique_id;
}

//prevent the compiler to optimize boad and family id
#if (defined(__ICCARM__))
#pragma optimize = none
static void setup_basics(void)
#elif (defined(__CC_ARM))
#pragma push
#pragma O0
static void setup_basics(void)
#elif (!defined(__GNUC__))
/* #pragma GCC push_options */
/* #pragma GCC optimize("O0") */
static void __attribute__((optimize("O0"))) setup_basics(void)
#else
#error "Unknown compiler"
#endif

{
    uint8_t i = 0, idx = 0;
    uint16_t family_id = get_family_id();
    memset(string_board_id, 0, sizeof(string_board_id));
    memset(string_host_id, 0, sizeof(string_host_id));
    memset(string_target_id, 0, sizeof(string_target_id));
    memset(string_hic_id, 0, sizeof(string_hic_id));
    memset(string_board_id, 0, sizeof(string_board_id));
    // Host ID
    idx = 0;

    for (i = 0; i < 4; i++) {
        idx += util_write_hex32(string_host_id + idx, host_id[i]);
    }

    string_host_id[idx++] = 0;
    // Target ID
    idx = 0;

    for (i = 0; i < 4; i++) {
        idx += util_write_hex32(string_target_id + idx, target_id[i]);
    }

    string_target_id[idx++] = 0;
    // HIC ID
    idx = 0;
    idx += util_write_hex32(string_hic_id + idx, hic_id);
    string_hic_id[idx++] = 0;
    // Board ID
    memcpy(string_board_id, get_board_id(), 4);
    string_board_id[4] = 0;
    idx = 0;
    //Family ID
    string_family_id[idx++] = hex_to_ascii(((family_id >> 12) & 0xF));    
    string_family_id[idx++] = hex_to_ascii(((family_id >> 8) & 0xF));
#if !(defined(DAPLINK_BL)) &&  defined(DRAG_N_DROP_SUPPORT)   //need to change the unique id when the msd is disabled 
    #if defined(MSC_ENDPOINT)
    if (config_ram_get_disable_msd() == 1 || flash_algo_valid()==0){
        string_family_id[idx++] = hex_to_ascii((((family_id >> 4) | 0x08) & 0xF)); 
    } else {
        string_family_id[idx++] = hex_to_ascii(((family_id >> 4) & 0xF));
    }
    #else //no msd support always have the most significant bit set for family id 2nd byte
        string_family_id[idx++] = hex_to_ascii((((family_id >> 4) | 0x08) & 0xF)); 
    #endif
#else
    string_family_id[idx++] = hex_to_ascii(((family_id >> 4) & 0xF));
#endif
    string_family_id[idx++] = hex_to_ascii(((family_id) & 0xF));    
    string_family_id[idx++] = 0;
    // Version
    idx = 0;
    string_version[idx++] = '0' + (DAPLINK_VERSION / 1000) % 10;
    string_version[idx++] = '0' + (DAPLINK_VERSION / 100) % 10;
    string_version[idx++] = '0' + (DAPLINK_VERSION / 10) % 10;
    string_version[idx++] = '0' + (DAPLINK_VERSION / 1) % 10;
    string_version[idx++] = 0;
}

static void setup_unique_id()
{
    memset(string_unique_id, 0, sizeof(string_unique_id));
    strcat(string_unique_id, string_board_id);
    strcat(string_unique_id, string_family_id);
    strcat(string_unique_id, string_host_id);
    strcat(string_unique_id, string_hic_id);
}

static void setup_string_descriptor()
{
    uint8_t i = 0, idx = 0, len = 0;
    len = strlen((const char *)string_unique_id);
    // bLength
    usb_desc_unique_id[idx++] = len * 2 + 2;
    // bDescriptorType
    usb_desc_unique_id[idx++] = 3;

    // bString
    for (i = 0; i < len; i++) {
        usb_desc_unique_id[idx++] = string_unique_id[i];
        usb_desc_unique_id[idx++] = 0;
    }
}

void info_init(void)
{
    info_crc_compute();
    read_unique_id(host_id);
    setup_basics();
    setup_unique_id();
    setup_string_descriptor();
}

void info_set_uuid_target(uint32_t *uuid_data)
{
    uint32_t idx = 0;
    // Save the target ID
    memcpy(target_id, uuid_data, 16);
    // patch for MAC use. Make sure MSB bits are set correctly
    uuid_data[2] |= (0x2 << 8);
    uuid_data[2] &= ~(0x1 << 8);
    idx += util_write_hex16(string_mac + idx, uuid_data[2] & 0xFFFF);
    idx += util_write_hex32(string_mac + idx, uuid_data[3]);
    string_mac[idx++] = 0;
}

bool info_get_bootloader_present(void)
{
    bool present = true;

    if (0 == DAPLINK_ROM_BL_SIZE) {
        present = false;
    }

    if (DAPLINK_BUILD_KEY_BL != info_bl->build_key) {
        present = false;
    }

    if (DAPLINK_HIC_ID != info_bl->hic_id) {
        present = false;
    }

    return present;
}

bool info_get_interface_present(void)
{
    bool present = true;

    if (0 == DAPLINK_ROM_IF_SIZE) {
        present = false;
    }

    if (DAPLINK_BUILD_KEY_IF != info_if->build_key) {
        present = false;
    }

    if (DAPLINK_HIC_ID != info_if->hic_id) {
        present = false;
    }

    return present;
}

bool info_get_config_admin_present(void)
{
    //TODO, c1728p9 - check if admin config has been loaded
    return false;
}

bool info_get_config_user_present(void)
{
    //TODO, c1728p9 - check if user config has a valid key
    return false;
}

uint32_t info_get_crc_bootloader()
{
    return crc_bootloader;
}

uint32_t info_get_crc_interface()
{
    return crc_interface;
}

uint32_t info_get_crc_config_admin()
{
    return crc_config_admin;
}

uint32_t info_get_crc_config_user()
{
    return crc_config_user;
}

void info_crc_compute()
{
    crc_bootloader = 0;
    crc_interface = 0;
    crc_config_admin = 0;
    crc_config_user = 0;

    // Compute the CRCs of regions that exist
    if (DAPLINK_ROM_BL_SIZE > 0) {
        crc_bootloader = crc32((void *)DAPLINK_ROM_BL_START, DAPLINK_ROM_BL_SIZE - 4);
    }

    if (DAPLINK_ROM_IF_SIZE > 0) {
        crc_interface = crc32((void *)DAPLINK_ROM_IF_START, DAPLINK_ROM_IF_SIZE - 4);
    }

    if (DAPLINK_ROM_CONFIG_ADMIN_SIZE > 0) {
        crc_config_admin = crc32((void *)DAPLINK_ROM_CONFIG_ADMIN_START, DAPLINK_ROM_CONFIG_ADMIN_SIZE);
    }

    if (DAPLINK_ROM_CONFIG_USER_SIZE > 0) {
        crc_config_user = crc32((void *)DAPLINK_ROM_CONFIG_USER_START, DAPLINK_ROM_CONFIG_USER_SIZE);
    }
}

// Get version info as an integer
uint32_t info_get_bootloader_version(void)
{
    // Don't read version if image is not present
    if (!info_get_bootloader_present()) {
        return 0;
    }

    return info_bl->version;
}

uint32_t info_get_interface_version(void)
{
    // Don't read version if image is not present
    if (!info_get_interface_present()) {
        return 0;
    }

    return info_if->version;
}

#if (defined(__CC_ARM))
#pragma pop
#endif
#if (defined(__GNUC__))
/* #pragma GCC pop_options */
#endif
