/**
 * @file    flash_intf.c
 * @brief   Implementation of flash_intf.h
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

#include "flash_intf.h"


__attribute__((weak))
const flash_intf_t *const flash_intf_iap_protected = 0;
__attribute__((weak))
const flash_intf_t *const flash_intf_target = 0;
__attribute__((weak))
const flash_intf_t *const flash_intf_target_custom = 0;
