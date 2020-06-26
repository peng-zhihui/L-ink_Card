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

static const uint32_t STM32F746ZG_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x8f4ff3bf, 0x03004770, 0x28400e00, 0x0980d302, 0x47701d00, 0x477008c0, 0x49474848, 0x49486041,
    0x21006041, 0x68c16001, 0x431122f0, 0x694060c1, 0xd4060680, 0x49434844, 0x21066001, 0x49436041,
    0x20006081, 0x483d4770, 0x05426901, 0x61014311, 0x47702000, 0x4839b510, 0x24046901, 0x61014321,
    0x03a26901, 0x61014311, 0x4a374939, 0x6011e000, 0x03db68c3, 0x6901d4fb, 0x610143a1, 0xbd102000,
    0xf7ffb530, 0x492dffc0, 0x23f068ca, 0x60ca431a, 0x610c2402, 0x0700690a, 0x43020e40, 0x6908610a,
    0x431003e2, 0x482a6108, 0xe0004a27, 0x68cd6010, 0xd4fb03ed, 0x43a06908, 0x68c86108, 0x0f000600,
    0x68c8d003, 0x60c84318, 0xbd302001, 0x4c1bb5f0, 0x08891cc9, 0x008968e5, 0x431d23f0, 0x230060e5,
    0x27016123, 0x4d1a06ff, 0x6923e021, 0x43334e19, 0x03066123, 0x19f60b36, 0x60336813, 0x8f4ff3bf,
    0xe0004e11, 0x68e36035, 0xd4fb03db, 0x085b6923, 0x6123005b, 0x061b68e3, 0xd0050f1b, 0x21f068e0,
    0x60e04308, 0xbdf02001, 0x1f091d00, 0x29001d12, 0x2000d1db, 0x0000bdf0, 0x45670123, 0x40023c00,
    0xcdef89ab, 0x00005555, 0x40003000, 0x00000fff, 0x0000aaaa, 0x00000201, 0x00000000
};

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {0x08000000, 0x8000},
};

static const program_target_t flash = {
    0x20000039, // Init
    0x20000067, // UnInit
    0x20000075, // EraseChip
    0x200000a1, // EraseSector
    0x200000ed, // ProgramPage
    0x0,        // Verify

    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000001,
        0x20000178,
        0x20000800
    },

    0x20000000 + 0x00000A00,  // mem buffer location
    0x20000000,               // location to write prog_blob in target RAM
    sizeof(STM32F746ZG_flash_prog_blob),   // prog_blob size
    STM32F746ZG_flash_prog_blob,           // address of prog_blob
    0x00000400       // ram_to_flash_bytes_to_be_written
};
