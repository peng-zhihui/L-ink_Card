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

from time import sleep
from random import randrange
import math

from pyocd.core.helpers import ConnectHelper
from pyocd.core.memory_map import MemoryType
from pyocd.flash.loader import FileProgrammer
from pyocd.utility.conversion import float32_to_u32

# TODO - make a dedicated test
# TODO - test all DapLink commands
# TODO - test various clock speeds
# TODO - test turnaround settings
# TODO - test HID speed
# TODO - test ram/rom transfer speeds


def test_hid(workspace, parent_test):
    test_info = parent_test.create_subtest("HID test")
    board = workspace.board
    with ConnectHelper.session_with_chosen_probe(unique_id=board.get_unique_id()) as session:
        mbed_board = session.board
        target = mbed_board.target
        binary_file = workspace.target.bin_path

        ram_region = target.memory_map.get_first_region_of_type(MemoryType.RAM)
        rom_region = target.memory_map.get_boot_memory()

        addr = ram_region.start + 4
        size = 0x502
        addr_bin = rom_region.start
        addr_flash = rom_region.start + rom_region.length // 2

        flash = rom_region.flash

        test_info.info("\r\n\r\n----- FLASH NEW BINARY -----")
        FileProgrammer(session).program(binary_file, base_address=addr_bin)

        test_info.info("\r\n\r\n------ GET Unique ID ------")
        test_info.info("Unique ID: %s" % mbed_board.unique_id)

        test_info.info("\r\n\r\n------ TEST READ / WRITE CORE REGISTER ------")
        pc = target.read_core_register('pc')
        test_info.info("initial pc: 0x%X" % target.read_core_register('pc'))
        # write in pc dummy value
        target.write_core_register('pc', 0x3D82)
        test_info.info("now pc: 0x%X" % target.read_core_register('pc'))
        # write initial pc value
        target.write_core_register('pc', pc)
        test_info.info("initial pc value rewritten: 0x%X" %
                       target.read_core_register('pc'))

        msp = target.read_core_register('msp')
        psp = target.read_core_register('psp')
        test_info.info("MSP = 0x%08x; PSP = 0x%08x" % (msp, psp))

        control = target.read_core_register('control')
        faultmask = target.read_core_register('faultmask')
        basepri = target.read_core_register('basepri')
        primask = target.read_core_register('primask')
        test_info.info("CONTROL = 0x%02x; FAULTMASK = 0x%02x; BASEPRI = 0x%02x; PRIMASK = 0x%02x" % (control, faultmask, basepri, primask))

        target.write_core_register('primask', 1)
        newPrimask = target.read_core_register('primask')
        test_info.info("New PRIMASK = 0x%02x" % newPrimask)
        target.write_core_register('primask', primask)
        newPrimask = target.read_core_register('primask')
        test_info.info("Restored PRIMASK = 0x%02x" % newPrimask)

        if target.has_fpu:
            s0 = target.read_core_register('s0')
            test_info.info("S0 = %g (0x%08x)" % (s0, float32_to_u32(s0)))
            target.write_core_register('s0', math.pi)
            newS0 = target.read_core_register('s0')
            test_info.info("New S0 = %g (0x%08x)" %
                           (newS0, float32_to_u32(newS0)))
            target.write_core_register('s0', s0)
            newS0 = target.read_core_register('s0')
            test_info.info("Restored S0 = %g (0x%08x)" %
                           (newS0, float32_to_u32(newS0)))

        test_info.info("\r\n\r\n------ TEST HALT / RESUME ------")

        test_info.info("resume")
        target.resume()
        sleep(0.2)

        test_info.info("halt")
        target.halt()
        test_info.info("HALT: pc: 0x%X" % target.read_core_register('pc'))
        sleep(0.2)

        test_info.info("\r\n\r\n------ TEST STEP ------")

        test_info.info("reset and halt")
        target.reset_and_halt()
        currentPC = target.read_core_register('pc')
        test_info.info("HALT: pc: 0x%X" % currentPC)
        sleep(0.2)

        for i in range(4):
            test_info.info("step")
            target.step()
            newPC = target.read_core_register('pc')
            test_info.info("STEP: pc: 0x%X" % newPC)
            currentPC = newPC
            sleep(0.2)

        test_info.info("\r\n\r\n------ TEST READ / WRITE MEMORY ------")
        target.halt()
        test_info.info("READ32/WRITE32")
        val = randrange(0, 0xffffffff)
        test_info.info("write32 0x%X at 0x%X" % (val, addr))
        target.write32(addr, val)
        res = target.read32(addr)
        test_info.info("read32 at 0x%X: 0x%X" % (addr, res))
        if res != val:
            test_info.failure("ERROR in READ/WRITE 32")

        test_info.info("\r\nREAD16/WRITE16")
        val = randrange(0, 0xffff)
        test_info.info("write16 0x%X at 0x%X" % (val, addr + 2))
        target.write16(addr + 2, val)
        res = target.read16(addr + 2)
        test_info.info("read16 at 0x%X: 0x%X" % (addr + 2, res))
        if res != val:
            test_info.failure("ERROR in READ/WRITE 16")

        test_info.info("\r\nREAD8/WRITE8")
        val = randrange(0, 0xff)
        test_info.info("write8 0x%X at 0x%X" % (val, addr + 1))
        target.write8(addr + 1, val)
        res = target.read8(addr + 1)
        test_info.info("read8 at 0x%X: 0x%X" % (addr + 1, res))
        if res != val:
            test_info.failure("ERROR in READ/WRITE 8")

        test_info.info("\r\n\r\n------ TEST READ / WRITE MEMORY BLOCK ------")
        data = [randrange(1, 50) for _ in range(size)]
        target.write_memory_block8(addr, data)
        block = target.read_memory_block8(addr, size)
        error = False
        for i in range(len(block)):
            if (block[i] != data[i]):
                error = True
                test_info.info("ERROR: 0x%X, 0x%X, 0x%X!!!" %
                               ((addr + i), block[i], data[i]))
        if error:
            test_info.failure("TEST FAILED")
        else:
            test_info.info("TEST PASSED")

        test_info.info("\r\n\r\n------ TEST RESET ------")
        target.reset()
        sleep(0.1)
        target.halt()

        for i in range(5):
            target.step()
            test_info.info("pc: 0x%X" % target.read_core_register('pc'))

        test_info.info("\r\n\r\n------ TEST PROGRAM/ERASE PAGE ------")
        # Fill 3 pages with 0x55
        page_size = flash.get_page_info(addr_flash).size
        fill = [0x55] * page_size
        for i in range(0, 3):
            address = addr_flash + page_size * i
            # Test only supports a location with 3 aligned
            # pages of the same size
            current_page_size = flash.get_page_info(addr_flash).size
            assert page_size == current_page_size
            assert address % current_page_size == 0

            flash.init(flash.Operation.ERASE)
            flash.erase_page(address)
            flash.uninit()

            flash.init(flash.Operation.PROGRAM)
            flash.program_page(address, fill)
            flash.uninit()
        # Erase the middle page
        flash.init(flash.Operation.ERASE)
        flash.erase_page(addr_flash + page_size)
        flash.cleanup()
        # Verify the 1st and 3rd page were not erased, and that the 2nd page is fully erased
        data = target.read_memory_block8(addr_flash, page_size * 3)
        expected = fill + [0xFF] * page_size + fill
        if data == expected:
            test_info.info("TEST PASSED")
        else:
            test_info.failure("TEST FAILED")

        test_info.info("\r\n\r\n----- Restoring image -----")
        FileProgrammer(session).program(binary_file, base_address=addr_bin)

        target.reset()
        test_info.info("HID test complete")
