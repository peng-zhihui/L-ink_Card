/**
 * @file    target_reset.c
 * @brief   Target reset for the cc3220sf
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

#include "swd_host.h"
#include "target_family.h"

#define GPRCM_0_APPS_SOFT_RESET_ADDR   0x4402D000
#define GPRCM_0_RESET_MCU_VALUE        0x1
#define GPRCM_0_RESET_MCU_PERIPH_VALUE 0x2

static uint8_t target_set_state_ti(target_state_t state)
{
    swd_set_soft_reset(VECTRESET);
    if (state == POST_FLASH_RESET)
    {
        // TI recommends reset via writing to PRCM register instead of AIRCR.
        // Reset apps processor and associated peripheral.
        uint32_t write_value = GPRCM_0_RESET_MCU_PERIPH_VALUE;
        swd_write_memory(GPRCM_0_APPS_SOFT_RESET_ADDR, (uint8_t *)&write_value, sizeof(write_value));
        return 1;
    }
    return swd_set_target_state_sw(state);
}

const uint32_t cookieList[]=
{
    0x5AA5A55A,
    0x000FF800,
    0xEFA3247D
};

// Override the weak validate_bin_nvic function. The weak function expects NVIC at the beginning of the flash.
// On CC3220SF, the beginning of the flash is the cookie list, which allows the boot ROM code to jump into onchip flash directly bypassing external flash.
static uint8_t validate_bin_nvic_ti(const uint8_t *buf)
{
    return (memcmp(buf, cookieList, sizeof(cookieList)) == 0);
}

const target_family_descriptor_t g_ti_family = {
    .family_id = kTI_Cc3220sf_FamilyID,
    .target_set_state = target_set_state_ti,
    .validate_bin_nvic = validate_bin_nvic_ti,
};

