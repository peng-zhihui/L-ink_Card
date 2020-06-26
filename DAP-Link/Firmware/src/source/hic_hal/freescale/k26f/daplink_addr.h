/**
 * @file    daplink_addr.h
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
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

#ifndef DAPLINK_ADDR_H
#define DAPLINK_ADDR_H

/* Device sizes */

#define DAPLINK_ROM_START               0x00000000
#define DAPLINK_ROM_SIZE                0x00040000 // Intentionally set to only 256 kB of total 2 MB.

#define DAPLINK_RAM_START               0x1fff0000
#define DAPLINK_RAM_SIZE                0x00040000

/* ROM sizes */

#define DAPLINK_ROM_BL_START            0x00000000
#define DAPLINK_ROM_BL_SIZE             0x00010000 // 64 kB bootloader

#define DAPLINK_ROM_CONFIG_ADMIN_START  0x00010000
#define DAPLINK_ROM_CONFIG_ADMIN_SIZE   0x00000000

#define DAPLINK_ROM_IF_START            0x00010000
#define DAPLINK_ROM_IF_SIZE             0x0002f000 // 192 kB interface

#define DAPLINK_ROM_CONFIG_USER_START   0x0003f000
#define DAPLINK_ROM_CONFIG_USER_SIZE    0x00001000 // 4 kB user config

/* RAM sizes */

#define DAPLINK_RAM_APP_START           0x1fff0000
#define DAPLINK_RAM_APP_SIZE            0x0003ff00

#define DAPLINK_RAM_SHARED_START        0x2002ff00
#define DAPLINK_RAM_SHARED_SIZE         0x00000100

/* Flash Programming Info */

#define DAPLINK_SECTOR_SIZE             0x00001000
#define DAPLINK_MIN_WRITE_SIZE          0x00000100

/* Current build */

#if defined(DAPLINK_BL)

#define DAPLINK_ROM_APP_START            DAPLINK_ROM_BL_START
#define DAPLINK_ROM_APP_SIZE             DAPLINK_ROM_BL_SIZE
#define DAPLINK_ROM_UPDATE_START         DAPLINK_ROM_IF_START
#define DAPLINK_ROM_UPDATE_SIZE          DAPLINK_ROM_IF_SIZE

#elif defined(DAPLINK_IF)

#define DAPLINK_ROM_APP_START            DAPLINK_ROM_IF_START
#define DAPLINK_ROM_APP_SIZE             DAPLINK_ROM_IF_SIZE
#define DAPLINK_ROM_UPDATE_START         DAPLINK_ROM_BL_START
#define DAPLINK_ROM_UPDATE_SIZE          DAPLINK_ROM_BL_SIZE

#else

#error "Build must be either bootloader or interface"

#endif

#endif
