/**
 * @file    flash_blob.c
 * @brief   Flash algorithm for the lpc1768
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

static const uint32_t LPC1768_FLM[] = {
    0xe00abe00, 0x062d780d, 0x24084068, 0xd3000040, 0x1e644058, 0x1c49d1fa, 0x2a001e52, 0x4770d1f2,

    /*0x20*/ 0x28100b00, 0x210ed302, 0xd0eb01, 0x494f4770, 0x607af44f, 0x60084449, 0x2100484d, 0x21aa7001,
    /*0x40*/ 0x21557301, 0x21017301, 0x1c40f800, 0x47702000, 0x47702000, 0x41f0e92d, 0x20324c46, 0x2500444c,
    /*0x60*/ 0xe884261dL, 0xf1040061L, 0x4f430114, 0x46204688, 0x696047b8, 0x2034b960, 0x61e884, 0x4641483b,
    /*0x80*/ 0x68004448, 0x462060e0, 0x696047b8, 0xd0002800L, 0xe8bd2001L, 0xe92d81f0L, 0xf7ff41f0L, 0x4d35ffc1,
    /*0xa0*/ 0x444d4604, 0xe9c52032L, 0xf1050400L, 0x4e320114, 0x4628460f, 0x47b060ac, 0xb9686968L, 0xe9c52034L,
    /*0xc0*/ 0x482a0400, 0x444860ac, 0x68004639, 0x462860e8, 0x696847b0, 0xd0dc2800L, 0xe7da2001L, 0x41f0e92d,
    /*0xe0*/ 0x64614, 0x4825d11d, 0x12fcf8d4, 0xd03a4281L, 0x42814823, 0x4823d037, 0xd0344281L, 0x4030ea4f,
    /*0x100*/ 0xd0304281L, 0x100e9d4, 0xe9d44408L, 0x44111202, 0x69214408, 0x69614408, 0x69a14408, 0x42404408,
    /*0x120*/ 0x463061e0, 0xff7cf7ffL, 0x21324d12, 0x4f12444d, 0x1000e9c5, 0x114f105, 0x468860a8, 0x47b84628,
    /*0x140*/ 0xb9806968L, 0xe9c52033L, 0xf44f0600L, 0xe9c57000L, 0x48064002, 0x44484641, 0x61286800, 0x47b84628,
    /*0x160*/ 0x28006968, 0x2001d095, 0xe793, 0x4, 0x400fc080, 0x8, 0x1fff1ff1, 0x4e697370,
    /*0x180*/ 0x12345678, 0x87654321L, 0x0, 0x0
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
    0x1000002f, // init
    0x10000051, // uninit
    0x10000055, // erase_chip
    0x10000097, // erase_sector
    0x100000dd, // program_page
    0x0,        // Verify
    
    {0x10000001, 0x10000214, 0x10001000}, // {breakpoint, RSB, RSP}

    0x1000023c, // program_buffer
    0x10000000, // algo_start
    0x00000190, // algo_size
    LPC1768_FLM, // image

    512          // ram_to_flash_bytes_to_be_written
};
