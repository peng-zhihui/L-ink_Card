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
import future
import os
import binascii
import intelhex
import sys

try:
    from cStringIO import StringIO
except ImportError:
    from io import StringIO

from msd_test import (MassStorageTester, MOCK_DIR_LIST, MOCK_FILE_LIST,
                      MOCK_DIR_LIST_AFTER, MOCK_FILE_LIST_AFTER)

TRIGGER_ASSERT_FILE_NAME = "ASSERT.ACT"
ASSERT_FILE_NAME = "ASSERT.TXT"
NEED_BL_FILE_NAME = "NEED_BL.TXT"
DAPLINK_VECTOR_TABLE_OFFSET = 0x00
DAPLINK_BUILD_KEY_OFFSET = 0x20
DAPLINK_HIC_ID_OFFSET = 0x24


def intel_hex_get_sections(intel_hex):
    """Return list of address, size tuples"""
    addresses = intel_hex.addresses()
    addresses.sort()
    section_list = []
    section_start = None
    last_addr = None
    for addr in addresses:
        if section_start is None:
            section_start = addr
            last_addr = addr
        if addr == last_addr or addr == last_addr + 1:
            last_addr = addr
        else:
            section_size = last_addr - section_start + 1
            section_list.append((section_start, section_size))
            section_start = addr
            last_addr = addr
    if section_start is not None:
        section_size = last_addr - section_start + 1
        section_list.append((section_start, section_size))
    return section_list


def bin_data_to_hex_data(addr, data):
    """Covert binary data to a string in intel hex format"""
    intel_hex = intelhex.IntelHex()
    if sys.version_info >= (3,0):
        data = data.decode('latin1')
    intel_hex.puts(addr, data)
    sio = StringIO()
    intel_hex.tofile(sio, format='hex')
    hex_data = sio.getvalue()
    return bytearray(hex_data.encode('latin1'))


class DLMassStorageTester(MassStorageTester):
    """DAPLink mass storage tester"""

    def __init__(self, board, parent_test, test_name, test_mode):
        super(DLMassStorageTester, self).__init__(board, parent_test,
                                                  test_name)
        self._expected_mode = None
        self._actual_mode = None
        self._test_mode = test_mode
        if self._test_mode == board.MODE_IF:
            self._crc_tag = board.KEY_BL_CRC
        elif self._test_mode == board.MODE_BL:
            self._crc_tag = board.KEY_IF_CRC
        else:
            assert False

    def _run(self, test_info):
        assert self._expected_mode is not None
        # Set board to the correct mode before running test
        self.board.set_mode(self._test_mode)
        self._actual_mode = None

        super(DLMassStorageTester, self)._run(test_info)

        if self._actual_mode is None:
            # Set expected mode if it hasn't been set in _check_data_correct
            self._actual_mode = self.board.get_mode()
        if self._expected_mode is not self._actual_mode:
            test_info.failure("Wrong mode after test - Expected "
                              " %s got %s" % (self._expected_mode,
                                              self._actual_mode))

    def set_expected_mode(self, mode):
        self._expected_mode = mode

    def _check_data_correct(self, expected_data, test_info):
        board = self.board
        self._actual_mode = self.board.get_mode()
        board.set_mode(self._test_mode)
        if self._crc_tag not in board.details_txt:
            test_info.info("CRC not in details.txt")
            return False
        actual_crc32 = int(self.board.details_txt[self._crc_tag], 16)
        expected_crc32 = binascii.crc32(expected_data[0:-4]) & 0xFFFFFFFF
        test_info.info("Expected CRC: 0x%08x, actual crc: 0x%08x" %
                       (expected_crc32, actual_crc32))
        return actual_crc32 == expected_crc32


def daplink_test(workspace, parent_test):
    board = workspace.board
    interface = workspace.if_firmware
    test_info = parent_test.create_subtest('daplink_test')

    intel_hex = intelhex.IntelHex(interface.hex_path)
    section_list = intel_hex_get_sections(intel_hex)
    assert len(section_list) == 1, ("Only 1 section supported, found %s" %
                                    len(section_list))
    start, length = section_list[0]

    bin_data = bytearray(intel_hex.tobinarray(start=start, size=length))    
    sio = StringIO()
    intel_hex.tofile(sio, format='hex')
    hex_data = sio.getvalue()
    hex_data = bytearray(hex_data.encode('latin1'))

    # Make sure asserts work as expected
    test_assert(workspace, test_info)

    # Test loading a binary file with shutils
    test = DLMassStorageTester(board, test_info, "Shutil binary file load "
                               "interface", board.MODE_BL)
    test.set_shutils_copy(interface.bin_path)
    test.set_expected_data(bin_data, start)
    test.set_expected_mode(board.MODE_IF)
    test.run()

    # Test loading a hex file with shutils
    test = DLMassStorageTester(board, test_info, "Shutil hex file load "
                               "interface", board.MODE_BL)
    test.set_shutils_copy(interface.hex_path)
    test.set_expected_data(bin_data, start)
    test.set_expected_mode(board.MODE_IF)
    test.run()

    test_file_type('bin', board.MODE_BL, board, test_info, start, bin_data)
    test_file_type('hex', board.MODE_BL, board, test_info, start, bin_data)

    # Test bootloader updates
    firmware = workspace.bl_firmware
    intel_hex = intelhex.IntelHex(firmware.hex_path)
    section_list = intel_hex_get_sections(intel_hex)
    assert len(section_list) == 1, ("Only 1 section supported, found %s" %
                                    len(section_list))
    start, length = section_list[0]

    bin_data = bytearray(intel_hex.tobinarray(start=start, size=length))
    sio = StringIO()
    intel_hex.tofile(sio, format='hex')
    hex_data = sio.getvalue()
    hex_data = bytearray(hex_data.encode('latin1'))

    # Test loading a binary file with shutils
    test = DLMassStorageTester(board, test_info, "Shutil binary file load "
                               "bootloader", board.MODE_IF)
    test.set_shutils_copy(firmware.bin_path)
    test.set_expected_data(bin_data, start)
    test.set_expected_mode(board.MODE_IF)
    test.run()

    # Test loading a hex file with shutils
    test = DLMassStorageTester(board, test_info, "Shutil hex file load "
                               "bootloader", board.MODE_IF)
    test.set_shutils_copy(firmware.hex_path)
    test.set_expected_data(bin_data, start)
    test.set_expected_mode(board.MODE_IF)
    test.run()

    test_file_type('bin', board.MODE_IF, board, test_info, start, bin_data)
    test_file_type('hex', board.MODE_IF, board, test_info, start, bin_data)


def test_assert(workspace, parent_test):
    """Test that asserts work and persist across modes"""
    board = workspace.board
    test_info = parent_test.create_subtest('Assert Test')

    board.set_assert_auto_manage(False)

    board.set_mode(board.MODE_IF)

    # Create a test assert file
    test_info.info('Triggering assert by creating %s' %
                   TRIGGER_ASSERT_FILE_NAME)
    trigger_assert_path = board.get_file_path(TRIGGER_ASSERT_FILE_NAME)
    with open(trigger_assert_path, 'wb') as _:
        pass
    board.wait_for_remount(test_info)

    test_info.info('Checking that assert file was created')
    board.set_mode(board.MODE_IF)
    assert_path = board.get_file_path(ASSERT_FILE_NAME)
    if not os.path.exists(assert_path):
        test_info.failure('Assert file not created')

    test_info.info('Checking that assert file persists if->bl')
    board.set_mode(board.MODE_BL)
    assert_path = board.get_file_path(ASSERT_FILE_NAME)
    if not os.path.exists(assert_path):
        test_info.failure('Assert file not created')

    test_info.info('Checking that assert file persists bl->if')
    board.set_mode(board.MODE_IF)
    assert_path = board.get_file_path(ASSERT_FILE_NAME)
    if not os.path.exists(assert_path):
        test_info.failure('Assert file not created')

    board.clear_assert()

    board.set_mode(board.MODE_IF)
    assert_path = board.get_file_path(ASSERT_FILE_NAME)
    if os.path.exists(assert_path):
        test_info.failure('Assert file not cleared correctly')

    board.set_mode(board.MODE_BL)
    assert_path = board.get_file_path(ASSERT_FILE_NAME)
    if os.path.exists(assert_path):
        test_info.failure('Assert file not cleared correctly')

    board.set_mode(board.MODE_IF)
    assert_path = board.get_file_path(ASSERT_FILE_NAME)
    if os.path.exists(assert_path):
        test_info.failure('Assert file not cleared correctly')

    board.set_assert_auto_manage(True)


def test_file_type(file_type, board_mode, board, parent_test,
                   data_start, raw_data):
    """Test updates of a given file type using the given mode"""
    assert file_type in ('hex', 'bin'), 'Unsupported file type %s' % file_type

    if board_mode == board.MODE_IF:
        data_type = board.MODE_BL
    elif board_mode == board.MODE_BL:
        data_type = board.MODE_IF
    else:
        assert False

    test_info = parent_test.create_subtest('%s %s filetype test' %
                                           (file_type, data_type))

    def get_file_name(base='image'):
        """Get the file name to be used for loading"""
        return base + '.' + file_type

    def get_file_content(addr, bin_data):
        """Get the file contents to be used for loading"""
        if file_type == 'bin':
            return bytearray(bin_data)  # Make a copy
        elif file_type == 'hex':
            return bin_data_to_hex_data(addr, bin_data)

    # Test partial update
    file_name = get_file_name()
    local_data = get_file_content(data_start, raw_data[0:len(raw_data) // 2])
    test = DLMassStorageTester(board, test_info, "Load partial",
                               board_mode)
    test.set_programming_data(local_data, file_name)
    test.set_expected_data(None)
    test.set_expected_failure_msg("In application programming failed because "
                                  "the update sent was incomplete.", "interface")
    test.set_expected_mode(board_mode)
    test.run()
    # If bootloader is missing then this should be indicated by a file
    if board_mode == board.MODE_IF:
        if not os.path.isfile(board.get_file_path(NEED_BL_FILE_NAME)):
            test_info.failure("Bootloader missing but file %s not present" %
                              NEED_BL_FILE_NAME)
        test_info.info("Testing switch to bootloader")
        try:
            board.set_mode(board.MODE_BL)
            test_info.failure("Board switched to bootloader mode")
        except Exception:
            pass
        finally:
            if board.get_mode() == board.MODE_IF:
                test_info.info("Device able to recover from bad BL")
            else:
                test_info.failure("Device in wrong mode")

    # Test loading a normal image
    file_name = get_file_name()
    local_data = get_file_content(data_start, raw_data)
    test = DLMassStorageTester(board, test_info, "Normal Load",
                               board_mode)
    test.set_programming_data(local_data, file_name)
    test.set_expected_data(raw_data)
    test.set_expected_mode(board.MODE_IF)
    test.run()

    # Wrong starting address
    if file_type != 'bin':
        mode_to_error = {
            board.MODE_IF: ('The starting address for the bootloader '
                            'update is wrong.'),
            board.MODE_BL: ('The starting address for the interface '
                            'update is wrong.')
        }
        file_name = get_file_name()
        local_data = get_file_content(data_start + 0x400, raw_data)
        test = DLMassStorageTester(board, test_info, "Wrong Address",
                                   board_mode)
        test.set_expected_failure_msg(mode_to_error[board_mode], 'user')
        test.set_programming_data(local_data, file_name)
        test.set_expected_data(raw_data)
        test.set_expected_mode(board_mode)
        test.run()

    # Test flushes during update
    file_name = get_file_name()
    local_data = get_file_content(data_start, raw_data)
    test = DLMassStorageTester(board, test_info, "Load with flushes",
                               board_mode)
    test.set_programming_data(local_data, file_name)
    test.set_expected_data(raw_data)
    test.set_flush_size(0x1000)
    test.set_expected_mode(board.MODE_IF)
    test.run()

    # Test bad crc
    file_name = get_file_name()
    local_raw_data = bytearray(raw_data)
    local_raw_data[-1] = (local_raw_data[-1] + 1) % 0x100  # Corrupt CRC
    local_data = get_file_content(data_start, local_raw_data)
    test = DLMassStorageTester(board, test_info, 'Wrong CRC',
                               board_mode)
    test.set_programming_data(local_data, file_name)
    if board_mode == board.MODE_IF:
        test.set_expected_failure_msg('The bootloader CRC did not pass.', 'interface')
        test.set_expected_data(None)
    elif board_mode == board.MODE_BL:
        # Interface images can be from other vendors and be missing
        # the crc, so don't treat this as an error
        test.set_expected_data(local_raw_data)
    test.set_expected_mode(board.MODE_IF)
    test.run()
    # If bootloader is missing then this should be indicated by a file
    if (board_mode == board.MODE_IF and
            not os.path.isfile(board.get_file_path(NEED_BL_FILE_NAME))):
        test_info.failure("Bootloader missing but file %s not present" %
                          NEED_BL_FILE_NAME)

    # Test load with extra padding
    file_name = get_file_name()
    local_data = get_file_content(data_start, raw_data)
    local_data.extend(b'\xFF' * 0x1000)
    test = DLMassStorageTester(board, test_info, "Padded load", board_mode)
    test.set_programming_data(local_data, file_name)
    test.set_expected_data(raw_data)
    test.set_expected_mode(board.MODE_IF)
    test.run()

    # Test bad crc in file data
    # Note - crc is only a requirment for loading bootloades
    if board_mode == board.MODE_IF:
        file_name = get_file_name()
        local_raw_data = bytearray(raw_data)
        local_raw_data[0x100] = (local_raw_data[0x100] + 1) % 0x100  # Corrupt CRC
        local_data = get_file_content(data_start, local_raw_data)
        test = DLMassStorageTester(board, test_info, 'Wrong data CRC',
                                   board_mode)
        test.set_programming_data(local_data, file_name)
        test.set_expected_failure_msg('The bootloader CRC did not pass.', 'interface')
        test.set_expected_data(None)
        test.set_expected_mode(board.MODE_IF)
        test.run()
        # If bootloader is missing then this should be indicated by a file
        if not os.path.isfile(board.get_file_path(NEED_BL_FILE_NAME)):
            test_info.failure("Bootloader missing but file %s not present" %
                              NEED_BL_FILE_NAME)

        # Restore a good image
        file_name = get_file_name()
        local_data = get_file_content(data_start, raw_data)
        test = DLMassStorageTester(board, test_info, "Normal Load",
                                   board_mode)
        test.set_programming_data(local_data, file_name)
        test.set_expected_data(raw_data)
        test.set_expected_mode(board.MODE_IF)
        test.run()

    # Test wrong HIC ID
    # Bootloader should perform interface update regardless of key
    if data_type == board.MODE_IF:
        local_raw_data = bytearray(raw_data)
        local_raw_data[DAPLINK_HIC_ID_OFFSET] = \
            (local_raw_data[DAPLINK_HIC_ID_OFFSET] + 1) % 0x100
        file_name = get_file_name()
        local_data = get_file_content(data_start, local_raw_data)
        test = DLMassStorageTester(board, test_info, "Wrong HIC ID",
                                   board_mode)
        test.set_programming_data(local_data, file_name)
        test.set_expected_data(local_raw_data)
        test.set_expected_mode(board.MODE_IF)
        test.run()

    # TODO future - Wrong type

    # Test a normal load with dummy files created beforehand
    file_name = get_file_name()
    local_data = get_file_content(data_start, raw_data)
    test = DLMassStorageTester(board, test_info, "Extra Files", board_mode)
    test.set_programming_data(local_data, file_name)
    test.add_mock_dirs(MOCK_DIR_LIST)
    test.add_mock_files(MOCK_FILE_LIST)
    test.add_mock_dirs_after_load(MOCK_DIR_LIST_AFTER)
    test.add_mock_files_after_load(MOCK_FILE_LIST_AFTER)
    test.set_expected_data(raw_data)
    test.set_expected_mode(board.MODE_IF)
    test.run()

    # Restore good image
    file_name = get_file_name()
    local_data = get_file_content(data_start, raw_data)
    test = DLMassStorageTester(board, test_info, "Restore image",
                               board_mode)
    test.set_programming_data(local_data, file_name)
    test.set_expected_data(raw_data)
    test.set_expected_mode(board.MODE_IF)
    test.run()

