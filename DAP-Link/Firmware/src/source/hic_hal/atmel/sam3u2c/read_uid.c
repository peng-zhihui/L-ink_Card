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

#include "stdbool.h"

#include "read_uid.h"
#include "sam3u.h"
#include "cortex_m.h"

bool unique_id_created = false;
static uint32_t unique_id[4];
void read_unique_id(uint32_t *id)
{
    if (!unique_id_created) {
        create_unique_id();
        unique_id_created = true;
    }

    id[0] = unique_id[0];
    id[1] = unique_id[1];
    id[2] = unique_id[2];
    id[3] = unique_id[3];
}

__attribute__((section("ram_func")))
void create_unique_id(void)
{
    cortex_int_state_t state;
    state = cortex_int_get_and_disable();
    EFC0->EEFC_FMR |= (1UL << 16);
    EFC0->EEFC_FCR = 0x5A00000E;

    /*Monitor FRDY*/
    while ((EFC0->EEFC_FSR & EEFC_FSR_FRDY) == EEFC_FSR_FRDY);

    unique_id[0] = *(uint32_t *)0x80000;
    unique_id[1] = *(uint32_t *)0x80004;
    unique_id[2] = *(uint32_t *)0x80008;
    unique_id[3] = *(uint32_t *)0x8000C;
    EFC0->EEFC_FCR = 0x5A00000F;

    /*Monitor FRDY*/
    while ((EFC0->EEFC_FSR & EEFC_FSR_FRDY) != EEFC_FSR_FRDY);

    EFC0->EEFC_FMR &= ~(1UL << 16);
    cortex_int_restore(state);
}
