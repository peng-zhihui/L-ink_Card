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
import info
import re
import firmware


def load_bundle_from_release(directory):
    """ Return a bundle representing the given build"""
    return ReleaseFirmwareBundle(directory)


def load_bundle_from_project(tool='uvision'):
    """
    Return a bundle for the given tool

    Note - This does not build the project.  It only returns the
    firmware that has already been built.
    """
    assert (tool == 'uvision' or tool == 'mbedcli'), 'Input tool %s is not supported' % (tool) 

    self_path = os.path.abspath(__file__)
    test_dir = os.path.dirname(self_path)
    daplink_dir = os.path.dirname(test_dir)
    assert os.path.basename(test_dir) == 'test', 'The script "%s" must be ' \
        'located in the "test" directory of daplink to work correctly.'

    if tool == 'uvision':
        project_dir = os.path.join(daplink_dir, 'projectfiles', tool)
        build_folder = 'build'
    elif tool == 'mbedcli':
        project_dir = os.path.join(daplink_dir, 'BUILD')
        build_folder = 'ARM-CUSTOM_PROFILE'

    return ProjectFirmwareBundle(project_dir, build_folder)


class ReleaseFirmwareBundle(firmware.FirmwareBundle):
    """Class to abstract access a formal build as a bundle"""

    def __init__(self, directory):
        bundle_contents = os.listdir(directory)
        firmware_list = []
        for name in bundle_contents:
            path = directory + os.sep + name
            if os.path.isdir(path):
                daplink_firmware = DAPLinkFirmware(name, self, path)
                if daplink_firmware.valid:
                    firmware_list.append(daplink_firmware)
            elif os.path.isfile(path):
                # Parse relevent info
                pass
            else:
                assert False
        self._firmware_list = firmware_list

    def get_firmware_list(self):
        return self._firmware_list

    @property
    def build_sha(self):
        raise NotImplementedError()

    @property
    def build_local_mods(self):
        raise NotImplementedError()


class ProjectFirmwareBundle(firmware.FirmwareBundle):
    """Class to abstract access to daplink's build directory as a bundle"""

    def __init__(self, project_dir, build_folder):

        if not os.path.exists(project_dir):
            print ("Error: DAPLink project folder %s missing" % project_dir)
            exit(-1)

        project_dir_list = os.listdir(project_dir)
        if not project_dir_list:
            print ("Error: DAPLink projects not build yet at %s" % project_dir)
            exit(-1)

        firmware_list = []
        for name in project_dir_list:
            build_dir = os.path.join(project_dir, name, build_folder)
            if os.path.isdir(build_dir):
                daplink_firmware = DAPLinkFirmware(name.lower(), self, build_dir)
                if daplink_firmware.valid:
                    firmware_list.append(daplink_firmware)
        self._firmware_list = firmware_list

    def get_firmware_list(self):
        return self._firmware_list

    @property
    def build_sha(self):
        raise NotImplementedError()

    @property
    def build_local_mods(self):
        raise NotImplementedError()


class DAPLinkFirmware(firmware.Firmware):
    """Class to abstract access to a daplink firmware image"""

    _IF_RE = re.compile("^([a-z0-9]+)([_a-z0-9]*)_if$")
    _BL_RE = re.compile("^([a-z0-9]+)_bl$")

    def __init__(self, name, bundle, directory):
        self._name = name
        self._bundle = bundle
        self._directory = directory
        self._valid = False

        # Set type
        self._type = None
        string_hic = None
        match = self._IF_RE.match(name)
        if match:
            string_hic = match.group(1)
            self._type = self.TYPE.INTERFACE
        match = self._BL_RE.match(name)
        if match:
            string_hic = match.group(1)
            self._type = self.TYPE.BOOTLOADER
        if self._type is None:
            assert False, 'Bad project name "%s"' % name

        # Set HIC
        assert string_hic in info.HIC_STRING_TO_ID, 'Unknown HIC "%s" must ' \
            'be added to HIC_STRING_TO_ID in info.py' % string_hic
        self._hic_id = info.HIC_STRING_TO_ID[string_hic]

        # Check firmware name and type
        assert self._type in self.TYPE, "Invalid type %s" % self._type
        if self._type is self.TYPE.INTERFACE:
            if name not in info.FIRMWARE_SET:
                print('Warning: board "%s" no entry in SUPPORTED_CONFIGURATIONS in info.py' % name)

        # Set file paths
        self._bin_path = self._directory + os.sep + '%s_crc.bin' % name
        self._hex_path = self._directory + os.sep + '%s_crc.hex' % name
        self._bin_path = os.path.abspath(self._bin_path)
        self._hex_path = os.path.abspath(self._hex_path)
        if not os.path.isfile(self._bin_path):
            return  # Failure
        if not os.path.isfile(self._hex_path):
            return  # Failure

        self._valid = True

    def __str__(self):
        return "Name=%s" % (self.name)

    @property
    def valid(self):
        """Set to True if the firmware is valid"""
        return self._valid

    @property
    def name(self):
        return self._name

    @property
    def hic_id(self):
        return self._hic_id

    @property
    def type(self):
        return self._type

    @property
    def bin_path(self):
        return self._bin_path

    @property
    def hex_path(self):
        return self._hex_path

    @property
    def elf_path(self):
        return self._elf_path
