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

static const uint32_t lpc54608_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x21002210, 0xf8c00690, 0x4a2c2630, 0xf0236813, 0x60134380, 0x1280f8c0, 0x1284f8c0, 0x68134a28,
    0x4370f423, 0xf8c06013, 0x21021380, 0x20006001, 0x20004770, 0xb5104770, 0x2000210f, 0xf844f000,
    0xbf182800, 0x4a1fbd10, 0xe8bd210f, 0x20004010, 0xb864f000, 0x0bc4b510, 0x46084621, 0xf834f000,
    0xbf182800, 0x4a17bd10, 0xe8bd4621, 0x46084010, 0xb854f000, 0x4614b570, 0xd10e0005, 0x0100e9d4,
    0xe9d44408, 0x44111202, 0x69214408, 0x69614408, 0x69a14408, 0x42404408, 0x0be861e0, 0xf0004601,
    0x2800f813, 0xbd70bf18, 0x46214b06, 0xe8bd4628, 0xf44f4070, 0xf0007280, 0x0000b818, 0x40000500,
    0x40000400, 0x00b71b00, 0xb08bb500, 0x92002232, 0x0101e9cd, 0x46684a39, 0x4790a906, 0x28009806,
    0xf600bf18, 0xb00b10c4, 0xb500bd00, 0xf04fb08b, 0x92030c33, 0xc000f8cd, 0x0101e9cd, 0x707af44f,
    0xf0f0fbb3, 0x4a2d9004, 0xa9064668, 0x98064790, 0xbf182800, 0x10c4f600, 0xbd00b00b, 0xb08bb500,
    0x93002334, 0x0101e9cd, 0x707af44f, 0xf0f0fbb2, 0x4a229003, 0xa9064668, 0x98064790, 0xbf182800,
    0x10c4f600, 0xbd00b00b, 0xb08bb500, 0x9300233b, 0x0101e9cd, 0x707af44f, 0xf0f0fbb2, 0x4a179003,
    0xa9064668, 0x98064790, 0xbf182800, 0x10c4f600, 0xbd00b00b, 0xb08bb500, 0x92002235, 0x0101e9cd,
    0x46684a0e, 0x4790a906, 0x28009806, 0xf600bf18, 0xb00b10c4, 0xb500bd00, 0x2338b08b, 0x93009203,
    0x0101e9cd, 0x46684a05, 0x4790a906, 0x28009806, 0xf600bf18, 0xb00b10c4, 0x0000bd00, 0x03000205,
    0x00000000
};

// Start address of flash
static const uint32_t flash_start = 0x00000000;
// Size of flash
static const uint32_t flash_size = 0x00080000;

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {0x00000000, 0x00008000},
};

static const program_target_t flash = {
    0x20000021, // Init
    0x20000053, // UnInit
    0x20000057, // EraseChip
    0x20000075, // EraseSector
    0x20000095, // ProgramPage
    0x0,        // Verify
    
    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000001,
        0x200001e0,
        0x20000400
    },

    0x20000000 + 0x00000A00,  // mem buffer location
    0x20000000,               // location to write prog_blob in target RAM
    sizeof(lpc54608_flash_prog_blob),   // prog_blob size
    lpc54608_flash_prog_blob,           // address of prog_blob
    0x00000100       // ram_to_flash_bytes_to_be_written
};
