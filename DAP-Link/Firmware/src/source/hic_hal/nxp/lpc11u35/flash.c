/**
 * @file    flash.c
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2017-2017, ARM Limited, All Rights Reserved
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

#include <string.h>
#include "flash_hal.h"
#include "iap.h"
#include "util.h"
#include "LPC11Uxx.h"

// page buffer must be 4 byte algined and must reside in ram bank 1
static __attribute__((section("RAM1"))) uint32_t page_buf[64];

static uint32_t get_sector_num(uint32_t addr);

uint32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    return 0; // No init needed
}

uint32_t UnInit(uint32_t fnc)
{
    return 0; // No init needed
}

uint32_t EraseChip(void)
{
    return (1); // IAP not supported
}

uint32_t EraseSector(uint32_t adr)
{
    uint32_t num = get_sector_num(adr);

    iap_lock();

    iap_op.cmd = 50;        // Prepare Sector for Erase
    iap_op.par[0] = num;    // Start Sector
    iap_op.par[1] = num;    // End Sector
    iap_call(&iap_op);
    if (iap_op.stat != CMD_SUCCESS) {
        iap_unlock();
        return (1);
    }

    iap_op.cmd = 52;                        // Erase Sector
    iap_op.par[0] = num;                    // Start Sector
    iap_op.par[1] = num;                    // End Sector
    iap_op.par[2] = SystemCoreClock / 1000; // Core Clock in kHz
    iap_call(&iap_op);
    if (iap_op.stat != CMD_SUCCESS) {
        iap_unlock();
        return (1);
    }

    iap_unlock();

    return 0; // Success
}

uint32_t ProgramPage(uint32_t adr, uint32_t sz, uint32_t *buf)
{
    uint32_t num = get_sector_num(adr);

    iap_lock();

    iap_op.cmd = 50;        // Prepare Sector for Program
    iap_op.par[0] = num;    // Start Sector
    iap_op.par[1] = num;    // End Sector
    iap_call(&iap_op);
    if (iap_op.stat != CMD_SUCCESS) {
        iap_unlock();
        return (1);
    }

    while (sz > 0) {
        uint32_t copy_size = MIN(sz, sizeof(page_buf));
        memset(page_buf, 0xFF, sizeof(page_buf));
        memcpy(page_buf, buf, copy_size);
        iap_op.cmd = 51;                        // Erase Sector
        iap_op.par[0] = adr;                    // Destination
        iap_op.par[1] = (uint32_t)page_buf;     // Source
        iap_op.par[2] = 256;                    // Write size
        iap_op.par[3] = SystemCoreClock / 1000; // Core Clock in kHz
        iap_call(&iap_op);
        if (iap_op.stat != CMD_SUCCESS) {
            iap_unlock();
            return (1);
        }
        sz -= copy_size;
        adr += copy_size;
        buf += copy_size / 4;
    }

    iap_unlock();

    return 0; // Success
}

static uint32_t get_sector_num(uint32_t addr)
{
    return addr / 0x1000;
}
