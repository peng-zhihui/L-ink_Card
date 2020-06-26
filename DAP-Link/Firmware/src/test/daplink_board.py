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
import re
import time
import subprocess
import sys
import binascii
import itertools
import mbed_lstools
import info
import test_daplink
from test_info import TestInfoStub, TestInfo
from intelhex import IntelHex
from pyocd.core.helpers import ConnectHelper

FILE_IGNORE_PATTERN_LIST = [
    re.compile("\\._\\.Trashes")
]


# This prevents the following error message from getting
# displayed on windows if the mbed dismounts unexpectedly
# during a transfer:
#   There is no disk in the drive. Please insert a disk into
#   drive \Device\<Harddiskx>\<rdrive>
def disable_popup():
    if sys.platform.startswith("win"):
        # pylint: disable=invalid-name
        import ctypes
        SEM_FAILCRITICALERRORS = 1
        GetErrorMode = \
            ctypes.windll.kernel32.GetErrorMode  # @UndefinedVariable
        GetErrorMode.restype = ctypes.c_uint
        GetErrorMode.argtypes = []
        SetErrorMode = \
            ctypes.windll.kernel32.SetErrorMode  # @UndefinedVariable
        SetErrorMode.restype = ctypes.c_uint
        SetErrorMode.argtypes = [ctypes.c_uint]

        err_mode = GetErrorMode()
        err_mode |= SEM_FAILCRITICALERRORS
        SetErrorMode(err_mode)

disable_popup()


def get_all_attached_daplink_boards():
    all_boards = []
    lstools = mbed_lstools.create()
    mbed_list = lstools.list_mbeds()
    for mbed in mbed_list:
        unique_id = mbed['target_id']
        board = DaplinkBoard(unique_id)
        if board._mode is not None: #Valid daplink should have set this mode
            all_boards.append(board)
        else:
            print("Warning: DAPLink tests cannot be done on board %s" % board.unique_id)
    return all_boards


def _unique_id_to_host_id(unique_id):
    """Return the chip id unique to the daplink host procesor

    Unique ID has the following fomat
    Board ID - 4 bytes
    Version  - 4 bytes
    Host ID  - Everything else
    """
    return unique_id[8:8 + 32]


def _get_board_endpoints(unique_id):
    """Return a tuple of unique_id, serial_port, mount_point"""
    lstools = mbed_lstools.create()
    mbed_list = lstools.list_mbeds()

    host_id = _unique_id_to_host_id(unique_id)
    for mbed in mbed_list:
        mbed_unique_id = mbed['target_id']
        mbed_serial_port = mbed['serial_port']
        mbed_mount_point = mbed['mount_point']
        mbed_host_id = _unique_id_to_host_id(mbed_unique_id)
        if mbed_host_id == host_id:
            return mbed_unique_id, mbed_serial_port, mbed_mount_point
    return None


def _ranges(i):
    for _, b in itertools.groupby(enumerate(i), lambda x_y: x_y[1] - x_y[0]):
        b = list(b)
        yield b[0][1], b[-1][1]


def _parse_kvp_file(file_path, parent_test=None):
    """Parse details.txt and return True if successful"""
    test_info = None
    kvp = {}
    if parent_test is not None:
        test_info = parent_test.create_subtest('parse_kvp_file')
    line_format = re.compile("^([a-zA-Z0-9 ]+): +(.+)$")
    if not os.path.isfile(file_path):
        return kvp

    with open(file_path, "r") as file_handle:
        for line in file_handle:
            if len(line) <= 0:
                if test_info is not None:
                    test_info.failure("Empty line in %s" % file_path)
                continue

            if line[0] == '#':
                # The line is a comment
                continue

            match = line_format.match(line)
            if match is None:
                if test_info is not None:
                    test_info.failure("Invalid line: %s" % line)
                continue

            key = match.group(1)
            key = key.lower().replace(" ", "_")
            value = match.group(2)
            value = value.lower()
            value = value.strip()
            if key in kvp:
                if test_info is not None:
                    test_info.failure("Duplicate key %s" % key)
                continue
            kvp[key] = value
    return kvp


def _compute_crc(hex_file_path):
    # Read in hex file
    new_hex_file = IntelHex()
    new_hex_file.padding = 0xFF
    new_hex_file.fromfile(hex_file_path, format='hex')

    # Get the starting and ending address
    addresses = new_hex_file.addresses()
    addresses.sort()
    start_end_pairs = list(_ranges(addresses))
    regions = len(start_end_pairs)
    assert regions == 1, ("Error - only 1 region allowed in "
                          "hex file %i found." % regions)
    start, end = start_end_pairs[0]

    # Compute checksum over the range (don't include data at location of crc)
    size = end - start + 1
    crc_size = size - 4
    data = new_hex_file.tobinarray(start=start, size=crc_size)
    data_crc32 = binascii.crc32(data) & 0xFFFFFFFF

    # Grab the crc from the image
    embedded_crc32 = (((new_hex_file[end - 3] & 0xFF) << 0) |
                      ((new_hex_file[end - 2] & 0xFF) << 8) |
                      ((new_hex_file[end - 1] & 0xFF) << 16) |
                      ((new_hex_file[end - 0] & 0xFF) << 24))
    return data_crc32, embedded_crc32


def _run_chkdsk(drive):
    args = ["chkdsk", drive]
    process = subprocess.Popen(args, stdin=subprocess.PIPE,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    process.communicate(input=bytearray('n\r\n',encoding='ascii'))  # Answer no if prompted
    process.wait()
    return process.returncode


class AssertInfo(object):

    def __init__(self, file_name, line_number):
        self._file = file_name
        self._line = line_number

    @property
    def file(self):
        return self._file

    @property
    def line(self):
        return self._line


class DaplinkBoard(object):

    MODE_IF = "interface"
    MODE_BL = "bootloader"

    # Keys for details.txt
    KEY_UNIQUE_ID = "unique_id"
    KEY_HIC_ID = "hic_id"
    KEY_MODE = "daplink_mode"
    KEY_BL_VERSION = "bootloader_version"
    KEY_IF_VERSION = "interface_version"
    KEY_GIT_SHA = "git_sha"
    KEY_LOCAL_MODS = "local_mods"
    KEY_USB_INTERFACES = "usb_interfaces"
    KEY_BL_CRC = "bootloader_crc"
    KEY_IF_CRC = "interface_crc"
    KEY_REMOUNT_COUNT = "remount_count"

    def __init__(self, unique_id):

        self.unique_id = unique_id
        self.details_txt = None
        self._mode = None
        self._remount_count = None
        self._assert = None
        self._check_fs_on_remount = False
        self._manage_assert = False
        self.update_board_info()

    def __str__(self):
        return "Name=%s Unique ID=%s" % (self.name, self.get_unique_id())

    def get_unique_id(self):
        return self.unique_id

    def get_board_id(self):
        return self.board_id

    @property
    def hic_id(self):
        return self._hic_id

    @property
    def name(self):
        if self.board_id in info.BOARD_ID_TO_BUILD_TARGET:
            board_target = info.BOARD_ID_TO_BUILD_TARGET[self.board_id]
        else:
            board_target = "Unknown"
        return board_target

    def get_serial_port(self):
        return self.serial_port

    def get_mount_point(self):
        return self.mount_point

    def get_connected(self):
        """Check if the board is connected"""
        return os.path.isdir(self.mount_point)

    def get_failure_message_and_type(self):
        """Get the failure message and types from fail.txt

        return None if there there is no failure
        """
        error = None
        error_type = None
        fail_file = self.get_file_path('FAIL.TXT')
        if not self.get_connected():
            raise Exception('Board not connected')
        if os.path.isfile(fail_file):
            with open(fail_file, 'r') as fail_file_handle:
                msg = fail_file_handle.read()
                lines = msg.splitlines()
                if len(lines) == 2:
                    if lines[0].startswith('error: '):
                        error = lines[0][7:]
                    else:
                        raise Exception('Can not parse error line in FAIL.TXT')
                    if lines[1].startswith('type: '):
                        error_type = lines[1][6:]
                    else:
                        raise Exception('Can not parse type line in FAIL.TXT')
                else:
                    raise Exception('Wrong number of lines in FAIL.TXT, expected: 2')
        return error, error_type

    def get_assert_info(self):
        """Return an AssertInfo if an assert occurred, else None"""
        return self._assert

    def get_mode(self):
        """Return either MODE_IF or MODE_BL"""
        assert ((self._mode is DaplinkBoard.MODE_BL) or
                (self._mode is DaplinkBoard.MODE_IF))
        return self._mode

    def get_file_path(self, file_name):
        """Convenience function to the path to a file on the drive"""
        return os.path.normpath(self.mount_point + os.sep + file_name)

    def refresh(self, parent_test):
        """Remount driver to get updated contents"""
        refresh_filename = self.get_file_path('REFRESH.ACT')
        with open(refresh_filename, 'wb') as _:
            pass
        self.wait_for_remount(parent_test)

    def set_mode(self, mode, parent_test=None):
        """Set the mode to either MODE_IF or MODE_BL"""
        assert ((mode is DaplinkBoard.MODE_BL) or
                (mode is DaplinkBoard.MODE_IF))
        if parent_test is None:
            parent_test = TestInfoStub()
        test_info = parent_test.create_subtest('set_mode')
        current_mode = self.get_mode()
        if current_mode is mode:
            # No mode change needed
            return

        if mode is self.MODE_BL:
            test_info.info("changing mode IF -> BL")
            # Create file to enter BL mode
            start_bl_path = self.get_file_path('START_BL.ACT')
            with open(start_bl_path, 'wb') as _: pass
        elif mode is self.MODE_IF:
            test_info.info("changing mode BL -> IF")
            # Create file to enter IF mode
            start_if_path = self.get_file_path('START_IF.ACT')
            with open(start_if_path, 'wb') as _: pass
        else:
            test_info.warning("Board is in unknown mode")
        self.wait_for_remount(test_info)

        new_mode = self.get_mode()
        if new_mode != mode:
            test_info.failure("Board in wrong mode: %s" % new_mode)
            raise Exception("Could not change board mode")

    def set_check_fs_on_remount(self, enabled):
        assert isinstance(enabled, bool)
        self._check_fs_on_remount = enabled
        self.set_assert_auto_manage(enabled)

    def set_assert_auto_manage(self, enabled):
        assert isinstance(enabled, bool)
        self.clear_assert()
        self._manage_assert = enabled

    def clear_assert(self):
        assert_path = self.get_file_path("ASSERT.TXT")
        if os.path.isfile(assert_path):
            os.remove(assert_path)
            self.wait_for_remount(TestInfoStub())

    def run_board_test(self, parent_test):
        test_daplink.daplink_test(self, parent_test)

    def read_target_memory(self, addr, size, resume=True):
        assert self.get_mode() == self.MODE_IF
        with ConnectHelper.session_with_chosen_probe(unique_id=self.get_unique_id(),
                                                     resume_on_disconnect=resume) as session:
            data = session.target.read_memory_block8(addr, size)
        return bytearray(data)

    def test_fs(self, parent_test):
        """Check if the raw filesystem is valid"""
        if sys.platform.startswith("win"):
            test_info = parent_test.create_subtest('test_fs')
            returncode = _run_chkdsk(self.mount_point)
            test_info.info('chkdsk returned %s' % returncode)
            if returncode != 0:
                test_info.failure('Disk corrupt')

            # Windows 8/10 workaround - rerun chkdsk until disk caching is on
            # Notes about this problem:
            # - This is less likely to occur when the "storage" service is
            #     turned off and/or you are running as administrator
            # - When creating a directory with os.mkdir the
            #     following error occurs: "WindowsError: [Error 1392] The
            #     file or directory is corrupted and unreadable: '<directory>'"
            # - When creating a file with open(<filename>, "wb") the
            #     following error occurs: "OError: [Errno 22] invalid
            #     mode ('wb') or filename: '<filename>'"
            # - When a file or directory is created on the drive in explorer
            #     and you preform a refresh, the newly created file or
            #     directory disappears
            persist_test_dir = self.get_file_path("persist_test_dir")
            for _ in range(10):
                try:
                    os.mkdir(persist_test_dir)
                except EnvironmentError as exception:
                    test_info.info("cache check exception %s" % exception)
                if os.path.exists(persist_test_dir):
                    os.rmdir(persist_test_dir)
                    break
                test_info.info("running checkdisk to re-enable caching")
                _run_chkdsk(self.mount_point)
            else:
                raise Exception("Unable to re-enable caching")

        # TODO - as a future improvement add linux and mac support

    # Tests for the following:
    # 1. Correct files present                -TODO
    # 2. Contents of file are valid ascii
    # 3. Each line ends with \r\n
    # 4. There is no whitespace at the end of the line
    # 5. Each file ends with \r\n
    def test_fs_contents(self, parent_test):
        """Check if the file contents are valid"""
        test_info = parent_test.create_subtest('test_fs_contents')
        non_ascii = b'[^\x20-\x7F\r\n]'
        non_cr_lf = b'\r[^\n]|[^\r]\n'
        trail_white = b'(?:\ \r|\ \n)'
        end_of_file = b'\r\n$'
        files = os.listdir(self.mount_point)
        non_ascii_re = re.compile(non_ascii)
        non_cr_lf_re = re.compile(non_cr_lf)
        trail_white_re = re.compile(trail_white)
        end_of_file_re = re.compile(end_of_file)
        for filename in files:
            filepath = self.get_file_path(filename)
            if not os.path.isfile(filepath):
                test_info.info("Skipping non file item %s" % filepath)
                continue
            skip = False
            for pattern in FILE_IGNORE_PATTERN_LIST:
                if pattern.match(filename):
                    skip = True
                    break
            if skip:
                continue

            with open(filepath, 'rb') as file_handle:
                file_contents = file_handle.read()
            if non_ascii_re.search(file_contents):
                test_info.failure("Non ascii characters in %s" % filepath)
            elif non_cr_lf_re.search(file_contents):
                test_info.failure("File has non-standard line endings %s" %
                                  filepath)
            elif trail_white_re.search(file_contents):
                test_info.warning("File trailing whitespace %s" %
                                  filepath)
            elif end_of_file_re.search(file_contents) is None:
                test_info.warning("No newline at end of file %s" %
                                  filepath)
            else:
                test_info.info("File %s valid" % filepath)

        self.test_details_txt(test_info)

    def load_interface(self, filepath, parent_test):
        """Load an interface binary or hex"""
        assert isinstance(filepath, str), "Invalid bootloader image!"
        assert isinstance(parent_test, TestInfo), "Invalid parent test object!"

        test_info = parent_test.create_subtest('load_interface')
        self.set_mode(self.MODE_BL, test_info)

        data_crc, crc_in_image = _compute_crc(filepath)
        assert data_crc == crc_in_image, ("CRC in interface is wrong "
                                          "expected 0x%x, found 0x%x" %
                                          (data_crc, crc_in_image))

        filename = os.path.basename(filepath)
        with open(filepath, 'rb') as firmware_file:
            data = firmware_file.read()
        out_file = self.get_file_path(filename)
        start = time.time()
        with open(out_file, 'wb') as firmware_file:
            firmware_file.write(data)
        stop = time.time()
        test_info.info("programming took %s s" % (stop - start))
        self.wait_for_remount(test_info)

        # Check the CRC
        self.set_mode(self.MODE_IF, test_info)
        if DaplinkBoard.KEY_IF_CRC not in self.details_txt:
            test_info.failure("No interface CRC in details.txt")
            return
        details_crc = int(self.details_txt[DaplinkBoard.KEY_IF_CRC], 0)
        test_info.info("Interface crc: 0x%x" % details_crc)
        if data_crc != details_crc:
            test_info.failure("Interface CRC is wrong")

    def load_bootloader(self, filepath, parent_test):
        """Load a bootloader binary or hex"""
        assert isinstance(filepath, str), "Invalid bootloader image!"
        assert isinstance(parent_test, TestInfo), "Invalid parent test object!"

        test_info = parent_test.create_subtest('load_bootloader')
        self.set_mode(self.MODE_IF, test_info)

        # Check image CRC
        data_crc, crc_in_image = _compute_crc(filepath)
        assert data_crc == crc_in_image, ("CRC in bootloader is wrong "
                                          "expected 0x%x, found 0x%x" %
                                          (data_crc, crc_in_image))

        filename = os.path.basename(filepath)
        with open(filepath, 'rb') as firmware_file:
            data = firmware_file.read()
        out_file = self.get_file_path(filename)
        start = time.time()
        with open(out_file, 'wb') as firmware_file:
            firmware_file.write(data)
        stop = time.time()
        test_info.info("programming took %s s" % (stop - start))
        self.wait_for_remount(test_info)

        # Check the CRC
        self.set_mode(self.MODE_IF, test_info)
        if DaplinkBoard.KEY_BL_CRC not in self.details_txt:
            test_info.failure("No bootloader CRC in details.txt")
            return
        details_crc = int(self.details_txt[DaplinkBoard.KEY_BL_CRC], 0)
        test_info.info("Bootloader crc: 0x%x" % details_crc)
        if data_crc != details_crc:
            test_info.failure("Bootloader CRC is wrong")

    def wait_for_remount(self, parent_test, wait_time=600):
        mode = self._mode
        count = self._remount_count
        test_info = parent_test.create_subtest('wait_for_remount')

        elapsed = 0
        start = time.time()
        remounted = False
        while os.path.isdir(self.mount_point):
            if self.update_board_info(False): #check info if it is already mounted
                if mode is not None and self._mode is not None and mode is not self._mode:
                    remounted = True
                    test_info.info("already remounted with change mode")
                    break
                elif count is not None and self._remount_count is not None and count != self._remount_count:
                        remounted = True
                        test_info.info("already remounted with change mount count")
                        break
            if elapsed > wait_time:
                raise Exception("Dismount timed out")
            time.sleep(0.1)
            elapsed += 0.2
        else:
            stop = time.time()
            test_info.info("unmount took %s s" % (stop - start))
        elapsed = 0
        start = time.time()

        while not remounted:
            if self.update_board_info(False):
                if os.path.isdir(self.mount_point):
                    # Information returned by mbed-ls could be old.
                    # Only break from the loop if the second call to
                    # mbed-ls returns the same mount point.
                    tmp_mount = self.mount_point
                    if self.update_board_info(False):
                        if tmp_mount == self.mount_point:
                            break
            if elapsed > wait_time:
                raise Exception("Mount timed out")
            time.sleep(0.1)
            elapsed += 0.1
        stop = time.time()
        test_info.info("mount took %s s" % (stop - start))

        if count is not None and self._remount_count is not None:
            expected_count = (0 if mode is not self._mode
                              else (count + 1) & 0xFFFFFFFF)
            if expected_count != self._remount_count:
                    test_info.failure('Expected remount count of %s got %s' %
                                      (expected_count, self._remount_count))

        # If enabled check the filesystem
        if self._check_fs_on_remount:
            self.test_fs(parent_test)
            self.test_fs_contents(parent_test)
            self.test_details_txt(parent_test)
            if self._manage_assert:
                if self._assert is not None:
                    test_info.failure('Assert on line %s in file %s' %
                                      (self._assert.line, self._assert.file))
                self.clear_assert()

    def update_board_info(self, exptn_on_fail=True):
        """Update board info

        Update all board information variables that could
        change when remounting or changing modes.
        Note - before this function is set self.unique_id
        must be set.
        """

        try:
            endpoints = _get_board_endpoints(self.unique_id)
            if endpoints is None:
                if exptn_on_fail:
                    raise Exception("Could not update board info: %s" %
                                    self.unique_id)
                return False
            self.unique_id, self.serial_port, self.mount_point = endpoints
            # Serial port can be missing
            if self.unique_id is None:
                if exptn_on_fail:
                    raise Exception("Mount point is null")
                return False
            if self.mount_point is None:
                if exptn_on_fail:
                    raise Exception("Mount point is null")
                return False
            self.board_id = int(self.unique_id[0:4], 16)
            self._hic_id = int(self.unique_id[-8:], 16)

            # Note - Some legacy boards might not have details.txt
            details_txt_path = self.get_file_path("details.txt")
            self.details_txt = _parse_kvp_file(details_txt_path)
            self._parse_assert_txt()

            self._remount_count = None
            if DaplinkBoard.KEY_REMOUNT_COUNT in self.details_txt:
                self._remount_count = int(self.details_txt[DaplinkBoard.KEY_REMOUNT_COUNT])
            self._mode = None
            if DaplinkBoard.KEY_MODE in self.details_txt:
                DETAILS_TO_MODE = {
                    "interface": DaplinkBoard.MODE_IF,
                    "bootloader": DaplinkBoard.MODE_BL,
                }
                mode_str = self.details_txt[DaplinkBoard.KEY_MODE]
                self._mode = DETAILS_TO_MODE[mode_str]
            else:
                #check for race condition here
                return False
            return True
        except Exception as e:
            if exptn_on_fail:
                raise e
            else:
                return False

    def test_details_txt(self, parent_test):
        """Check that details.txt has all requied fields"""
        test_info = parent_test.create_subtest('test_details_txt')
        required_key_and_format = {
            DaplinkBoard.KEY_UNIQUE_ID: re.compile("^[a-f0-9]{48}$"),
            DaplinkBoard.KEY_HIC_ID: re.compile("^[a-f0-9]{8}$"),
            DaplinkBoard.KEY_GIT_SHA: re.compile("^[a-f0-9]{40}$"),
            DaplinkBoard.KEY_LOCAL_MODS: re.compile("^[01]{1}$"),
            DaplinkBoard.KEY_USB_INTERFACES: re.compile("^.+$"),
            DaplinkBoard.KEY_MODE: re.compile("(interface|bootloader)"),
        }
        optional_key_and_format = {
            DaplinkBoard.KEY_BL_VERSION: re.compile("^[0-9]{4}$"),
            DaplinkBoard.KEY_IF_VERSION: re.compile("^[0-9]{4}$"),
            DaplinkBoard.KEY_BL_CRC: re.compile("^0x[a-f0-9]{8}$"),
            DaplinkBoard.KEY_IF_CRC: re.compile("^0x[a-f0-9]{8}$"),
        }
        # 1. keys and values are alphanumeric
        # 2. no duplicate keys
        # 3. format is key : value
        # 4. required keys are present
        # 5. optional keys have the expected format
        details_txt_path = self.get_file_path("details.txt")
        details_txt = _parse_kvp_file(details_txt_path, test_info)
        if not details_txt:
            test_info.failure("Could not parse details.txt")
            return

        # Check for required keys
        for key in required_key_and_format:
            if key not in details_txt:
                test_info.failure("Missing detail.txt entry: %s" % key)
                continue

            value = details_txt[key]
            pattern = required_key_and_format[key]
            if pattern.match(value) is None:
                test_info.failure("Bad format detail.txt %s: %s" %
                                  (key, value))

        # Check format of optional values
        for key in optional_key_and_format:
            if key not in details_txt:
                continue

            value = details_txt[key]
            pattern = optional_key_and_format[key]
            if pattern.match(value) is None:
                test_info.failure("Bad format detail.txt %s: %s" %
                                  (key, value))

        # Check details.txt contents
        details_unique_id = None
        details_hic_id = None
        if DaplinkBoard.KEY_UNIQUE_ID in details_txt:
            details_unique_id = details_txt[DaplinkBoard.KEY_UNIQUE_ID]
        if DaplinkBoard.KEY_HIC_ID in details_txt:
            details_hic_id = details_txt[DaplinkBoard.KEY_HIC_ID]
        if details_unique_id is not None:
            if details_unique_id != self.unique_id:
                test_info.failure("Unique ID mismatch in details.txt "
                                  "details.txt=%s, usb=%s" %
                                  (details_unique_id, self.unique_id))
            if details_hic_id is not None:
                usb_hic = details_unique_id[-8:]
                if details_hic_id != usb_hic:
                    test_info.failure("HIC ID is not the last 8 "
                                      "digits of unique ID "
                                      "details.txt=%s, usb=%s" %
                                      (details_hic_id, usb_hic))

    def _parse_assert_txt(self):
        file_path = self.get_file_path("ASSERT.TXT")
        if not os.path.isfile(file_path):
            self._assert = None
            return

        assert_table = _parse_kvp_file(file_path)
        assert "file" in assert_table
        assert "line" in assert_table

        self._assert = AssertInfo(assert_table["file"], assert_table['line'])
