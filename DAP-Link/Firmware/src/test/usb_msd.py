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

import os
import struct
import numbers
import time
import usb.util


class USBMsd(object):
    """Wrapper class for a MSD usb device"""

    # Bulk only transport documented in
    #     "Universal Serial Bus Mass Storage Class"
    # SCSI commands documented in "SCSI Commands Reference Manual" by Seagate

    CLASS_MSD = 0x8
    # Write 10
    # Read 10
    # Test unit ready
    # Request Sense

    # dCBWSignature
    # dCBWTag
    # dCBWDataTransferLength
    # bmCBWFlags
    # bCBWLUN
    # bCBWCBLength
    FMT_CBW = "<IIIBBB"

    # dCSWSignature
    # dCSWTag
    # dCSWDataResidue
    # bCSWStatus
    FMT_CSW = "<IIIB"

    CSW_STATUS_PASSED = 0
    CSW_STATUS_FAILED = 1
    CSW_STATUS_PHASE_ERROR = 2

    class SCSIError(Exception):

        def __init__(self, error):
            Exception.__init__(self)
            self.value = error

    # Some SCSI commands
    # Value   Keil middleware define         Seagate name
    # 0x12    SCSI_INQUIRY                   INQUIRY
    # 0x23    SCSI_READ_FORMAT_CAPACITIES    Missing
    # 0x25    SCSI_READ_CAPACITY             READ CAPACITY (10)
    # 0x28    SCSI_READ10                    READ (10)
    # 0x1A    SCSI_MODE_SENSE6               MODE SENSE (6)
    # 0x00    SCSI_TEST_UNIT_READY           TEST UNIT READY
    # 0x2A    SCSI_WRITE10                   WRITE (10)
    # 0x03    SCSI_REQUEST_SENSE             REQUEST SENSE
    # 0x1E    SCSI_MEDIA_REMOVAL             Missing

    def __init__(self, device):
        self._dev = device
        self._if = None
        self.ep_in = None
        self.ep_out = None
        self._locked = False
        self._cbw_tag = 0
        self.timeout = 60 * 1000
        # delays are for testing only
        self.delay_cbw_to_data = 0
        self.delay_data_to_csw = 0

        # Find interface
        for interface in device.get_active_configuration():
            if interface.bInterfaceClass == USBMsd.CLASS_MSD:
                assert self._if is None
                self._if = interface
        assert self._if is not None

        # Find endpoints
        for endpoint in self._if:
            if endpoint.bEndpointAddress & 0x80:
                assert self.ep_in is None
                self.ep_in = endpoint
            else:
                assert self.ep_out is None
                self.ep_out = endpoint
        assert self.ep_in is not None
        assert self.ep_out is not None

    def lock(self):
        """Acquire exclisive access to MSD"""
        assert not self._locked

        num = self._if.bInterfaceNumber
        try:
            if self._dev.is_kernel_driver_active(num):
                self._dev.detach_kernel_driver(num)
        except NotImplementedError:
            pass
        except usb.core.USBError:
            pass
        usb.util.claim_interface(self._dev, num)
        self._locked = True

    def unlock(self):
        """Release exclusive access to MSD"""
        assert self._locked

        num = self._if.bInterfaceNumber
        usb.util.release_interface(self._dev, num)
        try:
            self._dev.attach_kernel_driver(num)
        except NotImplementedError:
            pass
        except usb.core.USBError:
            pass
        self._locked = False

    def scsi_read10(self, lba, block_count):
        """Send the SCSI read 10 command and return the data read"""
        block_size = 512

        cbwcb = bytearray(10)
        cbwcb[0] = 0x28
        cbwcb[2] = (lba >> (8 * 3)) & 0xFF
        cbwcb[3] = (lba >> (8 * 2)) & 0xFF
        cbwcb[4] = (lba >> (8 * 1)) & 0xFF
        cbwcb[5] = (lba >> (8 * 0)) & 0xFF
        cbwcb[7] = (block_count >> (8 * 1)) & 0xFF
        cbwcb[8] = (block_count >> (8 * 0)) & 0xFF
        ret, data = self._msd_transfer(cbwcb, 0, block_count * block_size)
        if ret != self.CSW_STATUS_PASSED:
            raise self.SCSIError(ret)
        return data

    def scsi_write10(self, lba, data):
        """Send the SCSI write 10 command"""
        block_size = 512

        assert len(data) % block_size == 0
        block_count = (len(data) + (block_size - 1)) // block_size

        cbwcb = bytearray(10)
        cbwcb[0] = 0x2A
        cbwcb[2] = (lba >> (8 * 3)) & 0xFF
        cbwcb[3] = (lba >> (8 * 2)) & 0xFF
        cbwcb[4] = (lba >> (8 * 1)) & 0xFF
        cbwcb[5] = (lba >> (8 * 0)) & 0xFF
        cbwcb[7] = (block_count >> (8 * 1)) & 0xFF
        cbwcb[8] = (block_count >> (8 * 0)) & 0xFF
        ret, _ = self._msd_transfer(cbwcb, 0, data)
        if ret != self.CSW_STATUS_PASSED:
            raise self.SCSIError(ret)

    def scsi_test_unit_ready(self):
        """Send the SCSI test unit ready command and return status"""
        cbwcb = bytearray(10)
        cbwcb[0] = 0
        ret, _ = self._msd_transfer(cbwcb, 0)
        return ret

    def _msd_transfer(self, cbwcb, lun, size_or_data=None):
        """Perform a bulk only transfer"""
        assert self._locked
        assert 1 <= len(cbwcb) <= 16

        # Increment packet tag
        transfer_tag = self._cbw_tag
        self._cbw_tag = (self._cbw_tag + 1) & 0xFFFFFFFF

        # None means data size of zero
        if size_or_data is None:
            size_or_data = 0

        in_transfer = isinstance(size_or_data, numbers.Number)
        transfer_size = (size_or_data if in_transfer else len(size_or_data))
        assert in_transfer or len(size_or_data) > 0

        # Phase - Command transport
        cbw_signature = 0x43425355
        cbw_tag = transfer_tag
        cbw_data_transfer_length = transfer_size
        cbw_flags = (1 << 7) if in_transfer else 0
        cbw_lun = lun
        cbw_length = len(cbwcb)
        params = [cbw_signature, cbw_tag, cbw_data_transfer_length,
                  cbw_flags, cbw_lun, cbw_length]
        cbw = struct.pack(self.FMT_CBW, *params)
        pad_size = 16 - len(cbwcb)
        payload = cbw + cbwcb + bytearray(pad_size)
        self.ep_out.write(payload)

        if self.delay_cbw_to_data != 0:
            time.sleep(self.delay_cbw_to_data)

        # Phase - Data Out or Data In (Optional)
        data = None
        if transfer_size > 0:
            endpoint = self.ep_in if in_transfer else self.ep_out
            try:
                if in_transfer:
                    data = self.ep_in.read(transfer_size, self.timeout)
                else:
                    self.ep_out.write(size_or_data, self.timeout)
            except usb.core.USBError:
                endpoint.clear_halt()

        if self.delay_data_to_csw != 0:
            time.sleep(self.delay_data_to_csw)

        # Phase - Status Transport
        csw = self.ep_in.read(13, self.timeout)
        csw_signature, csw_tag, csw_data_residue, csw_status = \
            struct.unpack(self.FMT_CSW, csw)
        assert csw_signature == 0x53425355
        assert csw_tag == transfer_tag
        #TODO - check residue
        return (csw_status, data)


class Struct(object):
    """Base class for a C structure"""

    def __init__(self, name, structure, data):
        field_list = [field[0] for field in structure]
        fmt_list = [field[1] for field in structure]
        format_str = "<" + "".join(fmt_list)
        struct_size = struct.calcsize(format_str)
        value_list = struct.unpack(format_str, data[:struct_size])
        value_dict = {}
        for name, value in zip(field_list, value_list):
            value_dict[name] = value
        self.name = name
        self.format_str = format_str
        self.field_list = field_list
        self.value_dict = value_dict
        self.size = struct_size

    def __getitem__(self, key):
        return self.value_dict[key]

    def __setitem__(self, key, value):
        self.value_dict[key] = value

    def __str__(self):
        desc = ""
        desc += self.name + ":" + os.linesep
        for field in self.field_list:
            value = self.value_dict[field]
            if isinstance(value, bytes):
                value = list(bytearray(value))
            desc += ("    %s=%s" + os.linesep) % (field, value)
        return desc

    def pack(self):
        """Return a byte representation of this structure"""
        value_list = []
        for field in self.field_list:
            value_list.append(self.value_dict[field])
        return struct.pack(self.format_str, *value_list)


class MBR(Struct):
    """Wrapper class for a FAT MBR"""

    STRUCTURE = (
        ("BS_jmpBoot", "3s"),
        ("BS_OEMName", "8s"),
        ("BPB_BytsPerSec", "H"),
        ("BPB_SecPerClus", "B"),
        ("BPB_RsvdSecCnt", "H"),
        ("BPB_NumFATs", "B"),
        ("BPB_RootEntCnt", "H"),
        ("BPB_TotSec16", "H"),
        ("BPB_Media", "B"),
        ("BPB_FATSz16", "H"),
        ("BPB_SecPerTrk", "H"),
        ("BPB_NumHeads", "H"),
        ("BPB_HiddSec", "L"),
        ("BPB_TotSec32", "L"),
        )

    def __init__(self, data, sector=None):
        Struct.__init__(self, "MBR", self.STRUCTURE, data)
        self.sector = sector


class DirectoryEntry(Struct):
    """Wrapper class for a FAT DirectoryEntry"""

    STRUCTURE = (
        ("DIR_Name", "11s"),
        ("DIR_Attr", "B"),
        ("DIR_NTRes", "B"),
        ("DIR_CrtTimeTenth", "B"),
        ("DIR_CrtTime", "H"),
        ("DIR_CrtDate", "H"),
        ("DIR_LstAccDate", "H"),
        ("DIR_FstClusHI", "H"),
        ("DIR_WrtTime", "H"),
        ("DIR_WrtDate", "H"),
        ("DIR_FstClusLO", "H"),
        ("DIR_FileSize", "L"),
        )

    def __init__(self, data):
        Struct.__init__(self, "DirectoryEntry", self.STRUCTURE, data)


class Directory(object):
    """Wrapper class for a FAT Directory"""

    ENTRY_SIZE = 32

    def __init__(self, entry_count, data, sector=None):
        directory_list = []
        for i in range(entry_count):
            start = i * self.ENTRY_SIZE
            dir_data = data[start:start + self.ENTRY_SIZE]
            entry = DirectoryEntry(dir_data)
            directory_list.append(entry)
        self.directory_list = directory_list
        self.sector = sector

    def __iter__(self):
        return iter(self.directory_list)

    def __getitem__(self, key):
        return self.directory_list[key]

    def find_free_entry_index(self):
        """Find a free index in this Directory or return None"""
        for idx, directory in enumerate(self.directory_list):
            name_data = bytearray(directory["DIR_Name"])
            if name_data[0] in (0x00, 0xE5):
                return idx
        return None

    def pack(self):
        """Return a byte a Directory"""
        data = bytearray()
        for directory in self.directory_list:
            data.extend(directory.pack())
        return data


class Fat(object):
    """Wrapper class for a FAT filesystem on a SCSI device"""

    SECTOR_SIZE = 512
    CLUSTER_SIZE = 4 * 1024

    def __init__(self, msd):
        self.msd = msd
        self.reload()

    def reload(self):
        """Reload all internal data of this Fat filesystem"""

        # Read MBR
        mbr_data = self.msd.scsi_read10(0, 1)
        mbr = MBR(mbr_data, 0)

        # Read in the root directory
        root_dir_sec = (mbr["BPB_RsvdSecCnt"] +
                        (mbr["BPB_NumFATs"] * mbr["BPB_FATSz16"]))
        sec_count = (mbr["BPB_RootEntCnt"] * 32 + 512 - 1) // 512
        root_dir_data = self.msd.scsi_read10(root_dir_sec, sec_count)
        root_dir = Directory(mbr["BPB_RootEntCnt"], root_dir_data,
                             root_dir_sec)
        self.mbr = mbr
        self.root_dir = root_dir
