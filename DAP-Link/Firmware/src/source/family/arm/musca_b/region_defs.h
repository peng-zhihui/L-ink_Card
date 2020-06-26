/**
 * @file    utils.h
 * @brief   Region defines for Musca B target
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2018-2019, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#define MUSCA_B_NS_FLASH_START      0x00000000
#define MUSCA_B_S_FLASH_START       0x10000000
#define MUSCA_B_FLASH_SIZE          MB(8)

#define MUSCA_B_NS_EFLASH0_START    0x0A000000
#define MUSCA_B_S_EFLASH0_START     0x1A000000
#define MUSCA_B_EFLASH0_SIZE        MB(2)

#define MUSCA_B_NS_EFLASH1_START    0x0A200000
#define MUSCA_B_S_EFLASH1_START     0x1A200000
#define MUSCA_B_EFLASH1_SIZE        MUSCA_B_EFLASH0_SIZE

#define MUSCA_B_NS_CODE_SRAM_START  0x0A400000
#define MUSCA_B_S_CODE_SRAM_START   0x1A400000
#define MUSCA_B_CODE_SRAM_SIZE      KB(512)

#define MUSCA_B_NS_RAM_START        0x20000000
#define MUSCA_B_S_RAM_START         0x30000000
#define MUSCA_B_RAM_SIZE            KB(512)
