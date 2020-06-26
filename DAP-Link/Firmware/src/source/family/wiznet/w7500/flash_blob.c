/**
 * @file    flash_blob.c
 * @brief   Flash algorithm for the W7500
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

#include "flash_blob.h"
static const uint32_t w7500_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x4d27b430, 0x606c2400, 0x1e644d26, 0x4c26602c, 0x086d6925, 0x6125006d, 0x46a44c24, 0x4760bc30, 
    0x47702000, 0x47702000, 0x2014b570, 0x21004c1c, 0x4d1c6061, 0x602c1e4c, 0x69224c1b, 0x006d0855, 
    0x460b6125, 0x461a4c19, 0x200047a0, 0xb570bd70, 0x49132512, 0x604a2200, 0x17494b12, 0x4c126019, 
    0x08496921, 0x61210049, 0x4c104601, 0x46284613, 0x200047a0, 0xb570bd70, 0x4b092422, 0x605d2500, 
    0x175b4d08, 0x4d08602b, 0x085e692b, 0x612e0076, 0x4601460b, 0x4c054620, 0x200047a0, 0x0000bd70, 
    0xe000ed00, 0xe000e180, 0xe000e000, 0x1fff1001, 0x00000000, 
};

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {0, 256},
};

static const program_target_t flash = {
    0x20000041, // Init
    0x20000045, // UnInit
    0x20000049, // EraseChip
    0x2000006F, // EraseSector
    0x20000097, // ProgramPage
    0x0,        // Verify
    
    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000000 + 0x00000001,
        0x20000000 + 0x00000020 + 0x000000b0,
        0x20000800
    },

    0x20000000 + 0x00000A00,   // mem buffer location
    0x20000000,                // location to write prog_blob in target RAM
    sizeof(w7500_flash_prog_blob), // prog_blob size
    w7500_flash_prog_blob,         // address of prog_blob

    // Do not modify. program_buffer_size of W7500 must be set to 0x00000100
    0x00000100                 // ram_to_flash_bytes_to_be_written
};
