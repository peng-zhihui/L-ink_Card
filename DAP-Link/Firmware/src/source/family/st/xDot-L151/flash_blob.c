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

static const uint32_t stm32l151_flash_prog_blob[] = {
    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    0xbf182a01, 0xd1042a02, 0x6801488e, 0x6170f441, 0x20006001, 0x28014770, 0x2802bf18, 0x488ad108, 
    0xf0416801, 0x60010102, 0xf0416801, 0x60010101, 0x47702000, 0x68114a84, 0x0f01f011, 0x4984d004, 
    0x600b4b82, 0x600b4b83, 0xf0116811, 0xd0040f02, 0x4b814982, 0x4b82600b, 0x6811600b, 0x7100f441, 
    0x68116011, 0x0108f041, 0x21006011, 0x49756001, 0xf0106808, 0xd1fb0f01, 0xf4106808, 0xd0056f70, 
    0x680a6808, 0x6270f442, 0x4770600a, 0xf4206810, 0x60107000, 0xf0206810, 0x60100008, 0x47702000, 
    0x4df0e92d, 0xc1a8f8df, 0x4e6a4d68, 0xf8df4f6a, 0x4a6481b0, 0xf8df4b64, 0xf04fe1ac, 0xf10c6100, 
    0xf04f0404, 0x68180a00, 0x0f01f010, 0xf8ccbf1c, 0xf8cc5000, 0x68186000, 0x0f02f010, 0x6027bf1c, 
    0x8000f8c4, 0xf4406818, 0x60187000, 0xf0406818, 0x60180008, 0xa000f8c1, 0xf0106810, 0xd1fb0f01, 
    0xf4106810, 0xd0076f70, 0xf8d26810, 0xf44bb000, 0xf8c26b70, 0xe008b000, 0xf4206818, 0x60187000, 
    0xf0206818, 0x60180008, 0x28002000, 0xe8bdbf18, 0xf5018df0, 0x45717180, 0x2000d3c5, 0x8df0e8bd, 
    0x47702001, 0x4df0e92d, 0x68294d3f, 0x80fcf8df, 0xf8df4e3f, 0xf011a100, 0xbf1c0f01, 0x8000f8c6, 
    0xa000f8c6, 0xf8df6829, 0x4f3cb0f0, 0xe0f0f8df, 0x0f02f011, 0xf8c7bf1c, 0xf8c7b000, 0x6829e000, 
    0x0f01f011, 0x6829bf04, 0x0f02f011, 0x2001d002, 0x8df0e8bd, 0xf4416829, 0x60296180, 0xf0416829, 
    0x60290108, 0x68214c27, 0x0f01f011, 0x2100d1fb, 0xf852460b, 0xf840c023, 0x1d09c001, 0x29801c5b, 
    0x6821dbf7, 0x6f70f411, 0x6829d134, 0x0f01f011, 0xf8c6bf1c, 0xf8c68000, 0x6829a000, 0x0f02f011, 
    0xf8c7bf1c, 0xf8c7b000, 0x6829e000, 0x0f01f011, 0x6829bf04, 0x0f02f011, 0x6829d1c9, 0x6180f441, 
    0x68296029, 0x0108f041, 0x68216029, 0x0f01f011, 0x2180d1fb, 0xf8522320, 0xf840c023, 0x1d09c001, 
    0xf5b11c5b, 0xdbf67f80, 0xf4106820, 0xbf086070, 0x8df0e8bd, 0x68216820, 0x6170f441, 0xe8bd6021, 
    0x00008df0, 0x40023c18, 0x40023c04, 0x89abcdef, 0x40023c0c, 0x02030405, 0x8c9daebf, 0x40023c10, 
    0x13141516, 0x08040000, 0x00000000, 
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
    0x20000037, // UnInit
    0x200000C1, // EraseChip
    0x20000055, // EraseSector
    0x20000165, // ProgramPage
    0x0,        // Verify
    
    // BKPT : start of blob + 1
    // RSB  : blob start + header + rw data offset
    // RSP  : stack pointer
    {
        0x20000000 + 0x00000001,
        0x20000000 + 0x00000020 + 0x00000268,
        0x20000800
    },

    0x20000000 + 0x00000A00,   // mem buffer location
    0x20000000,                // location to write prog_blob in target RAM
    sizeof(stm32l151_flash_prog_blob), // prog_blob size
    stm32l151_flash_prog_blob,         // address of prog_blob
    0x00000200                 // ram_to_flash_bytes_to_be_written
};
