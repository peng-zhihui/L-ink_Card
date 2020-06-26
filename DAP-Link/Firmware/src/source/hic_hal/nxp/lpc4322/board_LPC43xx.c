/**
 * @file    board_LPC43xx.c
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

#include "sdk.h"
#include "lpc43xx_cgu.h"

void sdk_init(void)
{
    /* Set core clock to 96MHz */
    CGU_Init(96000000);
    /* Set up USB0 clock */
    /* Disable PLL first */
    CGU_EnableEntity(CGU_CLKSRC_PLL0, DISABLE);

    /* the usb core require output clock = 480MHz */
    if (CGU_SetPLL0() != CGU_ERROR_SUCCESS) {
        while (1);
    }

    CGU_EntityConnect(CGU_CLKSRC_XTAL_OSC, CGU_CLKSRC_PLL0);
    /* Enable PLL after all setting is done */
    CGU_EnableEntity(CGU_CLKSRC_PLL0, ENABLE);
    /* Turn on the USB0PHY */
    LPC_CREG->CREG0 &= ~(1 << 5);
}
