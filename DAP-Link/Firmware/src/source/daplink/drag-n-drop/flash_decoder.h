/**
 * @file    flash_decoder.h
 * @brief   Steam handling of different image types that are supported
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

#ifndef FLASH_DECODER_H
#define FLASH_DECODER_H

#include <stdint.h>
#include <stdbool.h>

#include "flash_intf.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

// Enough space for 12 vectors
#define FLASH_DECODER_MIN_SIZE      0x30

typedef enum {
    FLASH_DECODER_TYPE_UNKNOWN,
    FLASH_DECODER_TYPE_BOOTLOADER,
    FLASH_DECODER_TYPE_INTERFACE,
    FLASH_DECODER_TYPE_TARGET,
} flash_decoder_type_t;

flash_decoder_type_t flash_decoder_detect_type(const uint8_t *data, uint32_t size, uint32_t addr, bool addr_valid);
error_t flash_decoder_get_flash(flash_decoder_type_t type, uint32_t addr, bool addr_valid, uint32_t *start_addr, const flash_intf_t **flash_intf);

error_t flash_decoder_open(void);
error_t flash_decoder_write(uint32_t addr, const uint8_t *data, uint32_t size);
error_t flash_decoder_close(void);

#ifdef __cplusplus
}
#endif

#endif
