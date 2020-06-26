/**
 * @file    error.h
 * @brief   collection of known errors and accessor for the friendly string
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

#ifndef ERROR_H
#define ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

// Keep in sync with the lists error_message and error_type
typedef enum {
    /* Shared errors */
    ERROR_SUCCESS = 0,
    ERROR_FAILURE,
    ERROR_INTERNAL,

    /* VFS user errors */
    ERROR_ERROR_DURING_TRANSFER,
    ERROR_TRANSFER_TIMEOUT,
    ERROR_FILE_BOUNDS,
    ERROR_OOO_SECTOR,

    /* Target flash errors */
    ERROR_RESET,
    ERROR_ALGO_DL,
    ERROR_ALGO_MISSING,
    ERROR_ALGO_DATA_SEQ,
    ERROR_INIT,
    ERROR_UNINIT,
    ERROR_SECURITY_BITS,
    ERROR_UNLOCK,
    ERROR_ERASE_SECTOR,
    ERROR_ERASE_ALL,
    ERROR_WRITE,
    ERROR_WRITE_VERIFY,

    /* File stream errors */
    ERROR_SUCCESS_DONE,
    ERROR_SUCCESS_DONE_OR_CONTINUE,
    ERROR_HEX_CKSUM,
    ERROR_HEX_PARSER,
    ERROR_HEX_PROGRAM,
    ERROR_HEX_INVALID_ADDRESS,
    ERROR_HEX_INVALID_APP_OFFSET,

    /* Flash decoder error */
    ERROR_FD_BL_UPDT_ADDR_WRONG,
    ERROR_FD_INTF_UPDT_ADDR_WRONG,
    ERROR_FD_UNSUPPORTED_UPDATE,

    /* Flash IAP interface */
    ERROR_IAP_INIT,
    ERROR_IAP_UNINIT,
    ERROR_IAP_WRITE,
    ERROR_IAP_ERASE_SECTOR,
    ERROR_IAP_ERASE_ALL,
    ERROR_IAP_OUT_OF_BOUNDS,
    ERROR_IAP_UPDT_NOT_SUPPORTED,
    ERROR_IAP_UPDT_INCOMPLETE,
    ERROR_IAP_NO_INTERCEPT,
    ERROR_BL_UPDT_BAD_CRC,

    // Add new values here

    ERROR_COUNT
} error_t;

const char *error_get_string(error_t error);

typedef unsigned char error_type_t;

#define ERROR_TYPE_INTERNAL 0x1
#define ERROR_TYPE_TRANSIENT 0x2
#define ERROR_TYPE_USER 0x4
#define ERROR_TYPE_TARGET 0x8
#define ERROR_TYPE_INTERFACE 0x10
// If you add another error type:
// 1. update error_type_names, used by read_file_fail_txt()
// 2. update ERROR_TYPE_MASK
// 3. make sure that error type bits still fit inside of error_type_t
#define ERROR_TYPE_MASK 0x1F

error_type_t error_get_type(error_t error);

#ifdef __cplusplus
}
#endif

#endif
