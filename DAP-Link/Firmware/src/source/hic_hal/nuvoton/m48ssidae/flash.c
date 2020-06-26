/**
 * @file    flash.c
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

#include "flash_hal.h"        // FlashOS Structures
#include "target_config.h"    // target_device
#include "util.h"
#include "string.h"
#include "target_board.h"
#include "NuMicro.h"

uint32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    SYS_UnlockReg();
    /* Enable FMC ISP function */
    FMC_Open();
    FMC_ENABLE_AP_UPDATE();
    return 0;
}

uint32_t UnInit(uint32_t fnc)
{
    FMC_DISABLE_AP_UPDATE();
    /* Disable FMC ISP function */
    FMC_Close();
    SYS_LockReg();
    return 0;
}

uint32_t EraseChip(void)
{
    return 1;
}

uint32_t EraseSector(uint32_t adr)
{
    uint32_t ret = 0;

    if (FMC_Erase(adr) != 0) {
        ret = 1;
    }

    return ret;
}

uint32_t ProgramPage(uint32_t adr, uint32_t sz, uint32_t *buf)
{
    uint32_t i;
    uint32_t ret = 0;

    for (i = 0; i < sz; i += 4) {
        FMC_Write(adr + i, buf[i / 4]);

        if (FMC_GET_FAIL_FLAG()) {
            FMC_CLR_FAIL_FLAG();
            ret = 1;
            break;
        }
    }

    return ret;
}
