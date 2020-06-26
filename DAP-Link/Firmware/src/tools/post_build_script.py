#
# DAPLink Interface Firmware
# Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

from __future__ import absolute_import
from __future__ import print_function

import argparse
import itertools
import binascii
import struct
import intelhex
import offset_update
from os.path import dirname, join
from flash_algo import PackFlashAlgo

VECTOR_FMT = "<7I"
CHECKSUM_FMT = "<1I"
CHECKSUM_OFFSET = 0x1C
TARGET_INFO_OFFSET = 13*4
ALIGN_PADS = 4

def ranges(i):
    for _, b in itertools.groupby(enumerate(i), lambda x_y: x_y[1] - x_y[0]):
        b = list(b)
        yield b[0][1], b[-1][1]

def post_build_script(input_file, output_file, board_id=None, family_id=None, bin_offset=None, flash_algo_file=None, target_ram_start=None, target_ram_end=None, flash_blob_entry="0x20000000"):
    output_format_file = '-'.join(filter(None, (output_file, board_id, family_id, bin_offset)))
    print(output_format_file)
    output_file_hex = output_format_file + ".hex"
    output_file_binary = output_format_file + ".bin"
    output_file_txt = output_format_file + ".txt"
    output_file_c = output_format_file + ".c"
    output_file_c_generic = join(dirname(output_file), "bootloader_image.c")
    output_file_legacy = output_format_file + "_legacy_0x8000.bin"
    output_file_legacy_5000 = output_format_file + "_legacy_0x5000.bin"
    output_file_legacy_txt = output_format_file + "_legacy.txt"

    # Read in hex file
    new_hex_file = intelhex.IntelHex()
    new_hex_file.padding = 0xFF

    if input_file.lower().endswith('.bin'):
        if bin_offset is not None:
            new_hex_file.loadbin(input_file, offset=int(bin_offset, 16))
        else:
            new_hex_file.loadbin(input_file)
    else:   #always assume hex format
        new_hex_file.fromfile(input_file, format='hex')


    # Get the starting and ending address
    addresses = new_hex_file.addresses()
    addresses.sort()
    start_end_pairs = list(ranges(addresses))
    regions = len(start_end_pairs)
    assert regions == 1, ("Error - only 1 region allowed in "
                          "hex file %i found." % regions)
    start, end = start_end_pairs[0]

    pack_flash_algo = None
    if flash_algo_file is not None:
        with open(flash_algo_file, "rb") as file_handle:
            pack_flash_algo = PackFlashAlgo(file_handle.read())

    # Checksum the vector table
    #
    # Note this is only required for NXP devices but
    # it doesn't hurt to checksum all builds

    # Compute a checksum on the first 7 vector nvic vectors
    vector_size = struct.calcsize(VECTOR_FMT)
    vector_data = new_hex_file.tobinarray(start=start, size=vector_size)
    vectors = struct.unpack(VECTOR_FMT, vector_data)
    assert len(vectors) == 7, "Incorrect size of %i" % len(vectors)
    checksum = 0
    for vector in vectors:
        checksum += vector
    checksum = (~checksum + 1) & 0xFFFFFFFF  # Two's compliment
    # Write checksum back to hex
    csum_start = CHECKSUM_OFFSET + start
    csum_data = struct.pack(CHECKSUM_FMT, checksum)
    assert len(csum_data) == 4
    new_hex_file.puts(csum_start, csum_data)

    print("board_id", board_id)
    print("family_id", family_id)
    print("bin_offset", bin_offset)
    if board_id is not None or family_id is not None:
        target_info_addr = new_hex_file.gets(start + TARGET_INFO_OFFSET, 4)
        target_addr_unpack = struct.unpack("<1I",target_info_addr)[0]
        print("board_info offset: ",hex(target_addr_unpack - start))
        #family_id is in integer hex
        if family_id is not None:
            new_hex_file.puts(target_addr_unpack + 2,struct.pack('<1H',int(family_id, 16)))
        #board_id is in string hex
        if board_id is not None:
            new_hex_file.puts(target_addr_unpack + 4,struct.pack('4s',"%.04X" % int(board_id, 16)))
        if pack_flash_algo is not None:
            blob_header = (0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2)
            stack_size = 0x200
            region_info_fmt = '5I'
            region_info_total = 10
            target_cfg_fmt = '3I'+ region_info_fmt*region_info_total*2 + 'IHBB'
            sector_info_fmt = '2I'
            sector_info_len = len(pack_flash_algo.sector_sizes)
            program_target_fmt = '14I'
            flash_blob_entry = int(flash_blob_entry, 16)
            blob_pad_size = ((len(pack_flash_algo.algo_data) + ALIGN_PADS -1) // ALIGN_PADS * ALIGN_PADS) - len(pack_flash_algo.algo_data)
            blob_header_size = len(blob_header) * 4
            total_struct_size = blob_header_size + len(pack_flash_algo.algo_data) + blob_pad_size + sector_info_len*struct.calcsize(sector_info_fmt) + struct.calcsize(program_target_fmt) + struct.calcsize(target_cfg_fmt)
            flash_blob_addr = end + 1 - 4 -  total_struct_size #make room for crc
            print("flash_blob offset:", hex(flash_blob_addr - start))
            new_hex_file.puts(flash_blob_addr, struct.pack('<'+'I'*len(blob_header), *blob_header))
            new_hex_file.puts(flash_blob_addr + blob_header_size, pack_flash_algo.algo_data + "\x00" * blob_pad_size)
            sector_info_addr = flash_blob_addr+blob_header_size + len(pack_flash_algo.algo_data) + blob_pad_size
            sector_info_arr = []
            for flash_start, flash_size in pack_flash_algo.sector_sizes:
                sector_info_arr.append(flash_start + pack_flash_algo.flash_start)
                sector_info_arr.append(flash_size)
            print("sector_info offset:", hex(sector_info_addr - start))
            new_hex_file.puts(sector_info_addr,struct.pack('<' + 'I'*len(sector_info_arr), *sector_info_arr))
            program_target_addr = sector_info_addr + len(sector_info_arr)*4
            stack_pointer = (flash_blob_entry + blob_header_size + pack_flash_algo.rw_start + pack_flash_algo.rw_size + stack_size + 0x100 - 1) // 0x100 * 0x100
            print("program_target offset:", hex(program_target_addr - start))
            new_hex_file.puts(program_target_addr,struct.pack('<' + program_target_fmt,
                                                            pack_flash_algo.symbols['Init'] + blob_header_size + flash_blob_entry,
                                                            pack_flash_algo.symbols['UnInit'] + blob_header_size + flash_blob_entry,
                                                            pack_flash_algo.symbols['EraseChip'] + blob_header_size + flash_blob_entry if pack_flash_algo.symbols['EraseChip'] != 0xffffffff else 0,
                                                            pack_flash_algo.symbols['EraseSector'] + blob_header_size + flash_blob_entry,
                                                            pack_flash_algo.symbols['ProgramPage'] + blob_header_size + flash_blob_entry,
                                                            pack_flash_algo.symbols['Verify'] + blob_header_size + flash_blob_entry if pack_flash_algo.symbols['Verify'] != 0xffffffff else 0,
                                                            flash_blob_entry + 1, #BKPT : start of blob + 1
                                                            flash_blob_entry + blob_header_size + pack_flash_algo.rw_start, #RSB  : blob start + header + rw data offset
                                                            stack_pointer, #RSP  : stack pointer
                                                            flash_blob_entry + 0x00000A00, #mem buffer location
                                                            flash_blob_entry, #location to write prog_blob in target RAM
                                                            blob_header_size + len(pack_flash_algo.algo_data) + blob_pad_size, #prog_blob size
                                                            flash_blob_addr, #address of prog_blob
                                                            pack_flash_algo.page_size #ram_to_flash_bytes_to_be_written
                                                            ))
            target_cfg_addr = program_target_addr + struct.calcsize(program_target_fmt)
            print("target_cfg offset:", hex(target_cfg_addr - start))
            if target_ram_start is None or target_ram_end is None:
                 raise Exception("target_ram_start and target_ram_end should be defined!")
            first_flash_region = (pack_flash_algo.flash_start, pack_flash_algo.flash_start + pack_flash_algo.flash_size, 1, 0, program_target_addr)
            first_ram_region = (int(target_ram_start, 16), int(target_ram_end, 16), 0, 0, 0)
            emypty_region = (0, 0, 0, 0, 0) * (region_info_total -1)
            all_regions = first_flash_region + emypty_region + first_ram_region + emypty_region
            target_flags = ( 0, 0, 0, 0) #realtime board ID, family ID and erase reset flag
            regions_flags = all_regions + target_flags
            new_hex_file.puts(target_cfg_addr, struct.pack('<' + target_cfg_fmt,
                                                            0x1,                #script generated
                                                            sector_info_addr,   # Sector start and length list
                                                            sector_info_len,    #Sector start and length list total
                                                            *regions_flags
                                                            ))
            board_info_flag = 1 if pack_flash_algo.symbols['EraseSector'] != 0xffffffff else 0  #kEnablePageErase
            new_hex_file.puts(target_addr_unpack + 12,struct.pack('<1I', board_info_flag)) #always enable page erase EraseSector is a required symbol in flash algo
            new_hex_file.puts(target_addr_unpack + 16,struct.pack('<1I', target_cfg_addr))

    # CRC the entire image
    #
    # This is required for all builds

    # Compute checksum over the range (don't include data at location of crc)
    size = end - start + 1
    crc_size = size - 4
    data = new_hex_file.tobinarray(start=start, size=crc_size)
    crc32 = binascii.crc32(data) & 0xFFFFFFFF

    # Write CRC to the file in little endian
    new_hex_file[end - 3] = (crc32 >> 0) & 0xFF
    new_hex_file[end - 2] = (crc32 >> 8) & 0xFF
    new_hex_file[end - 1] = (crc32 >> 16) & 0xFF
    new_hex_file[end - 0] = (crc32 >> 24) & 0xFF

    # Write out file(s)
    new_hex_file.tofile(output_file_hex, 'hex')
    new_hex_file.tofile(output_file_binary, 'bin')
    with open(output_file_txt, 'w') as file_handle:
        file_handle.write("0x%08x\r\n" % crc32)

    # Write out data as a C array
    data = new_hex_file.tobinarray(start=start, size=size)
    data = list(bytearray(data))
    output_data = ('static const unsigned int image_start = 0x%08x;\n'
                    'static const unsigned int image_size = 0x%08x;\n'
                    'static const char image_data[0x%08x] = {\n    ' %
                    (start, size, size))
    for i, byte_val in enumerate(data):
        output_data += '0x%02x' % byte_val + ', '
        if ((i + 1) % 0x20) == 0:
            output_data += '\n    '
    output_data += '};\n'
    with open(output_file_c, 'w') as file_handle:
        file_handle.write(output_data)
    with open(output_file_c_generic, 'w') as file_handle:
        file_handle.write(output_data)

    # Print info on operation
    print("Start 0x%x, Length 0x%x, CRC32 0x%08x" % (start, size, crc32))

    if start == 0x8000 or start == 0x10000 or start == 0x88000 or start == 0x0800C000:
        if start == 0x0800C000:
            # Adjust for ST-Link
            pad_addr = start - 0x8000
        else:
            pad_addr = start - 0x3000
        legacy_zero = start + 7 * 4
        legacy_size = 4 * 4
        legacy_hex_file = intelhex.IntelHex(new_hex_file)
        for addr in range(legacy_zero, legacy_zero + legacy_size):
            legacy_hex_file[addr] = 0
        data = legacy_hex_file.tobinarray(start=start, size=crc_size)
        crc32 = binascii.crc32(data) & 0xFFFFFFFF
        # Write CRC to the file in little endian
        legacy_hex_file[end - 3] = (crc32 >> 0) & 0xFF
        legacy_hex_file[end - 2] = (crc32 >> 8) & 0xFF
        legacy_hex_file[end - 1] = (crc32 >> 16) & 0xFF
        legacy_hex_file[end - 0] = (crc32 >> 24) & 0xFF
        legacy_hex_file.tofile(output_file_legacy, 'bin')
        with open(output_file_legacy_txt, 'w') as file_handle:
            file_handle.write("0x%08x\r\n" % crc32)
        offset_update.create_padded_image(output_file_legacy,
                                          output_file_legacy_5000,
                                          start, pad_addr, 0x40)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Post build tool for Board ID, Family ID, Target flash algo and CRC generation')
    parser.add_argument("input", type=str, help="Hex or bin file to read from.")
    parser.add_argument("output", type=str,
                        help="Output base file name to write crc, board_id and family_id.")
    parser.add_argument("--board-id", type=str,
                        help="board id to for the target in hex")
    parser.add_argument("--family-id", type=str,
                        help="family id to for the target in hex")
    parser.add_argument("--bin-offset", type=str,
                        help="binary offset in hex can be supplied if input is bin")
    parser.add_argument("--flash-algo-file", type=str,
                        help="Elf, axf, or flm to extract flash algo from")
    parser.add_argument("--target-ram-start", type=str,
                        help="Lowest address of target RAM for flash algo in hex")
    parser.add_argument("--target-ram-end", type=str,
                        help="Highest address of target RAM for flash algo in hex")
    parser.add_argument("--flash-blob-entry", type=str, default="0x20000000",
                         help="Entry point of flash algo in the target")
    args = parser.parse_args()
    post_build_script(args.input, args.output, args.board_id, args.family_id, args.bin_offset, args.flash_algo_file, args.target_ram_start, args.target_ram_end, args.flash_blob_entry)
