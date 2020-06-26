/**
 * @file    flash_blob.c
 * @brief   Flash algorithm for the k20dx
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

static const uint32_t K20D50M_FLM[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0xb510482c, 0x5120f24c, 0xf64d81c1, 0x81c11128, 0xf0218801, 0x80010101, 0x44484827, 0xf884f000,
    0xd0002800, 0xbd102001, 0x47702000, 0xb5104822, 0xf0004448, 0xb920f86f, 0x2100481f, 0xf0004448,
    0x491ef903, 0xf442684a, 0x604a0270, 0xb510bd10, 0x46014604, 0x22014818, 0xf0004448, 0xb930f831,
    0x23004815, 0x46212201, 0xf0004448, 0x4913f8b7, 0xf442684a, 0x604a0270, 0xb5febd10, 0x460b460c,
    0x46014606, 0x4615480c, 0xf0004448, 0xb958f87f, 0x21012000, 0x1000e9cd, 0x48079002, 0x4622462b,
    0x44484631, 0xf8dcf000, 0x684a4904, 0x0270f442, 0xbdfe604a, 0x40052000, 0x00000004, 0x4001f000,
    0x41f0e92d, 0x460d4614, 0xf846f000, 0xd11b2800, 0x1e64442c, 0xd00505a1, 0xeb012101, 0x1e422194,
    0x2481eb02, 0x447e4e09, 0x8024f8df, 0x42a52709, 0x6830d80a, 0xf8886005, 0xf0007007, 0x2800f8e9,
    0xf505d102, 0xe7f26580, 0x81f0e8bd, 0x00000236, 0x40020000, 0x4903b120, 0x71c82044, 0xb8d8f000,
    0x47702004, 0x40020000, 0x4916b190, 0x4a166cc9, 0x6103f3c1, 0xf832447a, 0x03091011, 0x2200d00a,
    0x2100e9c0, 0x60812101, 0x60c10289, 0x47704610, 0x47702004, 0x47702064, 0xea41b128, 0x079b0302,
    0x2065d003, 0x20044770, 0x68034770, 0xd804428b, 0x44116840, 0x42884418, 0x2066d201, 0x20004770,
    0x00004770, 0x40048000, 0x000001c8, 0x47f0e92d, 0x0014461d, 0xd01a460e, 0xf7ff461a, 0x2800ffdd,
    0x4f0cd116, 0xf8df447f, 0xf04fa030, 0x2d000806, 0x6838d00e, 0x68396006, 0x6048cc01, 0x8007f88a,
    0xf886f000, 0xd1032800, 0x1f2d1d36, 0x2004e7ef, 0x87f0e8bd, 0x00000178, 0x40020000, 0x47f0e92d,
    0x4614469a, 0x4605460e, 0xffb6f7ff, 0xd1252800, 0x0101e9d5, 0xf8f1fbb0, 0xf1c84271, 0x40010000,
    0x42b5424d, 0x4445d100, 0x1bafb1bc, 0xd90042a7, 0x480b4627, 0x447808b9, 0x60066800, 0x22014809,
    0x0a0a71c2, 0x728172c2, 0xa009f880, 0xf850f000, 0xd1032800, 0x443e1be4, 0x2000e7e5, 0x87f0e8bd,
    0x00000106, 0x40020000, 0x4804b128, 0x71c22240, 0xf0007181, 0x2004b83d, 0x00004770, 0x40020000,
    0x4df0e92d, 0xe9dd001c, 0x46168709, 0xd025460d, 0xff72f7ff, 0xd11f2800, 0xb04cf8df, 0xf8df44fb,
    0x2e00a04c, 0xf8dbd018, 0x600d1000, 0xf88a2202, 0x9a082007, 0x200bf88a, 0x0000f8db, 0x60816821,
    0xf816f000, 0xf1b8b160, 0xd0010f00, 0x5000f8c8, 0xd0012f00, 0x60392100, 0x8df0e8bd, 0xe7fb2004,
    0x1d241f36, 0xe7dc1d2d, 0x000000a0, 0x40020000, 0x2170480a, 0x21807001, 0x78017001, 0xd5fc0609,
    0x06817800, 0x2067d501, 0x06c14770, 0x2068d501, 0x07c04770, 0x2069d0fc, 0x00004770, 0x40020000,
    0x00000000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000, 0x01000000, 0x01000000,
    0x40020004, 0x00000000,
};

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {0, 1024},
 };

static const program_target_t flash = {
    0x20000021, // Init
    0x20000049, // UnInit
    0x2000004D, // EraseChip
    0x2000006F, // EraseSector
    0x2000009B, // ProgramPage
    0x0,        // Verify
    
    // breakpoint = RAM start + 1
    // RSB : base address is address of Execution Region PrgData in map file
    //       to access global/static data
    // RSP : Initial stack pointer
    {
        0x20000001, // breakpoint instruction address
        0x20000000 + 0x20 + 0x324,  // static base register value (image start + header + static base offset)
        0x20000c00  // initial stack pointer
    },

    0x20001c00, // program_buffer, any valid RAM location with +512 bytes of headroom
    0x20000000, // algo_start, start of RAM
    sizeof(K20D50M_FLM), // algo_size, size of array above
    K20D50M_FLM,  // image, flash algo instruction array
    512        // ram_to_flash_bytes_to_be_written
};
