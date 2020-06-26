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

static const uint32_t stm32l1xx_128_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0x4603b510, 0x6980486e, 0x0224240f, 0x4c6c4320, 0x486c61a0, 0x486c60e0, 0x486c60e0, 0x486c6120,
    0x46206120, 0x240169c0, 0x40200524, 0xd1062800, 0x4c694868, 0x20066020, 0x48686060, 0x200060a0,
    0x4601bd10, 0x6840485e, 0x43102202, 0x60504a5c, 0x68404610, 0x43102201, 0x60504a59, 0x47702000,
    0x48574601, 0x22016840, 0x43100252, 0x60504a54, 0x68404610, 0x43102208, 0x60504a51, 0x60082000,
    0x4857e002, 0x60104a54, 0x6980484d, 0x0fc007c0, 0xd1f62800, 0x6840484a, 0x02522201, 0x4a484390,
    0x46106050, 0x22086840, 0x4a454390, 0x20006050, 0x46034770, 0x47702001, 0x4603b530, 0x1dc82480,
    0x0a0130f8, 0x483e0209, 0x00e56840, 0x4d3c4328, 0x46286068, 0x25086840, 0x4d394328, 0xbf006068,
    0x6810e005, 0x1d1b6018, 0x1f091d12, 0x29001f24, 0x2c00d001, 0xe002d1f5, 0x4d374839, 0x48306028,
    0x07c06980, 0x28000fc0, 0x482dd1f6, 0x250f6980, 0x4028022d, 0xd0062800, 0x69804829, 0x4d284328,
    0x200161a8, 0x4826bd30, 0x25016840, 0x43a802ad, 0x60684d23, 0x68404628, 0x43a82508, 0x60684d20,
    0x68404628, 0x4328152d, 0x60684d1d, 0x68404628, 0x43282508, 0x60684d1a, 0xe0052480, 0x60186810,
    0x1d121d1b, 0x1f241f09, 0xd0012900, 0xd1f52c00, 0x481be002, 0x60284d18, 0x69804811, 0x0fc007c0,
    0xd1f62800, 0x6980480e, 0x022d250f, 0x28004028, 0x480bd006, 0x43286980, 0x61a84d09, 0xe7c12001,
    0x68404807, 0x02ad2501, 0x4d0543a8, 0x46286068, 0x25086840, 0x4d0243a8, 0x20006068, 0x0000e7b2,
    0x40023c00, 0x89abcdef, 0x02030405, 0x8c9daebf, 0x13141516, 0x00005555, 0x40003000, 0x00000fff,
    0x0000aaaa, 0x00000000
};

// Start address of flash
static const uint32_t flash_start = 0x08000000;
// Size of flash
static const uint32_t flash_size = 0x00020000;

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {0x08000000, 0x00000100},
};

static const program_target_t flash = {
    0x20000021, // Init
    0x20000063, // UnInit
    0xffffffff, // EraseChip - NOT SUPPORTED
    0x20000081, // EraseSector
    0x200000d9, // ProgramPage
    0x0,        // Verify
    
    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000001,
        0x20000204,
        0x20000800
    },

    0x20000000 + 0x00000A00,  // mem buffer location
    0x20000000,               // location to write prog_blob in target RAM
    sizeof(stm32l1xx_128_flash_prog_blob),   // prog_blob size
    stm32l1xx_128_flash_prog_blob,           // address of prog_blob
    0x00000100       // ram_to_flash_bytes_to_be_written
};
