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


class Board(object):

    def update_interface(self):
        raise NotImplementedError()

    def update_bootloader(self):
        raise NotImplementedError()

    def prepare_for_testing(self):
        raise NotImplementedError()

    @property
    def hw_msd_support(self):
        """Set to True if the hardware supports mass storage"""
        raise NotImplementedError()

    @property
    def hw_cdc_support(self):
        """Set to True if the hardware supports a serial port"""
        raise NotImplementedError()

    @property
    def hw_hid_support(self):
        """Set to True if the hardware supports HID"""
        raise NotImplementedError()

    @property
    def mount_point(self):
        """Mount point of the drive or None if no SW support"""
        raise NotImplementedError()

    @property
    def serial_port(self):
        """Serial port string usable with pySerial  or None if no SW support"""
        raise NotImplementedError()

    @property
    def board_id(self):
        """The ID of this board type"""
        raise NotImplementedError()

    @property
    def unique_id(self):
        """ID which uniquely identifies this board"""
        raise NotImplementedError()
