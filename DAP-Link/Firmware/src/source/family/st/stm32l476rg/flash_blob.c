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

static const uint32_t STM32L476RG_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x6a094953, 0xd5040289, 0x42884952, 0x2001d301, 0x20004770, 0x484e4770, 0x6081494f, 0x6081494f,
    0x60012100, 0x31faf244, 0x6a006101, 0xd40803c0, 0xf245484b, 0x60015155, 0x60412106, 0x71fff640,
    0x20006081, 0x48424770, 0xf0416941, 0x61414100, 0x47702000, 0x47702001, 0xf64ab510, 0x494022aa,
    0xe000483b, 0x6903600a, 0xd4fb03db, 0x34faf244, 0xf2486104, 0x61430304, 0xf4436943, 0x61433380,
    0x600ae000, 0x03db6903, 0x2100d4fb, 0x69016141, 0xd0024221, 0x20016104, 0x2000bd10, 0xb570bd10,
    0xf7ff4602, 0xf3c2ffad, 0xf64a25c7, 0x4a2c23aa, 0xe0004927, 0x690c6013, 0xd4fb03e4, 0x34faf244,
    0x2602610c, 0x05c5eb06, 0x20c0ea45, 0x69486148, 0x3080f440, 0xe0006148, 0x69086013, 0xd4fb03c0,
    0x61482000, 0x40206908, 0x610cd001, 0xbd702001, 0x1dc9b5f0, 0x0107f021, 0x25aaf64a, 0x4c144e18,
    0x6035e000, 0x03db6923, 0xf244d4fb, 0x612737fa, 0x0e01f04f, 0x0c00f04f, 0xf8c4e015, 0x6813e014,
    0x68536003, 0xe0006043, 0x69236035, 0xd4fb03db, 0xc014f8c4, 0x423b6923, 0x6127d002, 0xbdf02001,
    0x39083008, 0x29003208, 0x2000d1e7, 0x0000bdf0, 0x40022000, 0x08080000, 0x45670123, 0xcdef89ab,
    0x40003000, 0x00000000
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
    0x20000037, // Init
    0x20000067, // UnInit
    0x20000079, // EraseChip
    0x200000bf, // EraseSector
    0x20000111, // ProgramPage
    0x0,        // Verify

    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000001,
        0x20000184,
        0x20000800
    },

    0x20000000 + 0x00000A00,  // mem buffer location
    0x20000000,               // location to write prog_blob in target RAM
    sizeof(STM32L476RG_flash_prog_blob),   // prog_blob size
    STM32L476RG_flash_prog_blob,           // address of prog_blob
    0x00000400       // ram_to_flash_bytes_to_be_written
};
