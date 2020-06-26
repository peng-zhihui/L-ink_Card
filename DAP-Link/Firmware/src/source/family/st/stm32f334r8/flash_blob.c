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

static const uint32_t STM32F334R8_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x49364837, 0x49376041, 0x21006041, 0x68c16001, 0x43112214, 0x69c060c1, 0xd40605c0, 0x49324833,
    0x21066001, 0x49326041, 0x20006081, 0x482c4770, 0x22806901, 0x61014311, 0x47702000, 0x4828b510,
    0x24046901, 0x61014321, 0x22406901, 0x61014311, 0x4a264928, 0x6011e000, 0x07db68c3, 0x6901d1fb,
    0x610143a1, 0xbd102000, 0x491db510, 0x2402690a, 0x610a4322, 0x69086148, 0x43102240, 0x481d6108,
    0xe0004a1a, 0x68cb6010, 0xd1fb07db, 0x43a06908, 0x20006108, 0xb570bd10, 0x08491c49, 0x26140049,
    0x23014d0f, 0x692ce016, 0x612c431c, 0x80048814, 0x07e468ec, 0x692cd1fc, 0x00640864, 0x68ec612c,
    0xd0044234, 0x433068e8, 0x200160e8, 0x1c80bd70, 0x1e891c92, 0xd1e62900, 0xbd702000, 0x45670123,
    0x40022000, 0xcdef89ab, 0x00005555, 0x40003000, 0x00000fff, 0x0000aaaa, 0x00000000
};

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {0x08000000, 0x800},
};

static const program_target_t flash = {
    0x20000021, // Init
    0x2000004f, // UnInit
    0x2000005d, // EraseChip
    0x20000089, // EraseSector
    0x200000b7, // ProgramPage
    0x0,        // Verify

    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000001,
        0x20000118,
        0x20000800
    },

    0x20000000 + 0x00000A00,  // mem buffer location
    0x20000000,               // location to write prog_blob in target RAM
    sizeof(STM32F334R8_flash_prog_blob),   // prog_blob size
    STM32F334R8_flash_prog_blob,           // address of prog_blob
    0x00000400       // ram_to_flash_bytes_to_be_written
};
