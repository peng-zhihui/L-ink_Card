/**
 * @file    FlashPrg.c
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

#include "FlashOS.h"        // FlashOS Structures
#include "fsl_flash.h"
#include "string.h"
#include "cortex_m.h"

flash_config_t g_flash; //!< Storage for flash driver.

uint32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    cortex_int_state_t state = cortex_int_get_and_disable();
#if defined (WDOG)
    /* Write 0xC520 to the unlock register */
    WDOG->UNLOCK = 0xC520;
    /* Followed by 0xD928 to complete the unlock */
    WDOG->UNLOCK = 0xD928;
    /* Clear the WDOGEN bit to disable the watchdog */
    WDOG->STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
#else
    SIM->COPC = 0x00u;
#endif
    cortex_int_restore(state);

    return (FLASH_Init(&g_flash) != kStatus_Success);
}


/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

uint32_t UnInit(uint32_t fnc)
{
    return (0);
}


/*  Blank Check Block in Flash Memory
 *    Parameter:      adr:  Block Start Address
 *                    sz:   Block Size (in bytes)
 *                    pat:  Block Pattern
 *    Return Value:   0 - OK,  1 - Failed
 */

// int BlankCheck (unsigned long adr, unsigned long sz, unsigned char pat)
// {
//     return (flash_verify_erase(&g_flash, adr, sz, kFlashMargin_Normal) != kStatus_Success);
// }
//
// /*
//  *  Verify Flash Contents
//  *    Parameter:      adr:  Start Address
//  *                    sz:   Size (in bytes)
//  *                    buf:  Data
//  *    Return Value:   (adr+sz) - OK, Failed Address
//  */
// unsigned long Verify (unsigned long adr, unsigned long sz, unsigned char *buf)
// {
//     uint32_t failedAddress;
//     status_t status = flash_verify_program(&g_flash, adr, sz,
//                               (const uint8_t *)buf, kFlashMargin_Normal,
//                               &failedAddress, NULL);
//
//     if (status == kStatus_Success)
//     {
//         // Finished without Errors
//         return (adr+sz);
//     }
//     else
//     {
//         return failedAddress;
//     }
// }

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */
uint32_t EraseChip(void)
{
    cortex_int_state_t state = cortex_int_get_and_disable();
    int status = FLASH_EraseAll(&g_flash, kFLASH_apiEraseKey);
    if (status == kStatus_Success)
    {
        status = FLASH_VerifyEraseAll(&g_flash, kFLASH_marginValueNormal);
    }
    cortex_int_restore(state);
    return status;
}

/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */
uint32_t EraseSector(uint32_t adr)
{
    cortex_int_state_t state = cortex_int_get_and_disable();
    int status = FLASH_Erase(&g_flash, adr, g_flash.PFlashSectorSize, kFLASH_apiEraseKey);
    if (status == kStatus_Success)
    {
        status = FLASH_VerifyErase(&g_flash, adr, g_flash.PFlashSectorSize, kFLASH_marginValueNormal);
    }
    cortex_int_restore(state);
    return status;
}

/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */
uint32_t ProgramPage(uint32_t adr, uint32_t sz, uint32_t *buf)
{
    cortex_int_state_t state = cortex_int_get_and_disable();
    int status = FLASH_Program(&g_flash, adr, buf, sz);
    if (status == kStatus_Success)
    {
        // Must use kFlashMargin_User, or kFlashMargin_Factory for verify program
        status = FLASH_VerifyProgram(&g_flash, adr, sz,
                              buf, kFLASH_marginValueUser,
                              NULL, NULL);
    }
    cortex_int_restore(state);
    return status;
}

