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

import os
import sys
import shutil
import argparse
import yaml
import subprocess

self_path = os.path.abspath(__file__)
tools_dir = os.path.dirname(self_path)
daplink_dir = os.path.dirname(tools_dir)
test_dir = os.path.join(daplink_dir, "test")
sys.path.append(test_dir)

import info

from make_update_yml import DefaultList
from make_update_yml import TargetList
from make_update_yml import InstructionsText
from make_update_yml import make_update_yml_file

import zipfile

def make_bin_zip(dir, name):
    working_dir = os.getcwd()
    os.chdir(dir)
    with zipfile.ZipFile(name, mode='w') as zipf:
        for file in os.listdir("./"):
            if file.endswith(".bin") or file.endswith(".hex"):
                zipf.write(os.path.join("./", file))
    #go back
    os.chdir(working_dir)

def package_release_files(source, dest, version, toolchain):

    proj_dir = source
    output_dir = dest
    build_number = "%04i" % version

    update_yml_entries = [{'default':DefaultList([
            ('website', 'http://os.mbed.com/platforms'),
            ('fw_version', "'" + build_number + "'"),
            ('image_format', '.bin'),
            ('instructions', InstructionsText['default'])
            ]) }]

    if os.path.exists(output_dir):
        shutil.rmtree(output_dir)
        print (output_dir + ' existed and deleted!!')

    os.mkdir(output_dir)

    for prj_name, legacy, offset, extension in info.PROJECT_RELEASE_INFO:
        legacy_str = "_legacy" if legacy else ""
        source_offset_str = "_0x%04x" % offset if legacy else ""
        source_name = prj_name + "_crc" + legacy_str + source_offset_str + "." + extension
        source_dir_part = os.path.join(proj_dir, prj_name, toolchain).upper()
        source_path = os.path.join(source_dir_part, source_name)
        if not os.path.isfile(source_path):
            print("Warning %s not added to release" % prj_name)
            continue
        items = prj_name.split('_')  # "k20dx_frdmk22f_if" -> ("k20dx", "frdmk22f", "if")
        assert items[-1] == "if", "Unexpected string: %s" % items[2]
        host_mcu = items[0]
        base_name = '_'.join(items[1:-1])
        dest_offset_str = "_0x%04x" % offset
        dest_name = build_number + "_" + host_mcu + "_" + base_name + dest_offset_str + "." + extension
        dest_path = os.path.join(output_dir, dest_name)
        shutil.copyfile(source_path, dest_path)

        product_code = 'NOT SUPPORTED'
        for board_id, family_id, fimware, bootloader, target in info.SUPPORTED_CONFIGURATIONS:
            if fimware == prj_name:
                product_code = board_id
                if target is not None:
                    target_name = target
                else:
                    target_name = base_name.upper()

                fw_instuction = InstructionsText['default']
                for fw_name_key in InstructionsText:
                    if fw_name_key in dest_name.lower():
                        fw_instuction = InstructionsText[fw_name_key]
                        break;

                if extension == 'bin':
                    update_yml_entries.append({target_name:TargetList([
                        ('name', target_name),
                        ('product_code', "'" + format(product_code, '04x') + "'"),
                        ('fw_name', host_mcu + "_" + base_name + dest_offset_str),
                        ('instructions', fw_instuction)
                        ])});

    make_bin_zip(output_dir, build_number + '_release_package_' + subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode().strip() + '.zip')

    make_update_yml_file(os.path.join(output_dir, 'update.yml'), update_yml_entries, explicit_start=True)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Package a release for distribution')
    parser.add_argument('source', help='Release directory to grab files from')
    parser.add_argument('dest', help='Directory to create and place files in')
    parser.add_argument('version', type=int, help='Version number of this release')
    parser.add_argument('--toolchain', type=str, default='', help='Toolchain directory if present')
    args = parser.parse_args()

    print("args",args.source,args.dest,args.version,args.toolchain)
    
    package_release_files(args.source,args.dest,args.version,args.toolchain)
