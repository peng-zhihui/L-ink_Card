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

from __future__ import absolute_import

from builtins import bytes

import argparse


def dec_or_hex(val):
    return int(val, 0)


def create_padded_image(input_file, output_file, start,
                        pad_start, copy_size):
    pad_size = start - pad_start
    with open(input_file, 'rb') as file_handle:
        data = file_handle.read()
    output_data = data[0:copy_size] + bytes([0xff] * (pad_size - copy_size)) + data
    with open(output_file, 'wb') as file_handle:
        data = file_handle.write(output_data)


def main():
    parser = argparse.ArgumentParser(description='File Padder')
    parser.add_argument("bin", type=str, default=None,
                        help="Input binary file")
    parser.add_argument("--start", type=dec_or_hex, default=0x8000,
                        help="Starting address of input binary file")
    parser.add_argument("--padded_start", type=dec_or_hex, default=0x5000,
                        help="Starting address after padding.")
    parser.add_argument("--output", type=str, required=True,
                        help="Output file")
    parser.add_argument("--copysize", type=str, default=0x40,
                        help="Size of original binary to copy")
    args = parser.parse_args()

    # Data is appened to front so padded start must be less than start
    assert args.start > args.padded_start
    create_padded_image(args.bin, args.output, args.start,
                        args.padded_start, args.copysize)


if __name__ == "__main__":
    main()
