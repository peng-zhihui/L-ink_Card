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
from __future__ import division
import os
import time
import shutil
import six
import info
import intelhex
from test_info import TestInfo

from pyocd.core.helpers import ConnectHelper
from pyocd.core.memory_map import MemoryType

def _same(d1, d2):
    assert type(d1) is bytearray
    assert type(d2) is bytearray
    for i in range(min(len(d1), len(d2))):
        if d1[i] != d2[i]:
            return False
    if len(d1) != len(d2):
        return False
    return True

MOCK_DIR_LIST = [
    "test",
    "blarg",
    "very_long_directory_name",
    "very_long_directory_name/and_subdirectory_name"
]

MOCK_FILE_LIST = [
    (".test", "blarg"),
    ("test/file1", "asdofahweaw"),
    ("file.jpg", "file contents here")
]

MOCK_DIR_LIST_AFTER = [
    "test2",
    "blarg2",
    "very_long_directory_name2",
    "very_long_directory_name2/and_subdirectory_name"
]

MOCK_FILE_LIST_AFTER = [
    (".test2", "blarg"),
    ("test2/file12", "asdofahweaw"),
    ("file2.jpg", "file contents here")
]

class MassStorageTester(object):

    RETRY_COUNT = 5
    DELAY_BEFORE_RETRY_S = 30

    def __init__(self, board, parent_test, test_name):
        self.board = board
        self.parent_test = parent_test
        self.test_name = test_name
        self._expected_failure_msg = None
        self._flush_time = 0.1
        self._load_with_shutils = None
        self._flush_size = None
        self._programming_data = None
        self._mock_file_list = []
        self._mock_dir_list = []
        self._mock_file_list_after = []
        self._mock_dir_list_after = []
        self._programming_file_name = None
        self._start = 0

    def set_shutils_copy(self, source_file_name):
        """
        Change mode to shutil file copy

        This option cannot be used with set_programming_data.
        """
        assert type(source_file_name) is str
        assert self._load_with_shutils is None
        self._source_file_name = source_file_name
        self._load_with_shutils = True

    def set_programming_data(self, data, file_name):
        """
        Set data to program over mass storage

        Data should be the conetents of the hex or binary file
        being loaded. This option cannot be used with set_shutils_copy.
        """
        assert type(data) is bytearray
        assert type(file_name) is str
        assert(self._load_with_shutils is False or
               self._load_with_shutils is None)
        self._load_with_shutils = False
        self._programming_data = data
        self._programming_file_name = file_name

    def set_flush_size(self, size):
        """Set the block size to simulate a flush of"""
        assert isinstance(size, six.integer_types)
        self._flush_size = size

    def set_expected_data(self, data, start=0):
        """Data that should have been written to the device"""
        assert data is None or type(data) is bytearray
        self._expected_data = data
        self._start = start

    def set_expected_failure_msg(self, msg, error_type):
        """Set the expected failure message as a string"""
        assert msg is None or type(msg) is str
        self._expected_failure_msg = msg
        self._expected_failure_type = error_type

    def add_mock_files(self, file_list):
        """Add a list of tuples containing a file and contents"""
        self._mock_file_list.extend(file_list)

    def add_mock_dirs(self, dir_list):
        """Add a list of directoies"""
        self._mock_dir_list.extend(dir_list)

    def add_mock_files_after_load(self, file_list):
        """Add a list of tuples containing a file and contents"""
        self._mock_file_list_after.extend(file_list)

    def add_mock_dirs_after_load(self, dir_list):
        """Add a list of directoies"""
        self._mock_dir_list_after.extend(dir_list)

    def _check_data_correct(self, expected_data, _):
        """Return True if the actual data written matches the expected"""
        data_len = len(expected_data)
        data_loaded = self.board.read_target_memory(self._start, data_len)
        return _same(expected_data, data_loaded)

    def run(self):
        for retry_count in range(self.RETRY_COUNT):
            test_info = TestInfo(self.test_name)
            if retry_count > 0:
                test_info.info('Previous attempts %s' % retry_count)
            try:
                self._run(test_info)
            except IOError:
                time.sleep(self.DELAY_BEFORE_RETRY_S)
                # Update board info since a remount could have occurred
                self.board.update_board_info()
                continue
            self.parent_test.attach_subtest(test_info)
            break
        else:
            raise Exception("Flashing failed after %i retries" %
                            self.RETRY_COUNT)

    def _run(self, test_info):
        # Expected data must be set, even if to None
        assert hasattr(self, '_expected_data')

        # Windows 8/10 workaround
        # ----
        # By default Windows 8 and 10 access and write to removable drives
        # shortly after they are connected. If this occurs at the same time
        # as a file copy the file could be sent out of order causing DAPLink
        # programming to terminate early and report an error.
        #
        # This causes testing to intermittently fail with errors such as:
        # - "The transfer timed out."
        # - "File sent out of order by PC. Target might
        #     not be programmed correctly."
        #
        # To prevent Windows from writing to removable drives on connect
        # drive indexing can be turned off with the following procedure:
        # - Start the program "gpedit.msc"
        # - Navigate to "Computer Configuration \ Administrative Templates
        #                \ Windows Components \ Search"
        # - Enable the policy "Do not allow locations on removable drives
        #                      to be added to  libraries."
        #
        # Rather than requiring all testers of DAPLink make this setting
        # change the below sleep has been added. This added delay allows
        # windows to complete the writes it performs shortly after connect.
        # This allows testing to be performed without interruption.
        #
        # Note - if drive indexing is turned off as mentioned above then
        #        this sleep is not needed.
        time.sleep(2)

        # Copy mock files before test
        self._mock_file_list = []
        for dir_name in self._mock_dir_list:
            dir_path = self.board.get_file_path(dir_name)
            os.mkdir(dir_path)
        for file_name, file_contents in self._mock_file_list:
            file_path = self.board.get_file_path(file_name)
            with open(file_path, 'wb') as file_handle:
                file_handle.write(file_contents)

        programming_file_name = None
        if self._programming_file_name is not None:
            programming_file_name = \
                self.board.get_file_path(self._programming_file_name)

        # Write data to the file
        start = time.time()
        if self._load_with_shutils:
            # Copy with shutils
            shutil.copy(self._source_file_name, self.board.get_mount_point())
        elif self._flush_size is not None:
            # Simulate flushes during the file transfer
            # Note - The file is explicitly opened and closed to more
            #        consistently simulate the undesirable behavior flush can
            #        cause.  On Windows flushing a file causes the data to be
            #        written out immediately, but only sometimes causes the
            #        filesize to get updated.
            size = len(self._programming_data)
            for addr in range(0, size, self._flush_size):
                data = self._programming_data[addr:addr + self._flush_size]
                with open(programming_file_name, 'ab') as file_handle:
                    file_handle.write(data)
                time.sleep(self._flush_time)
        else:
            # Perform a normal copy
            with open(programming_file_name, 'wb') as load_file:
                load_file.write(self._programming_data)
        stop = time.time()
        diff = stop - start
        test_info.info('Loading took %ss' % diff)
        if self._expected_data is not None:
            test_info.info('Programming rate %sB/s' %
                           (len(self._expected_data) / diff))
        if self._programming_data is not None:
            test_info.info('MSD transfer rate %sB/s' %
                           (len(self._programming_data) / diff))

        # Copy mock files after loading
        self._mock_file_list = []
        for dir_name in self._mock_dir_list_after:
            dir_path = self.board.get_file_path(dir_name)
            os.mkdir(dir_path)
        for file_name, file_contents in self._mock_file_list_after:
            file_path = self.board.get_file_path(file_name)
            with open(file_path, 'w') as file_handle:
                file_handle.write(file_contents)

        self.board.wait_for_remount(test_info)

        # Verify the disk is still valid
        self.board.test_fs(test_info)

        # Check various failure cases
        msg, error_type = self.board.get_failure_message_and_type()
        failure_expected = self._expected_failure_msg is not None
        failure_occured = msg is not None
        if failure_occured and not failure_expected:
            test_info.failure('Device reported failure: "%s"' % msg.strip())
            return
        if failure_expected and not failure_occured:
            test_info.failure('Failure expected but did not occur')
            return
        if failure_expected and failure_occured:
            if msg == self._expected_failure_msg and error_type == self._expected_failure_type:
                test_info.info(
                    'Failure as expected: "%s, %s"' %
                    (msg.strip(), error_type.strip()))
            elif msg != self._expected_failure_msg:
                test_info.failure('Failure but wrong string: "%s" vs "%s"' %
                                  (msg.strip(),
                                   self._expected_failure_msg.strip()))
            else:
                test_info.failure(
                    'Failure but wrong type: "%s" vs "%s"' %
                    (error_type.strip(), self._expected_failure_type.strip()))
            return

        # These cases should have been handled above
        assert not failure_expected
        assert not failure_occured

        # If there is expected data then compare
        if self._expected_data:
            if self._check_data_correct(self._expected_data, test_info):
                test_info.info("Data matches")
            else:
                test_info.failure('Data does not match')


def test_mass_storage(workspace, parent_test):
    """Test the mass storage endpoint

    Requirements:
        None

    Positional arguments:
        filename - A string containing the name of the file to load

    Return:
        True if the test passed, False otherwise
    """
    test_info = parent_test.create_subtest('test_mass_storage')

    # Setup test
    board = workspace.board
    target = workspace.target
    bin_file = target.bin_path
    hex_file = target.hex_path
    with open(bin_file, 'rb') as test_file:
        bin_file_contents = bytearray(test_file.read())
    with open(hex_file, 'rb') as test_file:
        hex_file_contents = bytearray(test_file.read())
    blank_bin_contents = bytearray([0xff]) * 0x2000
    vectors_and_pad = bin_file_contents[0:32] + blank_bin_contents
    locked_when_erased = board.get_board_id() in info.BOARD_ID_LOCKED_WHEN_ERASED
    page_erase_supported = board.get_board_id() in info.BOARD_ID_SUPPORTING_PAGE_ERASE
    bad_vector_table = target.name in info.TARGET_WITH_BAD_VECTOR_TABLE_LIST

    intel_hex = intelhex.IntelHex(hex_file)
    addresses = intel_hex.addresses()
    addresses.sort()
    start = addresses[0]

    # Test loading a binary file with shutils
    if not bad_vector_table:
        test = MassStorageTester(board, test_info, "Shutil binary file load")
        test.set_shutils_copy(bin_file)
        test.set_expected_data(bin_file_contents, start)
        test.run()

    # Test loading a binary file with flushes
    if not bad_vector_table:
        test = MassStorageTester(board, test_info, "Load binary with flushes")
        test.set_programming_data(bin_file_contents, 'image.bin')
        test.set_expected_data(bin_file_contents, start)
        test.set_flush_size(0x1000)
        test.run()

    # Test loading a hex file with shutils
    test = MassStorageTester(board, test_info, "Shutil hex file load")
    test.set_shutils_copy(hex_file)
    test.set_expected_data(bin_file_contents, start)
    test.run()

    # Test loading a hex file with flushes
    test = MassStorageTester(board, test_info, "Load hex with flushes")
    test.set_programming_data(hex_file_contents, 'image.hex')
    test.set_expected_data(bin_file_contents, start)
    test.set_flush_size(0x1000)
    test.run()

    # Test loading a binary smaller than a sector
    if not bad_vector_table:
        test = MassStorageTester(board, test_info, "Load .bin smaller than sector")
        test_data_size = 0x789
        test_data = bin_file_contents[0:0 + test_data_size]
        test.set_programming_data(test_data, 'image.bin')
        test.set_expected_data(test_data, start)
        test.run()

    # Test loading a blank binary - this image should cause a timeout
    #    since it doesn't have a valid vector table
    test = MassStorageTester(board, test_info, "Load blank binary")
    test.set_programming_data(blank_bin_contents, 'image.bin')
    test.set_expected_failure_msg("The transfer timed out.", "transient, user")
    test.set_expected_data(None, start)
    test.run()

    # Test loading a blank binary with a vector table but padded with 0xFF.
    #    A blank image can lock some devices.
    if not bad_vector_table:
        test = MassStorageTester(board, test_info, "Load blank binary + vector table")
        test.set_programming_data(vectors_and_pad, 'image.bin')
        if locked_when_erased:
            test.set_expected_failure_msg("The interface firmware ABORTED programming. Image is trying to set security bits", "user")
            test.set_expected_data(None, start)
        else:
            test.set_expected_data(vectors_and_pad, start)
        test.run()

    # Test a normal load with dummy files created beforehand
    test = MassStorageTester(board, test_info, "Extra Files")
    test.set_programming_data(hex_file_contents, 'image.hex')
    test.add_mock_dirs(MOCK_DIR_LIST)
    test.add_mock_files(MOCK_FILE_LIST)
    test.add_mock_dirs_after_load(MOCK_DIR_LIST_AFTER)
    test.add_mock_files_after_load(MOCK_FILE_LIST_AFTER)
    test.set_expected_data(bin_file_contents, start)
    test.run()
    # Note - it is not unexpected for an "Extra Files" test to fail
    #        when a binary file is loaded, since there is no way to
    #        tell where the end of the file is.

    if page_erase_supported:
        # Test page erase, a.k.a. sector erase by generating iHex with discrete addresses,
        # programing the device then comparing device memory against expected content.
        test = MassStorageTester(board, test_info, "Sector Erase")
        with ConnectHelper.session_with_chosen_probe(unique_id=board.get_unique_id(), open_session=False) as session:
            memory_map = session.target.memory_map
        flash_regions = memory_map.get_regions_of_type(MemoryType.FLASH)

        max_address = intel_hex.maxaddr()
        # Create an object. We'll add the addresses of unused even blocks to it first, then unused odd blocks for each region
        ih = intelhex.IntelHex()
        # Add the content from test bin first
        expected_bin_contents = bin_file_contents
        for region_index, the_region in enumerate(flash_regions):
            if the_region.is_boot_memory is False:
                continue
            flash_start = the_region.start
            flash_length = the_region.length
            block_size = the_region.blocksize

            number_of_blocks = flash_length // block_size

            # Sanity check the regions are contiguous
            if region_index:
                assert flash_start == (flash_regions[region_index - 1].start + flash_regions[region_index - 1].length)

            if max_address >= (flash_start + flash_length):
                # This bin image crosses this region, don't modify the content, go to the next region
                continue
            elif max_address >= flash_start:
                # This bin image occupies partial region. Skip the used portion to avoid touching any security bits and pad the rest
                expected_bin_contents += bytearray([0xff]) * (flash_start + flash_length - max_address - 1)
                # Calculate the starting block after the image to avoid stumbling upon security bits
                block_start = (max_address - flash_start) // block_size + 1
            else:
                # This bin image doesn't reach this region
                expected_bin_contents += bytearray([0xff]) * flash_length
                block_start = 0
            # For all even blocks, overwrite all addresses with 0x55; for all odd blocks, overwrite all addresses with 0xAA
            for pass_number in range (2):
                if pass_number == 0:
                    modifier = 0x55
                else:
                    modifier = 0xAA
                    block_start += 1
                for block_idx in range(block_start, number_of_blocks, 2):
                    for address_to_modify in range (flash_start + block_idx * block_size, flash_start + (block_idx + 1) * block_size):
                        expected_bin_contents[address_to_modify] = modifier
                        ih[address_to_modify] = modifier
        if not os.path.exists("tmp"):
            os.makedirs("tmp")
        # Write out the modified iHex to file
        ih.tofile("tmp/interleave.hex", format='hex')
        # Load this hex file with shutils
        test.set_shutils_copy("tmp/interleave.hex")
        test.set_expected_data(expected_bin_contents, start)
        test.run()

    # Finally, load a good binary
    test = MassStorageTester(board, test_info, "Load good file to restore state")
    test.set_programming_data(hex_file_contents, 'image.hex')
    test.set_expected_data(bin_file_contents, start)
    test.run()

    # Ideas for future tests - contributions welcome
    # -Zero length file
    # -Corrupt hex file
    # -Dummy files loaded before test
    # -Very large file
    # -Any MSD regressions
    # -Security bits in hex files
    # -Hex file with data at the end **
    # -Hidden files
    # -change file extension
    # -Change size (make smaller)
    # -change starting address
