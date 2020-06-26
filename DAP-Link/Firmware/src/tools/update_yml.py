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

import yaml

#classes needed to generate update.yml files
class TargetList(list):
    def sort(self, *args, **kwargs):
        pass

class InstructionList(unicode): 
    def sort(self, *args, **kwargs):
        pass

#Remove the sorting by default
yaml.add_representer(TargetList, yaml.representer.SafeRepresenter.represent_dict)
#needed block style for the instruction steps
yaml.add_representer(InstructionList, lambda dumper, data: dumper.represent_scalar(u'tag:yaml.org,2002:str', data, style='|') )


#instruction steps for firmware
InstructionsText = {
    'default':TargetList([
        ('windows', InstructionList(
            '1. Download the firmware file.\n'
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate and mount as `BOOTLOADER` or `MAINTENANCE`. For boards that enumerate as `BOOTLOADER` [see our blog to determine if an update for the DAPLink bootloader is available.](https://os.mbed.com/blog/entry/DAPLink-bootloader-update/)\n'
            '3. Drag-and-drop the firmware file onto the mounted drive.\n'
            '4. Wait for the file copy operation to complete.\n'
            '5. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.\n'
        )),
        ('osx', InstructionList(
            '1. Download the firmware file.\n'
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate as `BOOTLOADER` or `MAINTENANCE`. For boards that enumerate as `BOOTLOADER` [see our blog to determine if an update for the DAPLink bootloader is available.](https://os.mbed.com/blog/entry/DAPLink-bootloader-update/)\n'
            '3. In a terminal execute\n'
            '   - `sudo mount -u -w -o sync /Volumes/MAINTENANCE ; cp -X <path to firmware file> /Volumes/MAINTENANCE/`\n'
            '   - Note: If your drive does not mount as `MAINTENANCE` make sure to change this to match the name of the mounted disk attached to your system.\n'
            '4. Wait for the file copy operation to complete.\n'
            '5. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.\n'
        )),
        ('linux', InstructionList(
            '1. Download the firmware file.\n'
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate as `BOOTLOADER` or `MAINTENANCE`. For boards that enumerate as `BOOTLOADER` [see our blog to determine if an update for the DAPLink bootloader is available.](https://os.mbed.com/blog/entry/DAPLink-bootloader-update/)\n'
            '3. In a terminal execute\n'
            '   - `cp <path to firmware file> <MAINTENANCE> && sync`\n'
            '   - Note: make sure to change `MAINTENANCE` to the name of the mount point of the drive on your system.\n'
            '4. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.\n'
        ))
    ]),
    'lpc11u35':TargetList([
        ('windows', InstructionList(
            '1. Download the firmware file.\n'
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate and mount as `CRP DISABLD`\n'
            '3. Delete the file named `firmware.bin`\n'
            '4. Wait for the file copy operation to complete.\n'
            '5. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.\n'
        )),
        ('osx', InstructionList(
            '1. Download the firmware file.\n'
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate and mount as `CRP DISABLD`\n'
            '3. Delete the file named `firmware.bin`'
            '4. In a terminal execute\n'
            '   - `sudo mount -u -w -o sync /Volumes/CRP\ DISABLD ; rm /Volumes/CRP\ DISABLD/firmware.bin && cp -X <path to firmware file> /Volumes/CRP\ DISABLD/`\n'
            '5. Wait for the file copy operation to complete.\n'
            '6. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.\n'
        )),
        ('linux', InstructionList(
            '1. Download the firmware file.\n'
            '2. While holding down the boards reset button, connect the boards USB debug port to the computer. It should enumerate and mount as `CRP DISABLD`\n'
            '3. Delete the file named `firmware.bin`\n'
            '4. In a terminal execute\n'
            '   - `cp <path to firmware file> <CRP DISABLD> && sync`\n'
            '   - Note: make sure to change `CRP DISABLD` to the name of the mount point on your system.\n'
            '5. Power cycle the board. It will now enumerate and mount as `DAPLINK` or the name of the board.\n'
        ))
    ])

}
