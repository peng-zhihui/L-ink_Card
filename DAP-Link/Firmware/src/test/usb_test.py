#
# DAPLink Interface Firmware
# Copyright (c) 2016-2016, ARM Limited, All Rights Reserved
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
""" Module for low level and targeted USB tests"""

from __future__ import print_function

import os
import usb.core
import functools
import threading
import time
from usb_cdc import USBCdc
from usb_hid import USBHid
from usb_msd import USBMsd, Fat

DISMOUNT_TIME_S = 10.00


def test_usb(workspace, parent_test, force=False):
    """Run raw USB tests

    Requirements:
        -daplink-validation must be loaded for the target.
    """

    # Only test on supported platforms
    if not _platform_supports_usb_test() and not force:
        parent_test.info("Skipping USB test on this platform")
        return
    test_info = parent_test.create_subtest("USB test")

    # Find the device under test
    serial_number = workspace.board.get_unique_id()
    dev = _daplink_from_serial_number(serial_number)
    if dev is None:
        test_info.failure("Could not find board with serial number %s" %
                          serial_number)
        return

    # Create wrappers for and acquire exclusive access to interfaces
    cdc = USBCdc(dev)
    hid = USBHid(dev)
    msd = USBMsd(dev)
    cdc.lock()
    hid.lock()
    msd.lock()

    try:

        for usb_test_on in (False, True):

            _set_usb_test_mode(hid, usb_test_on)

            test_cdc(test_info, cdc)

            test_hid(test_info, hid)

            test_msd(test_info, msd)

            test_msd_stall(test_info, msd)

            test_control(test_info, dev, cdc, hid, msd)

            test_all(test_info, cdc, hid, msd)

            # TODO - future enhancements
            #  MSD remount + hid
            #  STALL IN and STALL OUT

    finally:
        try:
            _set_usb_test_mode(hid, False)
        except usb.core.USBError:
            pass
        cdc.unlock()
        hid.unlock()
        msd.unlock()


def main():
    """Run the usb test as a stand alone program"""

    import test_info
    import mock

    def get_unique_id(unique_id):
        """Mock function to return a unique id"""
        return unique_id

    dev_list = usb.core.find(find_all=True, custom_match=_daplink_match)
    for dev in dev_list:
        board_id = dev.serial_number
        print("Testing board %s" % board_id)
        print("----------------")
        mock_ws = mock.Mock()
        mock_ws.board = mock.Mock()
        mock_ws.board.unique_id = dev.serial_number
        mock_ws.board.get_unique_id = functools.partial(get_unique_id,
                                                        board_id)
        test_usb(mock_ws, test_info.TestInfoStub(), True)


def test_cdc(test_info, cdc):
    """Smoke test of the CDC endpoint"""
    cdc.set_line_coding(115200)
    baud, fmt, parity, databits = cdc.get_line_coding()
    test_info.info("Baud %i, fmt %i, parity %i, databits %i" %
                   (baud, fmt, parity, databits))
    cdc.send_break(cdc.SEND_BREAK_ON)
    cdc.send_break(cdc.SEND_BREAK_OFF)
    data = cdc.read(1024)
    test_info.info("Serial port data: %s" % bytearray(data))
    cdc.write("Hello world")
    data = cdc.read(1024)
    test_info.info("Serial port data2: %s" % bytearray(data))


def test_hid(test_info, hid):
    """Smoke test of the HID endpoint"""
    hid.set_idle()
    report = hid.get_descriptor(hid.DESC_TYPE_REPORT, 0)
    test_info.info("Report descriptor: %s" % report)
    # Send CMSIS-DAP vendor command to get the serial number
    data = bytearray(64)
    data[0] = 0x80
    hid.set_report(data)
    resp = hid.get_report(64)
    length = resp[1]
    test_info.info("CMSIS-DAP response: %s" %
                   bytearray(resp[1:1 + length]).decode("utf-8"))


def test_msd(test_info, msd):
    """MSD endpoint tests"""

    # Simple read
    mbr = msd.scsi_read10(0, 1)
    test_info.info("MBR[0:16]: %s" % mbr[0:16])

    # Test FAT filesystem
    fat = Fat(msd)
    print(fat.mbr)

    # Grab entries in the root directory
    root_dir = fat.root_dir
    for entry in root_dir:
        if entry["DIR_Name"][0] != "\0":
            print(entry)

    # Trigger a remount
    dir_idx = root_dir.find_free_entry_index()
    root_dir[dir_idx]["DIR_Name"] = "REFRESH ACT"
    root_dir_data = root_dir.pack()
    msd.scsi_write10(root_dir.sector, root_dir_data)

    test_info.info("Added file to root directory")
    start = time.time()
    while time.time() - start < DISMOUNT_TIME_S:
        try:
            msd.scsi_read10(0, 1)
        except msd.SCSIError:
            test_info.info("Dismount detected")
            break
    else:
        test_info.failure("Device failed to dismount")

    start = time.time()
    while time.time() - start < DISMOUNT_TIME_S:
        try:
            msd.scsi_read10(0, 1)
            test_info.info("Mount detected")
            break
        except msd.SCSIError:
            pass
    else:
        test_info.failure("Device failed to mount")


def test_msd_stall(test_info, msd):
    """Test stalls coming at various times in the middle of MSD xfers"""
    fat = Fat(msd)
    root_dir = fat.root_dir
    dir_idx = root_dir.find_free_entry_index()
    root_dir[dir_idx]["DIR_Name"] = "REFRESH ACT"
    root_dir_data = root_dir.pack()

    # Test that a write fails if media is removed after the CBW
    # stage but before the CSW stage
    msd.scsi_write10(root_dir.sector, root_dir_data)
    msd.delay_cbw_to_data = 1.0
    retval = msd.CSW_STATUS_PASSED
    try:
        msd.scsi_write10(0, bytearray(512))
        test_info.failure("Device failed to stall data stage")
    except msd.SCSIError as error:
        retval = error.value
    msd.delay_cbw_to_data = 0
    # Make sure device still works as expected
    time.sleep(3)
    msd.scsi_read10(0, 1)
    msd.scsi_write10(0, bytearray(512))
    if retval == msd.CSW_STATUS_FAILED:
        test_info.info("Test CBW,Stall,Data OUT - Pass")
    else:
        test_info.failure("Device returned wrong status")

    # Test that a write succeeds even if media is removed
    # after the OUT stage but before the CSW stage
    msd.scsi_write10(root_dir.sector, root_dir_data)
    msd.delay_data_to_csw = 1.0
    msd.scsi_write10(0, bytearray(512))
    msd.delay_data_to_csw = 0
    # Make sure device still works as expected
    time.sleep(3)
    msd.scsi_read10(0, 1)
    msd.scsi_write10(0, bytearray(512))
    test_info.info("Test DATA OUT,Stall,CSW - Pass")

    # Test that a read succeeds even if media is removed
    # after the IN stage but before the CSW stage
    msd.scsi_write10(root_dir.sector, root_dir_data)
    msd.delay_data_to_csw = 1.0
    resp = msd.scsi_read10(0, 1)
    assert len(resp) == 512
    msd.delay_data_to_csw = 0
    # Make sure device still works as expected
    time.sleep(3)
    msd.scsi_read10(0, 1)
    msd.scsi_write10(0, bytearray(512))
    test_info.info("Test DATA IN,Stall,CSW - Pass")

    # Test that a test unit ready succeeds even if media is removed
    # after the CBW stage but before the CSW stage
    msd.scsi_write10(root_dir.sector, root_dir_data)
    msd.delay_data_to_csw = 1.0
    resp = msd.scsi_test_unit_ready()
    msd.delay_data_to_csw = 0
    # Make sure device still works as expected
    time.sleep(3)
    msd.scsi_read10(0, 1)
    msd.scsi_write10(0, bytearray(512))
    if resp == msd.CSW_STATUS_PASSED:
        test_info.info("Test CBW,Stall,CSW - Pass")
    else:
        test_info.failure("Test CBW,Stall,CSW - Failed")

    # Test that a test unit ready succeeds even if media is removed
    # after the CBW stage but before the CSW stage
    msd.scsi_write10(root_dir.sector, root_dir_data)
    time.sleep(1.0)
    resp = msd.scsi_test_unit_ready()
    # Make sure device still works as expected
    time.sleep(3)
    msd.scsi_read10(0, 1)
    msd.scsi_write10(0, bytearray(512))
    if resp == msd.CSW_STATUS_FAILED:
        test_info.info("Test CBW,Stall,CSW - Pass")
    else:
        test_info.failure("Test CBW,Stall,CSW - Failed")


def test_control(test_info, dev, cdc, hid, msd):
    """Test for the control endpoint"""

    test_info.info("testing control transfer with size a multiple of 256")
    request_type = 0x80
    request = 0x06            # Get descriptor
    value = 0x200              # Configuration descriptor
    index = 0                  # Always 0 for this request
    resp = dev.ctrl_transfer(request_type, request, value, index, 256)
    assert len(resp) > 0

    test_info.info("testing control commands")
    # Test various patterns of control transfers
    #
    # Some devices have had problems with back-to-back
    # control transfers. Intentionally send these sequences
    # to make sure they are properly handled.
    for _ in range(100):
        # Control transfer with a data in stage
        cdc.get_line_coding()
    for _ in range(100):
        # Control transfer with a data out stage followed
        # by a control transfer with a data in stage
        cdc.set_line_coding(115200)
        cdc.get_line_coding()
    for _ in range(100):
        # Control transfer with a data out stage
        cdc.set_line_coding(115200)

    test_info.info("testing endpoint clearing")

    cdc.ep_data_out.clear_halt()
    cdc.ep_data_out.write('')      # DATA0
    cdc.ep_data_out.clear_halt()
    cdc.ep_data_out.write('')      # DATA0

    cdc.ep_data_out.clear_halt()
    cdc.ep_data_out.write('')      # DATA 0
    cdc.ep_data_out.write('')      # DATA 1
    cdc.ep_data_out.clear_halt()
    cdc.ep_data_out.write('')      # DATA 0


def test_all(test_info, cdc, hid, msd):
    """Test all endpoints in parallel"""
    mutex = threading.RLock()
    terminate = False
    error_msg_list = []

    def _safe_print(message):
        """Thread safe wrapper to print messages"""
        with mutex:
            print(message)

    def _test_msd():
        """MSD thread entry point for parallel testing"""
        try:
            _safe_print("msd started")
            msd_data = msd.scsi_read10(100, 1)
            while not terminate:
                #msd_data = 'x' * 1024 * 16  # 16KB
                msd.scsi_write10(100, msd_data)
            _safe_print("msd end")
        except:
            error_msg_list.append("MSD test failed")
            raise

    def _test_cdc():
        """CDC thread entry point for parallel testing"""
        try:
            _safe_print("cdc started")
            while not terminate:
                cdc.set_line_coding(115200)
                cdc.get_line_coding()
                cdc.send_break(cdc.SEND_BREAK_ON)
                cdc.send_break(cdc.SEND_BREAK_OFF)
                cdc.read(1024)
                cdc.write("Hello world")
                cdc.read(1024)
            _safe_print("cdc end")
        except:
            error_msg_list.append("CDC test failed")
            raise

    def _test_hid():
        """HID thread entry point for parallel testing"""
        try:
            _safe_print("hid started")
            data = bytearray(64)
            data[0] = 0x80
            while not terminate:
                hid.set_report(data)
                resp = hid.get_report(64)
                assert resp[0] == 0x80
            _safe_print("hid end")
        except:
            error_msg_list.append("HID test failed")
            raise

    thread_list = []
    for function in (_test_msd, _test_cdc, _test_hid):
        thread = threading.Thread(target=function)
        thread.start()
        thread_list.append(thread)

    time.sleep(10)

    terminate = True
    for thread in thread_list:
        thread.join()

    for error in error_msg_list:
        test_info.failure(error)


def _daplink_match(dev):
    """DAPLink match function to be used with usb.core.find"""
    try:
        device_string = dev.product
    except ValueError:
        return False
    if device_string is None:
        return False
    if device_string.find("CMSIS-DAP") < 0:
        return False
    return True


def _daplink_from_serial_number(serial_number):
    """Return a usb handle to the DAPLink device with the serial number"""
    dev_list = usb.core.find(find_all=True, custom_match=_daplink_match)
    for dev in dev_list:
        if dev.serial_number == serial_number:
            return dev
    return None


def _platform_supports_usb_test():
    """Return True if this platform supports USB testing, False otherwise"""
    if os.name != "posix":
        return False
    if not hasattr(os, 'uname'):
        if False:
            # Hack to supress warnings for uname not existing
            os.uname = lambda: [None]
        return False
    if os.uname()[0] == "Darwin":
        return False
    return True


def _set_usb_test_mode(hid, enabled):
    """Set to True to enable USB testing mode"""
    data = bytearray(64)
    data[0] = 0x88
    data[1] = 1 if enabled else 0
    hid.set_report(data)
    resp = hid.get_report(64)
    if (resp[0] != 0x88) or (resp[1] != 1):
        raise Exception("Error configuring USB test mode")


if __name__ == "__main__":
    main()
