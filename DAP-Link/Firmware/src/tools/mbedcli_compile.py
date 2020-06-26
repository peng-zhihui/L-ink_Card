#
# DAPLink Interface Firmware
# Copyright (c) 2009-2018, ARM Limited, All Rights Reserved
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
import os
import sys
import shutil
import yaml
import mbedcli_tools
from pre_build_script import generate_version_file
from package_release_files import package_release_files
from post_build_script import post_build_script

self_path = os.path.abspath(__file__)
tools_dir = os.path.dirname(self_path)
daplink_dir = os.path.dirname(tools_dir)
test_dir = os.path.join(daplink_dir, "test")
sys.path.append(test_dir)

import info

PROJECTS_YAML = "projects.yaml"
VERSION_YAML = "version.yaml"

def main():
    """python -u tools/mbedcli_compile.py kl26z_microbit_if kl26z_bl --clean"""
    #parse project yaml file for project list
    project_list = []
    with open(PROJECTS_YAML, 'r') as top_yaml:
        try:
            topdict = yaml.load(top_yaml)
            for dict_key in topdict:
                if dict_key == 'projects':
                    for project in topdict[dict_key]:
                        project_list.append(project)
                    break
        except yaml.YAMLError as ex:
            print("Found yaml parse error", ex)
    #parse the arguments
    projects = "List of supported projects\n\n" + ", ".join(project_list) #
    parser = argparse.ArgumentParser(description='mbedcli compile support for DAPLink', epilog=projects, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('projects', help='Selectively compile only the firmware specified otherwise all projects',
                        nargs='*', type=str, default=[])
    parser.add_argument("--board-id", type=str, help="board id to for the target in hex")
    parser.add_argument("--family-id", type=str, help="family id to for the target in hex")
    parser.add_argument("--bin-offset", type=str, help="binary offset in hex")
    parser.add_argument('--release', dest='release', action='store_true', help='Create a release with the yaml version file')
    parser.add_argument('--build-folder', type=str, default='BUILD', help='Release directory to grab files from')
    parser.add_argument('--release-folder', type=str, default='firmware', help='Directory to create and place files in')
    parser.add_argument('--toolchain', type=str, default='ARM', help='Toolchain directory if present')
    parser.add_argument('--clean', dest='clean', action='store_true', help='Rebuild or delete build folder before compile')
    parser.add_argument('-v', dest='verbosity', action='count', help='Pass verbosity level to mbed compile -vv for more')
    parser.set_defaults(clean=False)
    parser.set_defaults(release=False)
    args = parser.parse_args()
    self_path = os.path.abspath(__file__)
    tools_dir = os.path.dirname(self_path)
    daplink_dir = os.path.dirname(tools_dir)
    if os.path.basename(tools_dir) != "tools":
        print("Error - this script must be run from the tools directory")
        exit(-1)
    version_git_dir = os.path.join(daplink_dir, "source", "daplink")
    generate_version_file(version_git_dir)
    if not args.projects == []: 
        for project in args.projects:
            print("Compiling %s" % project)
            (cli_hex_output,crc_file_output) = mbedcli_tools.mbedcli_run(daplink_dir, args.build_folder, project, args.toolchain, args.clean, args.verbosity)
            print("Creating crc padded binaries %s" % os.path.basename(cli_hex_output))
            post_build_script(cli_hex_output, crc_file_output, args.board_id, args.family_id, args.bin_offset)
    else:
        print("compiling all firmware")
        #generate a dictionary of board ID, and family ID
        id_map = {}
        for board_id, family_id, firmware, bootloader, target in info.SUPPORTED_CONFIGURATIONS:
            if firmware in id_map:
                id_map[firmware].append((hex(board_id), hex(family_id)))
            else:
                id_map[firmware] = [(hex(board_id), hex(family_id))]
        for project in project_list:
            print("Compiling %s" % project)
            (cli_hex_output,crc_file_output) = mbedcli_tools.mbedcli_run(daplink_dir, args.build_folder, project, args.toolchain, args.clean, args.verbosity)
            print("Creating crc padded binaries %s" % os.path.basename(cli_hex_output))
            #can be a legacy build or 0 board_id and family_id
            post_build_script(cli_hex_output, crc_file_output)
            #do a build with board_id and family_id
            if project in id_map:
                for (boardid, familyid) in id_map[project]:
                    print(project, boardid, familyid)
                    post_build_script(cli_hex_output, crc_file_output, boardid, familyid)
    if args.release is True:
        release_version = 0
        with open(os.path.join("records","tools", VERSION_YAML), 'r') as ver_yaml:
            try:
                verdict = yaml.load(ver_yaml)
                release_version = int(verdict['common']['macros'][0].split('=')[1])
            except yaml.YAMLError as ex:
                print("Found yaml parse error", ex)

        release_dir = args.release_folder + "_%04i" % release_version 
        if os.path.exists(release_dir):
            print("Deleting %s" % release_dir)
            shutil.rmtree(release_dir, ignore_errors=True)
        print("Releasing directory: " + release_dir)
        toolchain_dir = args.toolchain+"-CUSTOM_PROFILE"
        package_release_files(args.build_folder, release_dir, release_version, toolchain_dir)

main()
