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

import struct
import argparse
from intelhex import IntelHex

CFG_KEY = 0x6b766c64
# Must stay in sync with the structure cfg_setting
# in the header file config_settings.h:
# 32 - key
# 16 - offset_of_end
# 8  - auto_rst
# 8  - automation_allowed
# 8  - overflow_detect
# 0  - 'end' member omitted
FORMAT = '<LHBBB'
FORMAT_LENGTH = struct.calcsize(FORMAT)
MINIMUM_ALIGN = 1 << 10  # 1k aligned


def create_hex(filename, addr, auto_rst, automation_allowed,
               overflow_detect, pad_size):
    file_format = 'hex'
    intel_hex = IntelHex()
    intel_hex.puts(addr, struct.pack(FORMAT, CFG_KEY, FORMAT_LENGTH, auto_rst,
                                     automation_allowed, overflow_detect))
    pad_addr = addr + FORMAT_LENGTH
    pad_byte_count = pad_size - (FORMAT_LENGTH % pad_size)
    pad_data = '\xFF' * pad_byte_count
    intel_hex.puts(pad_addr, pad_data)
    with open(filename, 'wb') as f:
        intel_hex.tofile(f, file_format)


def str_to_int(val):
    return int(val, 0)


POWERS_OF_TWO = [2**num for num in range(0, 32)]
parser = argparse.ArgumentParser(description='Configuration Creator')
parser.add_argument("--addr", type=str_to_int, required=True, help="Address of configuration data")
parser.add_argument("--auto_rst", type=int, required=True, choices=[0, 1], help="Auto reset configuration value")
parser.add_argument("--automation_allowed", type=int, required=True, choices=[0,1], help="Allow automation from filesystem interaction")
parser.add_argument("--overflow_detect", type=int, required=True, choices=[0,1], help="Enable detection of UART overflow")
parser.add_argument("--pad", type=int, default=16, choices=POWERS_OF_TWO, metavar="{1, 2, 4,...}", help="Byte aligned boundary to pad region to")
parser.add_argument("--output_file", type=str, default='settings.hex', help="Name of output file")


def main():
    args = parser.parse_args()
    print "Output file %s" % args.output_file
    print "Config Offset 0x%x" % args.addr
    if args.addr % MINIMUM_ALIGN != 0:
        print "WARNING! Configuration is not on a 1K boundary"
    if args.addr % args.pad != 0:
        print "WARNING! Configuration is not aligned to pad size"
    print "Settings:"
    print "  auto_rst: %i" % args.auto_rst
    print "  automation_allowed: %i" % args.automation_allowed
    print "  overflow_detect: %i" % args.overflow_detect
    print ""
    create_hex(args.output_file, args.addr, args.auto_rst,
               args.automation_allowed, args.overflow_detect, args.pad)

if __name__ == '__main__':
    main()
