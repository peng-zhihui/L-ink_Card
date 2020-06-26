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

import os
import shutil
import subprocess
from generate_mbedcli_files import generate_mbedcli_files

def call_and_copy_output(args):
    try:
        process = subprocess.Popen(args, stdout=None, stderr=subprocess.STDOUT)
        process.wait()
        if process.returncode != 0:
            print("Error - mbed compile!")
            exit(process.returncode)
    except (OSError, ValueError) as e:
        print("Error - Cannot do mbed compile")
        print(e.output)
        exit(1)

def mbedcli_run(daplink_dir, build_folder, project, toolchain, clean, verbosity):
    generate_mbedcli_files(os.path.join(daplink_dir, "projects.yaml"), project)
    project_dir=os.path.join(daplink_dir, build_folder, project.upper())
    if clean is True and os.path.exists(project_dir):
        print("Deleting %s" % project_dir)
        shutil.rmtree(project_dir, ignore_errors=True)
    args = ["mbed", "compile", "-m", project, "-t", toolchain, "--profile", "custom_profile.json"]
    if verbosity is not None:
        args.append("-" + "v" * verbosity)
    call_and_copy_output(args)
    cli_name_out = os.path.basename(daplink_dir)
    build_dir = os.path.join(project_dir, toolchain+"-CUSTOM_PROFILE")
    for file in os.listdir(build_dir):
        if file.startswith(cli_name_out):
            rename_file = os.path.join(build_dir, file.replace(cli_name_out, project, 1))
            if os.path.exists(rename_file):
                os.remove(rename_file)
            os.rename(os.path.join(build_dir, file), rename_file)
    cli_hex_output = os.path.join(build_dir, project + ".hex")
    crc_file_output = os.path.join(build_dir, project + "_crc")
    return (cli_hex_output,crc_file_output)
