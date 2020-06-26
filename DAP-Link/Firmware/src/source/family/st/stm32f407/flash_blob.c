/**
 * @file    flash_blob.c
 * @brief   Flash algorithm for the stm32f407
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


static const uint32_t STM32F407_FLM[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,

    /*0x020*/ 0xe000300, 0xd3022820L, 0x1d000940, 0x28104770, 0x900d302, 0x47701cc0, 0x47700880, 0x49414842,
    /*0x040*/ 0x49426041, 0x21006041, 0x68c16001, 0x431122f0, 0x694060c1, 0xd4060680L, 0x493d483e, 0x21066001,
    /*0x060*/ 0x493d6041, 0x20006081, 0x48374770, 0x5426901, 0x61014311, 0x47702000, 0x4833b510, 0x24046901,
    /*0x080*/ 0x61014321, 0x3a26901, 0x61014311, 0x4a314933, 0x6011e000, 0x3db68c3, 0x6901d4fb, 0x610143a1,
    /*0x0A0*/ 0xbd102000L, 0xf7ffb530L, 0x4927ffbb, 0x23f068ca, 0x60ca431a, 0x610c2402, 0x700690a, 0x43020e40,
    /*0x0C0*/ 0x6908610a, 0x431003e2, 0x48246108, 0xe0004a21L, 0x68cd6010, 0xd4fb03edL, 0x43a06908, 0x68c86108,
    /*0x0E0*/ 0xf000600, 0x68c8d003, 0x60c84318, 0xbd302001L, 0x4d15b570, 0x8891cc9, 0x8968eb, 0x433326f0,
    /*0x100*/ 0x230060eb, 0x4b16612b, 0x692ce017, 0x612c431c, 0x60046814, 0x3e468ec, 0x692cd4fc, 0x640864,
    /*0x120*/ 0x68ec612c, 0xf240624, 0x68e8d004, 0x60e84330, 0xbd702001L, 0x1d121d00, 0x29001f09, 0x2000d1e5,
    /*0x140*/ 0xbd70, 0x45670123, 0x40023c00, 0xcdef89abL, 0x5555, 0x40003000, 0xfff, 0xaaaa,
    /*0x160*/ 0x201, 0x0,
};

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {0x08000000, 1024},
};

static const program_target_t flash = {
    0x2000003D, // Init
    0x2000006B, // UnInit
    0x20000079, // EraseChip
    0x200000A5, // EraseSector
    0x200000F1, // ProgramPage
    0x0,        // Verify
    
// RSB : base adreess is address of Execution Region PrgData in map file
//       to access global/static data
// RSP : Initial stack pointer

    {0x20000001, 0x20000020 + 0x00000144, 0x20002000}, // {breakpoint, RSB, RSP}

    0x20001000, // program_buffer
    0x20000000, // algo_start
    0x00000170, // algo_size
    STM32F407_FLM,// image
    512        // ram_to_flash_bytes_to_be_written
};
