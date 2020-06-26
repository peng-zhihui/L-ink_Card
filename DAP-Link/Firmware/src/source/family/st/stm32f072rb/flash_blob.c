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

static const uint32_t STM32F072RB_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x49454846, 0x49466041, 0x21006041, 0x68c16001, 0x43112214, 0x69c060c1, 0xd4060740, 0x49414842,
    0x21066001, 0x49416041, 0x20006081, 0x483b4770, 0x22806901, 0x61014311, 0x47702000, 0x4837b530,
    0x241468c1, 0x60c14321, 0x25046901, 0x61014329, 0x22406901, 0x61014311, 0x4a334935, 0x6011e000,
    0x07db68c3, 0x6901d1fb, 0x610143a9, 0x422168c1, 0x68c1d004, 0x60c14321, 0xbd302001, 0xbd302000,
    0x4926b530, 0x231468ca, 0x60ca431a, 0x2402690a, 0x610a4322, 0x69086148, 0x43102240, 0x48246108,
    0xe0004a21, 0x68cd6010, 0xd1fb07ed, 0x43a06908, 0x68c86108, 0xd0034018, 0x431868c8, 0x200160c8,
    0xb5f0bd30, 0x1c494d15, 0x68eb0849, 0x24040049, 0x60eb4323, 0x4c162714, 0x692be01a, 0x43332601,
    0x8813612b, 0x4b108003, 0x601ce000, 0x07f668ee, 0x692bd1fb, 0x005b085b, 0x68eb612b, 0xd004423b,
    0x433868e8, 0x200160e8, 0x1c80bdf0, 0x1e891c92, 0xd1e22900, 0xbdf02000, 0x45670123, 0x40022000,
    0xcdef89ab, 0x00005555, 0x40003000, 0x00000fff, 0x0000aaaa, 0x00000000
};

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {0x08000000, 0x400},
};

static const program_target_t flash = {
    0x20000021, // Init
    0x2000004f, // UnInit
    0x2000005d, // EraseChip
    0x200000a1, // EraseSector
    0x200000e3, // ProgramPage
    0x0,        // Verify

    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000001,
        0x20000154,
        0x20000800
    },

    0x20000000 + 0x00000A00,  // mem buffer location
    0x20000000,               // location to write prog_blob in target RAM
    sizeof(STM32F072RB_flash_prog_blob),   // prog_blob size
    STM32F072RB_flash_prog_blob,           // address of prog_blob
    0x00000400       // ram_to_flash_bytes_to_be_written
};
