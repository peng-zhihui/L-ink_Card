#!/usr/bin/env python

#
# DAPLink Interface Firmware
# Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
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

import argparse
import itertools
import os.path
from intelhex import IntelHex


def ranges(i):
    for _, b in itertools.groupby(enumerate(i), lambda (x, y): y - x):
        b = list(b)
        yield b[0][1], b[-1][1]


def print_hex_info(filename, intel_hex):
    print "File: %s" % filename
    addresses = intel_hex.addresses()
    addresses.sort()
    data_list = list(ranges(addresses))
    for start, end in data_list:
        print "    0x%x, 0x%x" % (start, end)


def merge_hex(hex1, hex2):
    hex1_dict = hex1.todict()
    hex2_dict = hex2.todict()

    # The key "start_addr" shows up the hex
    # dictionary so remove it
    if "start_addr" in hex1_dict:
        del hex1_dict["start_addr"]
    if "start_addr" in hex2_dict:
        del hex2_dict["start_addr"]

    keys = hex2_dict.keys()
    keys.sort()

    # Verify nothing unexpected is in the dict
    for key in keys:
        if not type(key) in (int, long):
            print 'Unknown key "%s" of type %s' % (key, type(key))

    for key in keys:
        if key in hex1_dict:
            raise Exception("Overlapping data at 0x%x" % key)
        hex1_dict[key] = hex2_dict[key]
    return IntelHex(hex1_dict)

parser = argparse.ArgumentParser(description='Hex file merger')
parser.add_argument("--hex", type=str, default=[], action='append', help="Hex file to add to list of files to merge.  This can be specified multiple times.")
parser.add_argument("--bin", nargs=2, type=str, default=[], metavar=('FILE', 'ADDR'), action='append', help="Binary file to add to list of files to merge.  This can be specified multiple times.")
parser.add_argument("--output_file", type=str, default='image.hex', help="Name of output hex file.")
parser.add_argument("--output_bin_file", type=str, help="Name of output binary file. May be specified in addition to --output_file.")


def main():
    args = parser.parse_args()
    base_hex = IntelHex()
    # Merge in hex files
    for file_name in args.hex:
        file_name = os.path.expanduser(file_name)
        new_hex_data = IntelHex()
        print "opening file %s" % file_name
        new_hex_data.fromfile(file_name, format='hex')
        print_hex_info(file_name, new_hex_data)
        base_hex = merge_hex(base_hex, new_hex_data)
    # Merge in binary files
    for file_name, addr_str in args.bin:
        file_name = os.path.expanduser(file_name)
        offset = int(addr_str, 0)
        new_hex_data = IntelHex()
        new_hex_data.loadbin(file_name, offset=offset)
        print_hex_info(file_name, new_hex_data)
        base_hex = merge_hex(base_hex, new_hex_data)
    # Write out data
    print_hex_info(os.path.expanduser(args.output_file), base_hex)
    with open(os.path.expanduser(args.output_file), 'wb') as output_file:
        base_hex.tofile(output_file, 'hex')
    if args.output_bin_file is not None:
        with open(os.path.expanduser(args.output_bin_file), 'wb') as output_file:
            base_hex.tofile(output_file, 'bin')

if __name__ == '__main__':
    main()
