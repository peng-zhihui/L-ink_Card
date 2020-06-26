/**
 * @file    utils.c
 * @brief   Utilities for Musca B target
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2018-2019, ARM Limited, All Rights Reserved
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

#include "stdint.h"

void delay(uint32_t cycles)
{
    volatile uint32_t i;
    i = cycles * 4000;        // fudge factor to give approximate 1 ms

    while (i > 0) {
        i--;
    }
}

void delay_us(uint32_t cycles)
{
    volatile uint32_t i;
    i = cycles * 4;           // fudge factor to give approximate 1 us

    while (i > 0) {
        i--;
    }
}
