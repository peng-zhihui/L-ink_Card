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
from enum import Enum


class FirmwareBundle(object):

    def get_firmware_list(self):
        """Return the firmware objects associated with this bundle"""
        raise NotImplementedError()

    @property
    def build_sha(self):
        """The GIT SHA this build was created at as a string or None"""
        raise NotImplementedError()

    @property
    def build_local_mods(self):
        """True if this was a clean build, False otherwise"""
        raise NotImplementedError()


class Firmware(object):
    """Class wrapping a firmware build"""

    class TYPE(Enum):
        BOOTLOADER = 1
        INTERFACE = 2

    def __str__(self):
        """A string that describes this firmware"""
        raise NotImplementedError()

    @property
    def name(self):
        """Name of this project"""
        raise NotImplementedError()

    @property
    def hic_id(self):
        """HIC ID for the type of board this firmware can run on"""
        raise NotImplementedError()

    @property
    def type(self):
        """Build type - either interface or bootloader"""
        raise NotImplementedError()

    @property
    def bin_path(self):
        """Path to the binary vesion of this firmware or None"""
        raise NotImplementedError()

    @property
    def hex_path(self):
        """Path to the hex vesion of this firmware or None"""
        raise NotImplementedError()

    @property
    def elf_path(self):
        """Path to the hex vesion of this firmware or None"""
        raise NotImplementedError()
