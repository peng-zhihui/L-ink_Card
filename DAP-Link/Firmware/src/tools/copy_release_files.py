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

"""
Copy release files from the uvision build directories into a release package

This script is intended to be called the script creating a daplink build.
"""
from __future__ import absolute_import
from __future__ import print_function

import os
import shutil
import glob
import argparse

from subprocess import check_output, CalledProcessError

COPY_PATTERN_LIST = [
    "%s_crc*.bin",
    "%s_crc*.hex",
    "%s_crc*.txt",
    ]
OPTIONAL_COPY_PATTERN_LIST = [
    "%s.axf",
    "%s.elf",
    "%s_crc*.c",
    "%s.build_log.htm",
    "%s.map",
    "%s.htm",
    "%s_map.html",
]

TOOL_DIR = { 
    'uvision' : { 'proj_dir': os.path.join('projectfiles', 'uvision') , 'rel_dir' : 'uvision_release', 'build_dir' : 'build' },
    'mbedcli' : { 'proj_dir': 'BUILD' , 'rel_dir' : 'mbedcli_release', 'build_dir' : 'ARM-CUSTOM_PROFILE' }        
}

def generate_info_files(dir):

    output_info_file = os.path.join(os.path.normpath(dir),'git_info.txt')
    output_requirements_file = os.path.join(os.path.normpath(dir),'build_requirements.txt')

    # Get the git SHA.
    try:
        git_sha = check_output("git rev-parse --verify HEAD", shell=True)
        git_sha = git_sha.decode().strip()
    except:
        print("ERROR - failed to get git SHA, do you have git.exe in your PATH environment variable?")
        return 1

    # Check are there any local, uncommitted modifications.
    try:
        check_output("git diff --no-ext-diff --quiet --exit-code", shell=True)
    except (CalledProcessError, OSError):
        git_has_changes = '1'
    else:
        git_has_changes = '0'

    # Get the requirements version.
    try:
        pip_freeze = check_output("pip list", shell=True).decode().strip()
    except:
        print("ERROR - failed requirements, pip not installed?")
        return 1


    # Create the version files
    try:
        with open(output_info_file, 'w+') as file:
            file.write(git_sha + '\n' + 'Uncommitted Changes:' + git_has_changes + '\n' )
        with open(output_requirements_file, 'w+') as file:
            file.write(pip_freeze)
    except IOError:
        print("Error - failed to write information and version files")
        return 1;
    return 0


def main():
    """Copy imporant files for the current release"""
    parser = argparse.ArgumentParser(description='Copy imporant files for the current release')
    parser.add_argument('--project-tool', type=str, default='uvision', choices=['uvision', 'mbedcli'], help='Choose from uvision and mbedcli')
    args = parser.parse_args()

    self_path = os.path.abspath(__file__)
    tools_dir = os.path.dirname(self_path)
    daplink_dir = os.path.dirname(tools_dir)

    if os.path.basename(tools_dir) != "tools":
        print("Error - this script must be run from the tools directory")
        exit(-1)

    proj_dir = os.path.join(daplink_dir, TOOL_DIR[args.project_tool]['proj_dir'])
    rel_dir = os.path.join(daplink_dir, TOOL_DIR[args.project_tool]['rel_dir'])
    build_dir = TOOL_DIR[args.project_tool]['build_dir']
    # Make sure uvision dir is present
    if not os.path.isdir(proj_dir):
        print("Error - uvision directory '%s' missing" % proj_dir)
        exit(-1)

    # Clean release dir is present
    if os.path.isdir(rel_dir):
        shutil.rmtree(rel_dir)

    os.mkdir(rel_dir)

    generate_info_files(rel_dir)
    
    project_list = os.listdir(proj_dir)
    for project in project_list:
        src_dir = os.path.join(proj_dir, project, build_dir)
        dest_dir = os.path.join(rel_dir, project.lower())

        #only copy a built project
        if not os.path.exists(src_dir):
            continue

        # File must not have been copied already
        if os.path.exists(dest_dir):
            print("Error - package dir '%s' alread exists" % dest_dir)
            exit(-1)
        os.mkdir(dest_dir)

        for file_pattern in COPY_PATTERN_LIST:
            file_name = file_pattern % project.lower()
            file_source = os.path.join(src_dir, file_name)
            for file_wild in glob.glob(file_source):
                shutil.copy(file_wild, dest_dir)
        for file_pattern in OPTIONAL_COPY_PATTERN_LIST:
            file_name = file_pattern % project.lower()
            file_source = os.path.join(src_dir, file_name)
            for file_wild in glob.glob(file_source):
                shutil.copy(file_wild, dest_dir)

main()
