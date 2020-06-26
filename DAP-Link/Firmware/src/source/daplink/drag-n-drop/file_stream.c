/**
 * @file    file_stream.c
 * @brief   Implementation of file_stream.h
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

#include <string.h>

#include "file_stream.h"
#include "util.h"
#include "intelhex.h"
#include "flash_decoder.h"
#include "error.h"
#include "cmsis_os2.h"
#include "compiler.h"
#include "validation.h"

typedef enum {
    STREAM_STATE_CLOSED,
    STREAM_STATE_OPEN,
    STREAM_STATE_END,
    STREAM_STATE_ERROR
} stream_state_t;

typedef bool (*stream_detect_cb_t)(const uint8_t *data, uint32_t size);
typedef error_t (*stream_open_cb_t)(void *state);
typedef error_t (*stream_write_cb_t)(void *state, const uint8_t *data, uint32_t size);
typedef error_t (*stream_close_cb_t)(void *state);

typedef struct {
    stream_detect_cb_t detect;
    stream_open_cb_t open;
    stream_write_cb_t write;
    stream_close_cb_t close;
} stream_t;

typedef struct {
    uint8_t vector_buf[FLASH_DECODER_MIN_SIZE];
    uint8_t buf_pos;
    uint32_t flash_addr;
} bin_state_t;

typedef struct {
    bool parsing_complete;
    uint8_t bin_buffer[256];
} hex_state_t;

typedef union {
    bin_state_t bin;
    hex_state_t hex;
} shared_state_t;

static bool detect_bin(const uint8_t *data, uint32_t size);
static error_t open_bin(void *state);
static error_t write_bin(void *state, const uint8_t *data, uint32_t size);
static error_t close_bin(void *state);

static bool detect_hex(const uint8_t *data, uint32_t size);
static error_t open_hex(void *state);
static error_t write_hex(void *state, const uint8_t *data, uint32_t size);
static error_t close_hex(void *state);

stream_t stream[] = {
    {detect_bin, open_bin, write_bin, close_bin},   // STREAM_TYPE_BIN
    {detect_hex, open_hex, write_hex, close_hex},   // STREAM_TYPE_HEX
};
COMPILER_ASSERT(ARRAY_SIZE(stream) == STREAM_TYPE_COUNT);
// STREAM_TYPE_NONE must not be included in count
COMPILER_ASSERT(STREAM_TYPE_NONE > STREAM_TYPE_COUNT);

static shared_state_t shared_state;
static stream_state_t state = STREAM_STATE_CLOSED;
static stream_t *current_stream = 0;

// Thread variables (STUB these if RTX is not used)
static osThreadId_t stream_thread_tid = 0;
static void stream_thread_set(void)
{
    stream_thread_tid =  osThreadGetId();
}
static void stream_thread_assert(void)
{
    util_assert(osThreadGetId() == stream_thread_tid);
}

stream_type_t stream_start_identify(const uint8_t *data, uint32_t size)
{
    stream_type_t i;

    for (i = STREAM_TYPE_START; i < STREAM_TYPE_COUNT; i++) {
        if (stream[i].detect(data, size)) {
            return i;
        }
    }

    return STREAM_TYPE_NONE;
}

// Identify the file type from its extension
stream_type_t stream_type_from_name(const vfs_filename_t filename)
{
    // 8.3 file names must be in upper case
    if (0 == strncmp("BIN", &filename[8], 3)) {
        return STREAM_TYPE_BIN;
    } else if (0 == strncmp("HEX", &filename[8], 3)) {
        return STREAM_TYPE_HEX;
    } else {
        return STREAM_TYPE_NONE;
    }
}

error_t stream_open(stream_type_t stream_type)
{
    error_t status;

    // Stream must not be open already
    if (state != STREAM_STATE_CLOSED) {
        util_assert(0);
        return ERROR_INTERNAL;
    }

    // Stream must be of a supported type
    if (stream_type >= STREAM_TYPE_COUNT) {
        util_assert(0);
        return ERROR_INTERNAL;
    }

    stream_thread_set();
    // Initialize all variables
    memset(&shared_state, 0, sizeof(shared_state));
    state = STREAM_STATE_OPEN;
    current_stream = &stream[stream_type];
    // Initialize the specified stream
    status = current_stream->open(&shared_state);

    if (ERROR_SUCCESS != status) {
        state = STREAM_STATE_ERROR;
    }

    return status;
}

error_t stream_write(const uint8_t *data, uint32_t size)
{
    error_t status;

    // Stream must be open already
    if (state != STREAM_STATE_OPEN) {
        util_assert(0);
        return ERROR_INTERNAL;
    }

    // Check thread after checking state since the stream thread is
    // set only if stream_open has been called
    stream_thread_assert();
    // Write to stream
    status = current_stream->write(&shared_state, data, size);

    if (ERROR_SUCCESS_DONE == status) {
        state = STREAM_STATE_END;
    } else if ((ERROR_SUCCESS_DONE_OR_CONTINUE == status) || (ERROR_SUCCESS == status)) {
        // Stream should remain in the open state
        util_assert(STREAM_STATE_OPEN == state);
    } else {
        state = STREAM_STATE_ERROR;
    }

    return status;
}

error_t stream_close(void)
{
    error_t status;

    // Stream must not be closed already
    if (STREAM_STATE_CLOSED == state) {
        util_assert(0);
        return ERROR_INTERNAL;
    }

    // Check thread after checking state since the stream thread is
    // set only if stream_open has been called
    stream_thread_assert();
    // Close stream
    status = current_stream->close(&shared_state);
    state = STREAM_STATE_CLOSED;
    return status;
}

/* Binary file processing */

static bool detect_bin(const uint8_t *data, uint32_t size)
{
    return FLASH_DECODER_TYPE_UNKNOWN != flash_decoder_detect_type(data, size, 0, false);
}

static error_t open_bin(void *state)
{
    error_t status;
    status = flash_decoder_open();
    return status;
}

static error_t write_bin(void *state, const uint8_t *data, uint32_t size)
{
    error_t status;
    bin_state_t *bin_state = (bin_state_t *)state;

    if (bin_state->buf_pos < FLASH_DECODER_MIN_SIZE) {
        flash_decoder_type_t flash_type;
        uint32_t size_left;
        uint32_t copy_size;
        uint32_t start_addr;
        const flash_intf_t *flash_intf;
        // Buffer Data
        size_left = FLASH_DECODER_MIN_SIZE - bin_state->buf_pos;
        copy_size = MIN(size_left, size);
        memcpy(bin_state->vector_buf + bin_state->buf_pos, data, copy_size);
        bin_state->buf_pos += copy_size;

        if (bin_state->buf_pos < FLASH_DECODER_MIN_SIZE) {
            // Not enough data to determine type
            return ERROR_SUCCESS;
        }

        data += copy_size;
        size -= copy_size;
        // Determine type
        flash_type = flash_decoder_detect_type(bin_state->vector_buf, bin_state->buf_pos, 0, false);

        if (FLASH_DECODER_TYPE_UNKNOWN == flash_type) {
            return ERROR_FD_UNSUPPORTED_UPDATE;
        }

        // Determine flash addresss
        status = flash_decoder_get_flash(flash_type, 0, false, &start_addr, &flash_intf);

        if (ERROR_SUCCESS != status) {
            return status;
        }

        bin_state->flash_addr = start_addr;
        // Pass on data to the decoder
        status = flash_decoder_write(bin_state->flash_addr, bin_state->vector_buf, bin_state->buf_pos);

        if (ERROR_SUCCESS != status) {
            return status;
        }

        bin_state->flash_addr += bin_state->buf_pos;
    }

    // Write data
    status = flash_decoder_write(bin_state->flash_addr, data, size);

    if (ERROR_SUCCESS != status) {
        return status;
    }

    bin_state->flash_addr += size;
    // There is no way to determine the end of a binary
    // file so any point could be the end
    return ERROR_SUCCESS_DONE_OR_CONTINUE;
}

static error_t close_bin(void *state)
{
    error_t status;
    status = flash_decoder_close();
    return status;
}

/* Hex file processing */

static bool detect_hex(const uint8_t *data, uint32_t size)
{
    return 1 == validate_hexfile(data);
}

static error_t open_hex(void *state)
{
    error_t status;
    hex_state_t *hex_state = (hex_state_t *)state;
    memset(hex_state, 0, sizeof(*hex_state));
    reset_hex_parser();
    hex_state->parsing_complete = false;
    status = flash_decoder_open();
    return status;
}

static error_t write_hex(void *state, const uint8_t *data, uint32_t size)
{
    error_t status = ERROR_SUCCESS;
    hex_state_t *hex_state = (hex_state_t *)state;
    hexfile_parse_status_t parse_status = HEX_PARSE_UNINIT;
    uint32_t bin_start_address = 0; // Decoded from the hex file, the binary buffer data starts at this address
    uint32_t bin_buf_written = 0;   // The amount of data in the binary buffer starting at address above
    uint32_t block_amt_parsed = 0;  // amount of data parsed in the block on the last call

    while (1) {
        // try to decode a block of hex data into bin data
        parse_status = parse_hex_blob(data, size, &block_amt_parsed, hex_state->bin_buffer, sizeof(hex_state->bin_buffer), &bin_start_address, &bin_buf_written);

        // the entire block of hex was decoded. This is a simple state
        if (HEX_PARSE_OK == parse_status) {
            if (bin_buf_written > 0) {
                status = flash_decoder_write(bin_start_address, hex_state->bin_buffer, bin_buf_written);
            }

            break;
        } else if (HEX_PARSE_UNALIGNED == parse_status) {
            if (bin_buf_written > 0) {
                status = flash_decoder_write(bin_start_address, hex_state->bin_buffer, bin_buf_written);

                if (ERROR_SUCCESS != status) {
                    break;
                }
            }

            // incrememntal offset to finish the block
            size -= block_amt_parsed;
            data += block_amt_parsed;
        } else if (HEX_PARSE_EOF == parse_status) {
            if (bin_buf_written > 0) {
                status = flash_decoder_write(bin_start_address, hex_state->bin_buffer, bin_buf_written);
            }

            if (ERROR_SUCCESS == status) {
                status = ERROR_SUCCESS_DONE;
            }

            break;
        } else if (HEX_PARSE_CKSUM_FAIL == parse_status) {
            status = ERROR_HEX_CKSUM;
            break;
        } else if ((HEX_PARSE_UNINIT == parse_status) || (HEX_PARSE_FAILURE == parse_status)) {
            util_assert(HEX_PARSE_UNINIT != parse_status);
            status = ERROR_HEX_PARSER;
            break;
        }
    }

    return status;
}

static error_t close_hex(void *state)
{
    error_t status;
    status = flash_decoder_close();
    return status;
}
