/**
 * @file    flash_hal.c
 * @brief   Implementation of flash_hal.h
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
#include "cortex_m.h"

uint32_t  flash_erase_sector(uint32_t addr)
{
    cortex_int_state_t state;
    int retval = -1;
    state = cortex_int_get_and_disable();
    retval = EraseSector(addr);
    cortex_int_restore(state);
    return retval;
}

uint32_t  flash_program_page(uint32_t adr, uint32_t sz, uint8_t *buf)
{
    int retval = -1;
    cortex_int_state_t state;
    state = cortex_int_get_and_disable();
    retval = ProgramPage(adr, sz, (uint32_t *)buf);
    cortex_int_restore(state);
    return retval;
}
