/**
 * @file    flash_manager.h
 * @brief   Handling of file steam decode and prep for writing
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

#ifndef FLASH_MANAGER_H
#define FLASH_MANAGER_H

#include <string.h>
#include <stdbool.h>
#include "flash_intf.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

error_t flash_manager_init(const flash_intf_t *flash_intf);
error_t flash_manager_data(uint32_t addr, const uint8_t *data, uint32_t size);
error_t flash_manager_uninit(void);
void flash_manager_set_page_erase(bool enabled);

#ifdef __cplusplus
}
#endif

#endif
