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

static const uint32_t stm32l1xx_256_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0xbf182a01, 0xd11e2a02, 0x680148a5, 0x6170f441, 0x48a56001, 0x600149a3, 0x600149a4, 0x0004f100,
    0x600149a3, 0x600149a3, 0x680048a3, 0x1f80f410, 0x48a2d109, 0x5155f245, 0xf04f6001, 0x60410106,
    0x71fff640, 0xf04f6081, 0x47700000, 0xbf182801, 0xd1082802, 0x6801489a, 0x0102f041, 0x68016001,
    0x0101f041, 0xf04f6001, 0x47700000, 0xc250f8df, 0x1000f8dc, 0x0f01f011, 0x498bd004, 0x600a4a89,
    0x600a4a8a, 0x1000f8dc, 0x0f02f011, 0x498dd004, 0x600a4a87, 0x600a4a87, 0x1000f8dc, 0x7100f441,
    0x1000f8cc, 0x1000f8dc, 0x0108f041, 0x1000f8cc, 0x60012100, 0x6801487a, 0x0f01f011, 0xf64abf1c,
    0x4a7e21aa, 0x6011d004, 0xf0136803, 0xd1fa0f01, 0xf4116801, 0xd0056f70, 0xf4416801, 0x60016170,
    0x47702001, 0x0000f8dc, 0x7000f420, 0x0000f8cc, 0x0000f8dc, 0x0008f020, 0x0000f8cc, 0x47702000,
    0x4df0e92d, 0x4f674c68, 0x81a0f8df, 0xa1a0f8df, 0xb1a0f8df, 0x4a624e69, 0xf04f4b69, 0x1d256100,
    0x0e00f04f, 0x2caaf64a, 0xf0106818, 0xbf1c0f01, 0xf8c46027, 0x68188000, 0x0f02f010, 0xf8c5bf1c,
    0xf8c5a000, 0x6818b000, 0x7000f440, 0x68186018, 0x0008f040, 0xf8c16018, 0x6810e000, 0x0f01f010,
    0xf8c6d005, 0x6810c000, 0x0f01f010, 0x6810d1f9, 0x6f70f410, 0x6810d005, 0x6070f440, 0x20016010,
    0x6818e008, 0x7000f420, 0x68186018, 0x0008f020, 0x20006018, 0xbf182800, 0x8df0e8bd, 0xf501484a,
    0x42817180, 0x2000d3c0, 0x8df0e8bd, 0x47702001, 0x4e43b4f0, 0x31ff2380, 0xc000f8d6, 0x01fff021,
    0x6c80f44c, 0xc000f8c6, 0xc000f8d6, 0x0c08f04c, 0xc000f8c6, 0xf852e005, 0xf840cb04, 0x1f09cb04,
    0x29001f1b, 0x2b00bf18, 0xf8dfd1f5, 0xf8dcc0b4, 0x4d323000, 0x0f01f013, 0x24aaf64a, 0x602cd005,
    0x3000f8dc, 0x0f01f013, 0xf8dcd1f9, 0xf4133000, 0xd12e6f70, 0xf4236833, 0x60336380, 0xf0236833,
    0x60330308, 0xf4436833, 0x60336380, 0xf0436833, 0x60330308, 0x0380f04f, 0xf852e007, 0xf8407b04,
    0xf1a17b04, 0xf1a30104, 0x29000304, 0x2b00bf18, 0xf8dcd1f3, 0xf0100000, 0xd0050f01, 0xf8dc602c,
    0xf0100000, 0xd1f90f01, 0x0000f8dc, 0x6f70f410, 0xf8dcd009, 0xf4400000, 0xf8cc6070, 0xbcf00000,
    0x0001f04f, 0x68304770, 0x6080f420, 0x68306030, 0x0008f020, 0xbcf06030, 0x0000f04f, 0x00004770,
    0x40023c18, 0x89abcdef, 0x40023c0c, 0x02030405, 0x8c9daebf, 0x13141516, 0x40023c1c, 0x40003000,
    0x40023c04, 0x40023c10, 0x08040000, 0x00000000
};

/**
* List of start and size for each size of flash sector
* The size will apply to all sectors between the listed address and the next address
* in the list.
* The last pair in the list will have sectors starting at that address and ending
* at address start + size.
*/
static const sector_info_t sectors_info[] = {
    {0x08000000, 256},
};

static const program_target_t flash = {
    0x20000021, // Init
    0x2000006d, // UnInit
    0x20000121, // EraseChip
    0x2000008d, // EraseSector
    0x200001d1, // ProgramPage
    0x0,        // Verify
    
    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000001,
        0x200002ec,
        0x20000800
    },

    0x20000000 + 0x00000A00,  // mem buffer location
    0x20000000,               // location to write prog_blob in target RAM
    sizeof(stm32l1xx_256_flash_prog_blob),   // prog_blob size
    stm32l1xx_256_flash_prog_blob,           // address of prog_blob
    0x00000100       // ram_to_flash_bytes_to_be_written
};
