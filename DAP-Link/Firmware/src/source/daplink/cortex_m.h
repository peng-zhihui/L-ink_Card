/**
 * @file    cortex_m.h
 * @brief   ARM Cortex-Mx cpu state helper functions
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

#ifndef CORTEX_M_H
#define CORTEX_M_H

#include <stdint.h>
#include <stdbool.h>

#include "IO_Config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int cortex_int_state_t;

__attribute__((always_inline))
static cortex_int_state_t cortex_int_get_and_disable(void)
{
    cortex_int_state_t state;
    state = __disable_irq();
    return state;
}

__attribute__((always_inline))
static void cortex_int_restore(cortex_int_state_t state)
{
    if (!state) {
        __enable_irq();
    }
}

__attribute__((always_inline))
static bool cortex_in_isr(void)
{
    return (__get_xPSR() & 0x1F) != 0;
}

extern void SystemReset(void);

#ifdef __cplusplus
}
#endif

#endif
