/**
 * @file    flash_blob.c
 * @brief   Flash algorithm for the efm32gg
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

static const uint32_t efm32gg_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x4770ba40, 0x4770bac0, 0x4c0cb510, 0xf04068a0, 0x60a00001, 0x301af246, 0xf44f6560, 0x60e07040,
    0x20012100, 0xf83ef000, 0x65612100, 0xf02168a1, 0x60a10101, 0xbf182800, 0xbd102001, 0x400c0000,
    0x4684b510, 0xf44f2300, 0xf8505180, 0x1f092b04, 0x1c52d002, 0xe001d0f9, 0xd0131c50, 0x68a04c0c,
    0x0001f040, 0xf8c460a0, 0x2001c010, 0x200260e0, 0x210060e0, 0xf0002001, 0x4603f815, 0xf02068a0,
    0x60a00001, 0xbf142b00, 0x20002001, 0x0000bd10, 0x400c0000, 0xf6414902, 0x63c83071, 0x47702000,
    0x400c0000, 0x4b14b410, 0xc050f8df, 0x201cf8dc, 0x0f16f012, 0xf8dcd014, 0xf0200008, 0xf8cc0005,
    0xf0120008, 0xbf1e0f02, 0x0001f06f, 0x4770bc10, 0x0f04f012, 0xf04fbf1e, 0xbc1030ff, 0xe0074770,
    0x428a4002, 0xbc10d102, 0x47702000, 0xd1dd1e5b, 0xf06fbc10, 0x47700002, 0x00989680, 0x400c0000,
    0x4df0e92d, 0x1cc84604, 0x0603f020, 0x46904859, 0xf0416881, 0x60810101, 0xd9732e07, 0x0f07f014,
    0xf8d8d011, 0x61041000, 0x60c22201, 0x21086181, 0x210060c1, 0xf7ff4610, 0x2800ffb5, 0x1d24d17e,
    0x0810f108, 0x484b1f36, 0xf0416881, 0x60810104, 0xbf882e07, 0x0a02f06f, 0xf504d966, 0x46015080,
    0x010bf36f, 0x42b11b09, 0x4635bf8e, 0x000bf36f, 0xf0151b05, 0xbf180f04, 0x46471f2d, 0x20012100,
    0xff90f7ff, 0xd13b2800, 0xe0e8f8df, 0x4010f8ce, 0xf8ce2001, 0xf857000c, 0xf8ce0b04, 0xf8570018,
    0xf8ce0b04, 0x20080018, 0x000cf8ce, 0x0208f1a5, 0xd9332a07, 0x0b08f04f, 0x46dc482f, 0x101cf8de,
    0x0f16f011, 0xf8ded012, 0xf0200008, 0xf8ce0005, 0xf0110008, 0xbf180f02, 0x0001f06f, 0xf011d10f,
    0xbf180f04, 0x30fff04f, 0xe007d109, 0x010bea01, 0xd1014561, 0xe0022000, 0xd1df1e40, 0xb1104650,
    0xe00bb9e0, 0xf857e01c, 0xf8ce0b04, 0xf8570018, 0xf8ce0b04, 0x3a080018, 0xd8cb2a07, 0xeb081b76,
    0x442c0885, 0xd8982e07, 0x20012100, 0xff3af7ff, 0x4810b920, 0xf0216881, 0xe0000104, 0x6081e00f,
    0x480cb186, 0x1000f8d8, 0x22016104, 0x618160c2, 0x60c12108, 0x46102100, 0xff24f7ff, 0x2001b110,
    0x8df0e8bd, 0x68814803, 0x0101f021, 0x20006081, 0x8df0e8bd, 0x400c0000, 0x00989680, 0x68814803,
    0x0105f021, 0x20006081, 0x00004770, 0x400c0000, 0x00000000,
};

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {0, 4096},
};

static const program_target_t flash = {
    0x200000B5, // Init
    0x2000029D, // UnInit
    0x20000029, // EraseChip
    0x20000061, // EraseSector
    0x20000121, // ProgramPage
    0x0,        // Verify
    
    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000000 + 0x00000001,
        0x20000000 + 0x00000020 + 0x00000290,
        0x20000800
    },

    0x20000000 + 0x00000A00,   // mem buffer location
    0x20000000,                // location to write prog_blob in target RAM
    sizeof(efm32gg_flash_prog_blob), // prog_blob size
    efm32gg_flash_prog_blob,         // address of prog_blob
    0x00000200                 // ram_to_flash_bytes_to_be_written
};
