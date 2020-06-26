/**
 * @file    daplink.c
 * @brief   Implementation of daplink.h for the bootloader application
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

#include "daplink.h"
#include "virtual_fs.h"
#include "compiler.h"

COMPILER_ASSERT(DAPLINK_BUILD_KEY == DAPLINK_BUILD_KEY_BL);

// daplink_mode_file_name, daplink_url_name and
// daplink_drive_name strings must be 11 characters
// excluding the null terminated character
const char daplink_mode_file_name[11] = "START_IFACT";

bool daplink_is_bootloader()
{
    return true;
}

bool daplink_is_interface()
{
    return false;
}
