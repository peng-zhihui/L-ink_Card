/**
 * @file    read_uid.c
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
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

#include "read_uid.h"
#include "NuMicro.h"

void read_unique_id(uint32_t *id)
{
    SYS_UnlockReg();
    /* Enable FMC ISP function */
    FMC_Open();
    /* Read Unique ID */
    id[0] = FMC_ReadUID(0);
    id[1] = FMC_ReadUID(1);
    id[2] = FMC_ReadUID(2);
    id[3] = 0;
    /* Disable FMC ISP function */
    FMC_Close();
    SYS_LockReg();
}
