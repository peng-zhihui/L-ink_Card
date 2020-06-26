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

static const uint32_t STM32F439ZI_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x03004601, 0x28200e00, 0x0940d302, 0xe0051d00, 0xd3022810, 0x1cc00900, 0x0880e000, 0xd50102c9,
    0x43082110, 0x48464770, 0x60414944, 0x60414945, 0x60012100, 0x22f068c1, 0x60c14311, 0x06806940,
    0x4842d406, 0x60014940, 0x60412106, 0x60814940, 0x47702000, 0x6901483a, 0x43110542, 0x20006101,
    0xb5304770, 0x69014836, 0x43212404, 0x69016101, 0x43290365, 0x69016101, 0x431103a2, 0x49356101,
    0xe0004a32, 0x68c36011, 0xd4fb03db, 0x43a16901, 0x69016101, 0x610143a9, 0xbd302000, 0xf7ffb530,
    0x4927ffaf, 0x23f068ca, 0x60ca431a, 0x610c2402, 0x06c0690a, 0x43020e00, 0x6908610a, 0x431003e2,
    0x48246108, 0xe0004a21, 0x68cd6010, 0xd4fb03ed, 0x43a06908, 0x68c86108, 0x0f000600, 0x68c8d003,
    0x60c84318, 0xbd302001, 0x4d15b570, 0x08891cc9, 0x008968eb, 0x433326f0, 0x230060eb, 0x4b16612b,
    0x692ce017, 0x612c431c, 0x60046814, 0x03e468ec, 0x692cd4fc, 0x00640864, 0x68ec612c, 0x0f240624,
    0x68e8d004, 0x60e84330, 0xbd702001, 0x1d121d00, 0x29001f09, 0x2000d1e5, 0x0000bd70, 0x45670123,
    0x40023c00, 0xcdef89ab, 0x00005555, 0x40003000, 0x00000fff, 0x0000aaaa, 0x00000201, 0x00000000
};

/**
 * @brief List of start and size for each size of flash sector.
 *
 * Even indexes are start, odd are size. The size will apply to all sectors between the listed
 * address and the next address in the list. The last pair in the list will have sectors starting
 * at that address and ending at address flash_start + flash_size.
*/
static const sector_info_t sectors_info[] = {
    { 0x08000000, 0x00004000 }, // 4 x 16KB
    { 0x08010000, 0x00010000 }, // 1 x 64KB
    { 0x08020000, 0x00020000 }, // 7 x 128KB
    { 0x08100000, 0x00004000 },
    { 0x08110000, 0x00010000 },
    { 0x08120000, 0x00020000 },
};

static const program_target_t flash = {
    0x20000047, // Init
    0x20000075, // UnInit
    0x20000083, // EraseChip
    0x200000bd, // EraseSector
    0x20000109, // ProgramPage
    0x0,        // Verify
    
    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000001,
        0x2000017c,
        0x20000800
    },

    0x20000000 + 0x00000A00,  // mem buffer location
    0x20000000,               // location to write prog_blob in target RAM
    sizeof(STM32F439ZI_flash_prog_blob),   // prog_blob size
    STM32F439ZI_flash_prog_blob,           // address of prog_blob
    0x00000400       // ram_to_flash_bytes_to_be_written
};
