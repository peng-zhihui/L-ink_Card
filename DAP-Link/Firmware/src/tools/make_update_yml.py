#
# DAPLink Interface Firmware
# Copyright (c) 2009-2018, ARM Limited, All Rights Reserved
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

import logging

logging.basicConfig(format='Line: %(lineno)d %(message)s')
logger = logging.getLogger('yaml gen')
logger.setLevel(logging.DEBUG)


class DefaultList(list):
    def sort(self, *args, **kwargs):
        pass

class TargetList(list):
    def sort(self, *args, **kwargs):
        pass

class InstructionList(list): 
    def sort(self, *args, **kwargs):
        pass


#instruction steps for firmware
InstructionsText = {
    'default':InstructionList([
        ('windows', [
            '1. Download the firmware file.',
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate and mount as `BOOTLOADER` or `MAINTENANCE`. For boards that enumerate as `BOOTLOADER` [see our blog to determine if an update for the DAPLink bootloader is available.](https://os.mbed.com/blog/entry/DAPLink-bootloader-update/)',
            '3. Drag-and-drop the firmware file onto the mounted drive.',
            '4. Wait for the file copy operation to complete.',
            '5. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.'
        ]),
        ('osx', [
            '1. Download the firmware file.',
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate as `BOOTLOADER` or `MAINTENANCE`. For boards that enumerate as `BOOTLOADER` [see our blog to determine if an update for the DAPLink bootloader is available.](https://os.mbed.com/blog/entry/DAPLink-bootloader-update/)',
            '3. In a terminal execute',
            '   - `sudo mount -u -w -o sync /Volumes/MAINTENANCE ; cp -X <path to firmware file> /Volumes/MAINTENANCE/`',
            '   - Note: If your drive does not mount as `MAINTENANCE` make sure to change this to match the name of the mounted disk attached to your system.',
            '4. Wait for the file copy operation to complete.',
            '5. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.'
        ]),
        ('linux', [
            '1. Download the firmware file.',
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate as `BOOTLOADER` or `MAINTENANCE`. For boards that enumerate as `BOOTLOADER` [see our blog to determine if an update for the DAPLink bootloader is available.](https://os.mbed.com/blog/entry/DAPLink-bootloader-update/)',
            '3. In a terminal execute',
            '   - `cp <path to firmware file> <MAINTENANCE> && sync`',
            '   - Note: make sure to change `MAINTENANCE` to the name of the mount point of the drive on your system.',
            '4. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.'
        ])
    ]),
    'lpc11u35':InstructionList([
        ('windows', [
            '1. Download the firmware file.',
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate and mount as `CRP DISABLD`',
            '3. Delete the file named `firmware.bin`, then drag and drop or copy the new bin file',
            '4. Wait for the file copy operation to complete.',
            '5. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.'
        ]),
        ('osx', [
            '1. Download the firmware file.',
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate and mount as `CRP DISABLD`',
            '3. Delete the file named `firmware.bin`',
            '4. In a terminal execute',
            '   - `sudo mount -u -w -o sync /Volumes/CRP\ DISABLD ; rm /Volumes/CRP\ DISABLD/firmware.bin && cp -X <path to firmware file> /Volumes/CRP\ DISABLD/`',
            '5. Wait for the file copy operation to complete.',
            '6. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.'
        ]),
        ('linux', [
            '1. Download the firmware file.',
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate and mount as `CRP DISABLD`',
            '3. Delete the file named `firmware.bin`',
            '4. In a terminal execute',
            '   - `cp <path to firmware file> <CRP DISABLD> && sync`',
            '   - Note: make sure to change `CRP DISABLD` to the name of the mount point on your system.',
            '5. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.'
        ])
    ])

}


def string_writer(element):
    if type(element) is str:
        return element
    elif type(element) is int:
        return "_0x%04i" % element
    else:
        return None


def yml_object_parser(f, entry, level, tabs):
    header = " " * level * tabs
    if type(entry) is list:
        for element in entry:
            yml_object_parser(f, element, level, tabs);

    elif type(entry) is dict:
        for key in entry:
            f.write('-' + ' ' * (level * tabs -1) + string_writer(key) + ": ");
            value = string_writer(entry[key])
            if value is not None:
                f.write(value)
                f.write('\n')
            else:
                f.write('\n')
                yml_object_parser(f, entry[key], level + 1, tabs);
            

    elif type(entry) is DefaultList:     
        for target in entry:
            if type(target) is tuple:        
                #print target
                f.write(" " * level * tabs + string_writer(target[0]) + ": ");
                value = string_writer(target[1])
                if value is not None:
                    f.write(value)
                    f.write('\n')
                else:
                    f.write('\n')
                    yml_object_parser(f, target[1], level + 1, tabs);
            else:
                logger.error("Not expecting input type %s %s " % (type(target), str(target)))

    elif type(entry) is TargetList:         
        #print "found TargetList"
        for target in entry:
            if type(target) is tuple:        
                #print target
                f.write(" " * (level-1) * tabs + string_writer(target[0]) + ": ");
                value = string_writer(target[1])
                if value is not None:
                    f.write(value)
                    f.write('\n')
                else:
                    f.write('\n')
                    yml_object_parser(f, target[1], level, tabs);
            else:
                logger.error("Not expecting input type %s %s " % (type(target), str(target)))

    elif type(entry) is InstructionList:     
        #print "found InstructionList"        
        for target in entry:
            if type(target) is tuple:    
                f.write(" " * level * tabs + string_writer(target[0]) + ": |\n")
                if type(target[1]) is list:
                    for texts in target[1]:
                        f.write(" " * (level + 1) * tabs + string_writer(texts) + "\n")
                else:
                    logger.error("Not expecting input type %s %s " % (type(target[1]), str(target[1])))
            else:
                logger.error("Not expecting input type %s %s " % (type(target), str(target)))
    else:
        logger.error("Not expecting input type %s %s " % (type(entry), str(entry)))


def make_update_yml_file(file_loc, entries, explicit_start=False, tab=2):
    with open(file_loc, "w") as yml_file:
        if explicit_start is True:
            yml_file.write("---\n")
        yml_object_parser(yml_file, entries, 1, tab);
