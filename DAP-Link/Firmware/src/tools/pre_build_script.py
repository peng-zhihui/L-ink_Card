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

import sys
import os
import argparse
from subprocess import check_output, CalledProcessError

VERSION_GIT_FILE_TEMPLATE = """
/**
 * @file    version_git.h
 * @brief   GIT version info
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef VERSION_GIT_H
#define VERSION_GIT_H

#define GIT_COMMIT_SHA  \"%s\"
#define GIT_LOCAL_MODS  %d

#endif

"""



def generate_version_file(version_git_dir):

    output_file = os.path.join(os.path.normpath(version_git_dir),'version_git.h')
    print("#> Pre-build script start")

    # Get the git SHA.
    print("#> Getting git SHA")
    try:
        git_sha = check_output("git rev-parse --verify HEAD", shell=True)
        git_sha = git_sha.decode().strip()
    except:
        print("#> ERROR: Failed to get git SHA, do you have git.exe in your PATH environment variable?")
        return 1

    # Check are there any local, uncommitted modifications.
    print("#> Checking for local changes")
    try:
        check_output("git diff --no-ext-diff --quiet --exit-code", shell=True)
    except (CalledProcessError, OSError):
        git_has_changes = 1
    else:
        git_has_changes = 0


    # Create the version file. Only overwrite an existing file if it changes.
    version_text = VERSION_GIT_FILE_TEMPLATE % (git_sha, git_has_changes)
    try:
        with open(output_file, 'r') as version_file:
            current_version_text = version_file.read()
    except IOError:
        current_version_text = ''
    if version_text != current_version_text:
        print("#> Writing git version file")
        with open(output_file, 'w+') as version_file:
            version_file.write(version_text)
    else:
        print("#> Keeping git version file since it didn't need to change")

    print("#> Pre-build script completed written %s" % output_file )

    return 0

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='git version generator')
    parser.add_argument("--version_git_dir", type=str, default='../../../source/daplink/', help="directory to output version_git.h file")
    args = parser.parse_args()
    exit(generate_version_file(args.version_git_dir))
