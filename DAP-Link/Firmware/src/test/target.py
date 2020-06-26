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
import os
import time
import info
import mbedapi
from test_info import TestInfoStub
from intelhex import IntelHex

TEST_REPO = 'https://developer.mbed.org/users/c1728p9/code/daplink-validation/'


def load_target_bundle(directory):
    return TargetBundle(directory)


class TargetBundle(object):

    def __init__(self, directory):
        dir_contents = os.listdir(directory)
        name_to_target = {}
        for name in dir_contents:
            base_name, extension = os.path.splitext(name)
            path = directory + os.sep + name
            if os.path.isdir(path):
                # Directories are unused
                pass
            elif os.path.isfile(path):
                if extension not in ('.bin', '.hex'):
                    continue
                if base_name not in name_to_target:
                    name_to_target[base_name] = Target(base_name)
                if extension == '.bin':
                    name_to_target[base_name].set_bin_path(path)
                elif extension == '.hex':
                    name_to_target[base_name].set_hex_path(path)
                else:
                    # Unsupported file type
                    pass
            else:
                assert False
        all_targets = list(name_to_target.values())
        self._target_list = [target for target in all_targets if target.valid]

    def get_target_list(self):
        """Return the target objects associated with this bundle"""
        return self._target_list


def build_target_bundle(directory, username, password, parent_test=None):
    """Build target firmware package"""
    if parent_test is None:
        parent_test = TestInfoStub()
    target_names = info.TARGET_WITH_COMPILE_API_LIST
    for build_name in target_names:
        name_base = os.path.normpath(directory + os.sep + build_name)
        target_hex_path = name_base + '.hex'
        target_bin_path = name_base + '.bin'

        # Build target test image
        test_info = parent_test.create_subtest('Building target %s' %
                                               build_name)
        if not os.path.isdir(directory):
            os.mkdir(directory)
        # Remove previous build files
        if os.path.isfile(target_hex_path):
            os.remove(target_hex_path)
        if os.path.isfile(target_bin_path):
            os.remove(target_bin_path)
        test_info.info('Starting remote build')
        start = time.time()
        built_file = mbedapi.build_repo(username, password,
                                        TEST_REPO, build_name,
                                        directory)
        stop = time.time()
        test_info.info("Build took %s seconds" % (stop - start))
        extension = os.path.splitext(built_file)[1].lower()
        assert extension == '.hex' or extension == '.bin'
        if extension == '.hex':
            intel_hex = IntelHex(built_file)
            # Only supporting devices with the starting
            # address at 0 currently
            assert intel_hex.minaddr() == 0
            intel_hex.tobinfile(target_bin_path)
            os.rename(built_file, target_hex_path)
        if extension == '.bin':
            intel_hex = IntelHex()
            intel_hex.loadbin(built_file, offset=0)
            intel_hex.tofile(target_hex_path, 'hex')
            os.rename(built_file, target_bin_path)

        # Assert that required files are present
        assert os.path.isfile(target_hex_path)
        assert os.path.isfile(target_bin_path)


class Target(object):

    def __init__(self, name, hex_path=None, bin_path=None):
        self._name = name
        self._valid = False
        self._hex_path = None
        self._bin_path = None
        if hex_path is not None:
            self.set_hex_path(hex_path)
        if bin_path is not None:
            self.set_bin_path(bin_path)
        self._valid = True

    def __str__(self):
        return "Name=%s" % self.name

    def set_hex_path(self, path):
        base_name = os.path.basename(path)
        assert self._hex_path is None
        assert base_name == self._name + '.hex'
        path = os.path.abspath(path)
        assert os.path.isfile(path)
        self._hex_path = path

    def set_bin_path(self, path):
        base_name = os.path.basename(path)
        assert self._bin_path is None
        assert base_name == self._name + '.bin'
        path = os.path.abspath(path)
        assert os.path.isfile(path)
        self._bin_path = path

    @property
    def valid(self):
        hex_valid = self._hex_path is not None
        bin_valid = self._bin_path is not None
        return hex_valid and bin_valid and self._valid

    @property
    def name(self):
        return self._name

    @property
    def hex_path(self):
        return self._hex_path

    @property
    def bin_path(self):
        return self._bin_path
