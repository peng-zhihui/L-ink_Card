/**
 * @file    file_stream.h
 * @brief   Different file stream parsers that are supported
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

#ifndef FILE_STREAM_H
#define FILE_STREAM_H

#include <stdint.h>

#include "virtual_fs.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    STREAM_TYPE_START = 0,

    STREAM_TYPE_BIN = STREAM_TYPE_START,
    STREAM_TYPE_HEX,

    // Add new stream types here

    STREAM_TYPE_COUNT,

    STREAM_TYPE_NONE
} stream_type_t;

// Stateless function to identify a filestream by its contents
stream_type_t stream_start_identify(const uint8_t *data, uint32_t size);

// Stateless function to identify a filestream by its name
stream_type_t stream_type_from_name(const vfs_filename_t filename);

error_t stream_open(stream_type_t stream_type);

error_t stream_write(const uint8_t *data, uint32_t size);

error_t stream_close(void);

#ifdef __cplusplus
}
#endif

#endif
