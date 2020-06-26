/* Flash OS Routines (Automagically Generated)
 * Copyright (c) 2009-2019 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "flash_blob.h"

static const uint32_t stm32l0xx_192_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x4603b510, 0x69804851, 0x0224240f, 0x4c4f4320, 0x484f61a0, 0x484f60e0, 0x484f60e0, 0x484f6120,
    0x46206120, 0x240169c0, 0x40200524, 0xd1062800, 0x4c4c484b, 0x20066020, 0x484b6060, 0x200060a0,
    0x4601bd10, 0x68404841, 0x43102202, 0x60504a3f, 0x68404610, 0x43102201, 0x60504a3c, 0x47702000,
    0x483a4601, 0x22016840, 0x43100252, 0x60504a37, 0x68404610, 0x43102208, 0x60504a34, 0x60082000,
    0x483ae002, 0x60104a37, 0x69804830, 0x0fc007c0, 0xd1f62800, 0x6840482d, 0x02522201, 0x4a2b4390,
    0x46106050, 0x22086840, 0x4a284390, 0x20006050, 0x46034770, 0x47702001, 0x4603b570, 0x303f4608,
    0x01890981, 0xe03b2500, 0x68404820, 0x02b62601, 0x4e1e4330, 0x46306070, 0x26086840, 0x4e1b4330,
    0x24406070, 0x6810e004, 0x1d1b6018, 0x1f241d12, 0xd1f82c00, 0x481de002, 0x60304e1a, 0x69804813,
    0x0fc007c0, 0xd1f62800, 0x69804810, 0x0236260f, 0x28004030, 0x480dd006, 0x43306980, 0x61b04e0b,
    0xbd702001, 0x68404809, 0x02b62601, 0x4e0743b0, 0x46306070, 0x26086840, 0x4e0443b0, 0x1c6d6070,
    0x42a80988, 0x2000d8c0, 0x0000e7eb, 0x40022000, 0x89abcdef, 0x02030405, 0x8c9daebf, 0x13141516,
    0x00005555, 0x40003000, 0x00000fff, 0x0000aaaa, 0x00000000
};

// Start address of flash
static const uint32_t flash_start = 0x08000000;
// Size of flash
static const uint32_t flash_size = 0x00030000;

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {0x08000000, 0x00000080},
};

static const program_target_t flash = {
    0x20000021, // Init
    0x20000063, // UnInit
    0x00000000, // EraseChip - NOT SUPPORTED
    0x20000081, // EraseSector
    0x200000d9, // ProgramPage
    0x0,        // Verify
    
    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000001,
        0x20000190,
        0x20000800
    },

    0x20000000 + 0x00000A00,  // mem buffer location
    0x20000000,               // location to write prog_blob in target RAM
    sizeof(stm32l0xx_192_flash_prog_blob),   // prog_blob size
    stm32l0xx_192_flash_prog_blob,           // address of prog_blob
    0x00000080       // ram_to_flash_bytes_to_be_written
};
