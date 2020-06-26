/**
 * @file    cmsis.h
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2019, ARM Limited, All Rights Reserved
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

#ifndef DEVICE_H
#define DEVICE_H

#if defined (INTERFACE_K20D5) \
    || defined (INTERFACE_K26F) \
    || defined (INTERFACE_KL26Z) \
    || defined (INTERFACE_LPC55XX)
#include "fsl_device_registers.h"
#elif defined (INTERFACE_LPC11U35)
#include "LPC11Uxx.h"
#elif defined (INTERFACE_LPC4322)
#include "LPC43xx.h"
#elif defined (INTERFACE_MAX32620)
#include "max32620.h"
#elif defined (INTERFACE_MAX32625)
#include "max32625.h"
#elif defined (INTERFACE_SAM3U2C)
#include "sam3u2c.h"
#elif defined (INTERFACE_STM32F103XB)
#include "stm32f103xb.h"
#elif defined (INTERFACE_M48SSIDAE)
#include "M480.h"
#else
#error "CMSIS core headers needed"
#endif

#endif // DEVICE_H
