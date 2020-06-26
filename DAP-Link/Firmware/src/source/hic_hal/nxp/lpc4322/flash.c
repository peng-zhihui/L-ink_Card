/**
 * @file    flash_hal_stub.c
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

#include "flash_hal.h"
#include "system_LPC43xx.h"
#include "daplink_addr.h"
#include "cortex_m.h"

#define END_SECTOR     14  /* 15 sectors per bank */
#define FLASH_BANK_A    0
#define FLASH_BANK_B    1

/* IAP Structure */
struct sIAP {
    uint32_t cmd;           // Command
    uint32_t par[4];        // Parameters
    uint32_t stat;          // Status
    uint32_t res[2];        // Result
} IAP;

/* IAP Call */
typedef void (*IAP_Entry) (uint32_t *cmd, uint32_t *stat);
#define IAP_Call ((IAP_Entry) (*(volatile unsigned int *)(0x10400100)))

uint32_t GetSecNum (uint32_t adr) {
    uint32_t n;
    uint32_t flashadr = (adr & 0xFFFFFF);

    if (flashadr < 0x10000) {
        n = flashadr >> 13;          //  8kB Sector
    } else {
        n = (flashadr >> 16) + 7;    // 64kB Sector
    }

    return n;                        // Sector Number
}


uint32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    cortex_int_state_t local_state = cortex_int_get_and_disable();

    IAP.cmd    = 49;                            // Initialize
    IAP.par[0] = 0;
    IAP_Call(&IAP.cmd, &IAP.stat);              // Call IAP Command

    cortex_int_restore(local_state);

    return 0;
}

uint32_t UnInit(uint32_t fnc)
{
    return 0;
}

uint32_t EraseChip(void)
{
    cortex_int_state_t local_state = cortex_int_get_and_disable();

    IAP.cmd    = 50;                            // Prepare Sector for Erase
    IAP.par[0] = 0;                             // Start Sector
    IAP.par[1] = END_SECTOR;                    // End Sector
    IAP.par[2] = FLASH_BANK_A;                  // Flash Bank
    IAP_Call(&IAP.cmd, &IAP.stat);              // Call IAP Command
    if (IAP.stat) {
        cortex_int_restore(local_state);
        return 1;  // Command Failed
    }

    IAP.cmd    = 52;                            // Erase Sector
    IAP.par[0] = 0;                             // Start Sector
    IAP.par[1] = END_SECTOR;                    // End Sector
    IAP.par[2] = SystemCoreClock / 1000;        // CCLK in kHz
    IAP.par[3] = FLASH_BANK_A;                  // Flash Bank
    IAP_Call(&IAP.cmd, &IAP.stat);              // Call IAP Command
    if (IAP.stat) {
        cortex_int_restore(local_state);
        return 1;  // Command Failed
    }

    cortex_int_restore(local_state);

    return 0;
}

uint32_t EraseSector(uint32_t adr)
{
    uint32_t n;

    n = GetSecNum(adr);                         // Get Sector Number

    cortex_int_state_t local_state = cortex_int_get_and_disable();

    IAP.cmd    = 50;                            // Prepare Sector for Erase
    IAP.par[0] = n;                             // Start Sector
    IAP.par[1] = n;                             // End Sector
    IAP.par[2] = FLASH_BANK_A;                  // Flash Bank
    IAP_Call(&IAP.cmd, &IAP.stat);              // Call IAP Command
    if (IAP.stat) {
        cortex_int_restore(local_state);
        return 1;  // Command Failed
    }

    IAP.cmd    = 52;                            // Erase Sector
    IAP.par[0] = n;                             // Start Sector
    IAP.par[1] = n;                             // End Sector
    IAP.par[2] = SystemCoreClock / 1000;        // CCLK in kHz
    IAP.par[3] = FLASH_BANK_A;                  // Flash Bank
    IAP_Call(&IAP.cmd, &IAP.stat);              // Call IAP Command
    if (IAP.stat) {
        cortex_int_restore(local_state);
        return 1;  // Command Failed
    }

    cortex_int_restore(local_state);

    return 0;
}

uint32_t ProgramPage(uint32_t adr, uint32_t sz, uint32_t *buf)
{
    uint32_t n;

    if (adr == DAPLINK_ROM_START) {                              // Check for Vector Table
        n = *((unsigned long *)(buf + 0)) +
            *((unsigned long *)(buf + 1)) +
            *((unsigned long *)(buf + 2)) +
            *((unsigned long *)(buf + 3)) +
            *((unsigned long *)(buf + 4)) +
            *((unsigned long *)(buf + 5)) +
            *((unsigned long *)(buf + 6));
            *((unsigned long *)(buf + 7)) = 0 - n;  // Signature at Reserved Vector
    }
    n = GetSecNum(adr);                         // Get Sector Number

    cortex_int_state_t local_state = cortex_int_get_and_disable();

    IAP.cmd    = 50;                            // Prepare Sector for Write
    IAP.par[0] = n;                             // Start Sector
    IAP.par[1] = n;                             // End Sector
    IAP.par[2] = FLASH_BANK_A;                  // Flash Bank
    IAP_Call(&IAP.cmd, &IAP.stat);              // Call IAP Command
    if (IAP.stat) {
        cortex_int_restore(local_state);
        return 1;  // Command Failed
    }

    IAP.cmd    = 51;                            // Copy RAM to Flash
    IAP.par[0] = adr;                           // Destination Flash Address
    IAP.par[1] = (unsigned long)buf;            // Source RAM Address
    IAP.par[2] = 0x400;                         // Fixed Page Size
    IAP.par[3] = SystemCoreClock / 1000;        // CCLK in kHz
    IAP_Call(&IAP.cmd, &IAP.stat);              // Call IAP Command
    if (IAP.stat) {
        cortex_int_restore(local_state);
        return 1;  // Command Failed
    }

    cortex_int_restore(local_state);

    return 0;
}
