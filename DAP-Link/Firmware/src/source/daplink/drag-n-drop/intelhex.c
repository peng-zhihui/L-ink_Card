/**
 * @file    intelhex.c
 * @brief   Implementation of intelhex.h
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

#include "intelhex.h"

typedef enum hex_record_t hex_record_t;
enum hex_record_t {
    DATA_RECORD = 0,
    EOF_RECORD = 1,
    EXT_SEG_ADDR_RECORD = 2,
    START_SEG_ADDR_RECORD = 3,
    EXT_LINEAR_ADDR_RECORD = 4,
    START_LINEAR_ADDR_RECORD = 5
};

typedef union hex_line_t hex_line_t;
union __attribute__((packed)) hex_line_t {
    uint8_t buf[0x25];
    struct __attribute__((packed)) {
        uint8_t  byte_count;
        uint16_t address;
        uint8_t  record_type;
        uint8_t  data[0x25 - 0x5];
        uint8_t  checksum;
    };
};

/** Swap 16bit value - let compiler figure out the best way
 *  @param val a variable of size uint16_t to be swapped
 *  @return the swapped value
 */
static uint16_t swap16(uint16_t a)
{
    return ((a & 0x00ff) << 8) | ((a & 0xff00) >> 8);
}

/** Converts a character representation of a hex to real value.
 *   @param c is the hex value in char format
 *   @return the value of the hex
 */
static uint8_t ctoh(char c)
{
    return (c & 0x10) ? /*0-9*/ c & 0xf : /*A-F, a-f*/ (c & 0xf) + 9;
}

/** Calculate checksum on a hex record
 *   @param data is the line of hex record
 *   @param size is the length of the data array
 *   @return 1 if the data provided is a valid hex record otherwise 0
 */
static uint8_t validate_checksum(hex_line_t *record)
{
    uint8_t result = 0, i = 0;

    for (; i < (record->byte_count + 5); i++) {
        result += record->buf[i];
    }

    return (result == 0);
}

static hex_line_t line = {0}, shadow_line = {0};
static uint32_t next_address_to_write = 0;
static uint8_t low_nibble = 0, idx = 0, record_processed = 0, load_unaligned_record = 0;

void reset_hex_parser(void)
{
    memset(line.buf, 0, sizeof(hex_line_t));
    memset(shadow_line.buf, 0, sizeof(hex_line_t));
    next_address_to_write = 0;
    low_nibble = 0;
    idx = 0;
    record_processed = 0;
    load_unaligned_record = 0;
}

hexfile_parse_status_t parse_hex_blob(const uint8_t *hex_blob, const uint32_t hex_blob_size, uint32_t *hex_parse_cnt, uint8_t *bin_buf, const uint32_t bin_buf_size, uint32_t *bin_buf_address, uint32_t *bin_buf_cnt)
{
    uint8_t *end = (uint8_t *)hex_blob + hex_blob_size;
    hexfile_parse_status_t status = HEX_PARSE_UNINIT;
    // reset the amount of data that is being return'd
    *bin_buf_cnt = (uint32_t)0;

    // we had an exit state where the address was unaligned to the previous record and data count.
    //  Need to pop the last record into the buffer before decoding anthing else since it was
    //  already decoded.
    if (load_unaligned_record) {
        // need some help...
        load_unaligned_record = 0;
        // move from line buffer back to input buffer
        memcpy((uint8_t *)bin_buf, (uint8_t *)line.data, line.byte_count);
        bin_buf += line.byte_count;
        *bin_buf_cnt = (uint32_t)(*bin_buf_cnt) + line.byte_count;
        // Store next address to write
        next_address_to_write = ((next_address_to_write & 0xffff0000) | line.address) + line.byte_count;
    }

    while (hex_blob != end) {
        switch ((uint8_t)(*hex_blob)) {
            // we've hit the end of an ascii line
            // junk we dont care about could also just run the validate_checksum on &line
            case '\r':
            case '\n':
                //ignore new lines
                break;

            // found start of a new record. reset state variables
            case ':':
                memset(line.buf, 0, sizeof(hex_line_t));
                low_nibble = 0;
                idx = 0;
                record_processed = 0;
                break;

            // decoding lines
            default:
                if (low_nibble) {
                    line.buf[idx] |= ctoh((uint8_t)(*hex_blob)) & 0xf;
                    if (++idx >= (line.byte_count + 5)) { //all data in
                        if (0 == validate_checksum(&line)) {
                            status = HEX_PARSE_CKSUM_FAIL;
                            goto hex_parser_exit;
                        } else {
                            if (!record_processed) {
                                record_processed = 1;
                                // address byteswap...
                                line.address = swap16(line.address);

                                switch (line.record_type) {
                                    case DATA_RECORD:
                                        // keeping a record of the last hex record
                                        memcpy(shadow_line.buf, line.buf, sizeof(hex_line_t));

                                        // verify this is a continous block of memory or need to exit and dump
                                        if (((next_address_to_write & 0xffff0000) | line.address) != next_address_to_write) {
                                            load_unaligned_record = 1;
                                            status = HEX_PARSE_UNALIGNED;
                                            goto hex_parser_exit;
                                        }

                                        // move from line buffer back to input buffer
                                        memcpy(bin_buf, line.data, line.byte_count);
                                        bin_buf += line.byte_count;
                                        *bin_buf_cnt = (uint32_t)(*bin_buf_cnt) + line.byte_count;
                                        // Save next address to write
                                        next_address_to_write = ((next_address_to_write & 0xffff0000) | line.address) + line.byte_count;
                                        break;

                                    case EOF_RECORD:
                                        status = HEX_PARSE_EOF;
                                        goto hex_parser_exit;

                                    case EXT_SEG_ADDR_RECORD:
                                        // Could have had data in the buffer so must exit and try to program
                                        //  before updating bin_buf_address with next_address_to_write
                                        memset(bin_buf, 0xff, (bin_buf_size - (uint32_t)(*bin_buf_cnt)));
                                        // figure the start address for the buffer before returning
                                        *bin_buf_address = next_address_to_write - (uint32_t)(*bin_buf_cnt);
                                        *hex_parse_cnt = (uint32_t)(hex_blob_size - (end - hex_blob));
                                        // update the address msb's
                                        next_address_to_write = (next_address_to_write & 0x00000000) | ((line.data[0] << 12) | (line.data[1] << 4));
                                        // Need to exit and program if buffer has been filled
                                        status = HEX_PARSE_UNALIGNED;
                                        return status;

                                    case EXT_LINEAR_ADDR_RECORD:
                                        // Could have had data in the buffer so must exit and try to program
                                        //  before updating bin_buf_address with next_address_to_write
                                        //  Good catch Gaute!!
                                        memset(bin_buf, 0xff, (bin_buf_size - (uint32_t)(*bin_buf_cnt)));
                                        // figure the start address for the buffer before returning
                                        *bin_buf_address = next_address_to_write - (uint32_t)(*bin_buf_cnt);
                                        *hex_parse_cnt = (uint32_t)(hex_blob_size - (end - hex_blob));
                                        // update the address msb's
                                        next_address_to_write = (next_address_to_write & 0x00000000) | ((line.data[0] << 24) | (line.data[1] << 16));
                                        // Need to exit and program if buffer has been filled
                                        status = HEX_PARSE_UNALIGNED;
                                        return status;

                                    default:
                                        break;
                                }
                            }
                        }
                    }
                } else {
                    if (idx < sizeof(hex_line_t)) {
                        line.buf[idx] = ctoh((uint8_t)(*hex_blob)) << 4;
                    }
                }

                low_nibble = !low_nibble;
                break;
        }

        hex_blob++;
    }

    // decoded an entire hex block - verify (cant do this hex_parse_cnt is figured below)
    //status = (hex_blob_size == (uint32_t)(*hex_parse_cnt)) ? HEX_PARSE_OK : HEX_PARSE_FAILURE;
    status = HEX_PARSE_OK;
hex_parser_exit:
    memset(bin_buf, 0xff, (bin_buf_size - (uint32_t)(*bin_buf_cnt)));
    // figure the start address for the buffer before returning
    *bin_buf_address = next_address_to_write - (uint32_t)(*bin_buf_cnt);
    *hex_parse_cnt = (uint32_t)(hex_blob_size - (end - hex_blob));
    return status;
}
