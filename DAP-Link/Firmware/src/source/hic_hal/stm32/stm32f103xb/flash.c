/**
 * @file    flash_hal_stm32f103xb.c
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
#include "stm32f1xx.h"
#include "util.h"
#include "string.h"
#include "target_board.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
uint32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    //
    // No special init required
    //
    return (0);
}

uint32_t UnInit(uint32_t fnc)
{
    //
    // No special uninit required
    //
    return (0);
}

uint32_t EraseChip(void)
{
    FLASH_EraseInitTypeDef erase_init;
    uint32_t error;
    uint32_t ret = 0;  // O.K.
    if (g_board_info.target_cfg) {
        HAL_FLASH_Unlock();
        //bootloader, interface flashing only concerns 1 flash region
        util_assert((g_board_info.target_cfg->flash_regions[0].end - g_board_info.target_cfg->flash_regions[0].start) %
                    FLASH_PAGE_SIZE == 0);
        memset(&erase_init, 0, sizeof(erase_init));
        erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
        erase_init.PageAddress = g_board_info.target_cfg->flash_regions[0].start;
        erase_init.NbPages = (g_board_info.target_cfg->flash_regions[0].end - g_board_info.target_cfg->flash_regions[0].start) % FLASH_PAGE_SIZE;
        if (HAL_FLASHEx_Erase(&erase_init, &error) != HAL_OK) {
            ret = 1;
        }
        
        HAL_FLASH_Lock();
    }else{
        ret = 1;
    }
    return ret;
}

uint32_t EraseSector(uint32_t adr)
{
    FLASH_EraseInitTypeDef erase_init;
    uint32_t error;
    uint32_t ret = 0;  // O.K.

    HAL_FLASH_Unlock();
    
    memset(&erase_init, 0, sizeof(erase_init));
    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.PageAddress = adr;
    erase_init.NbPages = 1;
    if (HAL_FLASHEx_Erase(&erase_init, &error) != HAL_OK) {
        ret = 1;
    }

    HAL_FLASH_Lock();
    return ret;
}

uint32_t ProgramPage(uint32_t adr, uint32_t sz, uint32_t *buf)
{
    uint32_t i;
    uint32_t ret = 0;  // O.K.

    HAL_FLASH_Unlock();

    util_assert(sz % 4 == 0);
    for (i = 0; i < sz / 4; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, adr + i * 4, buf[i]) != HAL_OK) {
            ret = 1;
            break;
        }
    }

    HAL_FLASH_Lock();
    return ret;
}
