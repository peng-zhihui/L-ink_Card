/**
 * @file    DAP_queue.h
 * @brief   DAP processing queue
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

#ifndef DAP_QUEUE_H
#define DAP_QUEUE_H

#include "usb_def.h"
#include "DAP_config.h"
#include "DAP.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FREE_COUNT_INIT          (DAP_PACKET_COUNT)
#define SEND_COUNT_INIT          0

typedef struct _DAP_queue {
    uint8_t     USB_Request [DAP_PACKET_COUNT][DAP_PACKET_SIZE];  // Request  Buffer
    uint16_t    resp_size[DAP_PACKET_COUNT]; //track the return response size
    uint32_t    free_count;
    uint32_t    send_count;
    uint32_t    recv_idx;
    uint32_t    send_idx;
} DAP_queue;

void DAP_queue_init(DAP_queue * queue);

/*
 *  Get the a buffer from the DAP_queue where the response to the request is stored
 *    Parameters:      queue - DAP queue, buf = return the buffer location, len = return the len of the response
 *    Return Value:    TRUE - Success, FALSE - Error
 */
BOOL DAP_queue_get_send_buf(DAP_queue * queue, uint8_t ** buf, int * len);

/*
 *  Execute a request and store result to the DAP_queue
 *    Parameters:      queue - DAP queue, reqbuf = buffer with DAP request, len = of the request buffer, retbuf = buffer to peek on the result of the DAP operation
 *    Return Value:    TRUE - Success, FALSE - Error
 */
BOOL DAP_queue_execute_buf(DAP_queue * queue, const uint8_t *reqbuf, int len, uint8_t ** retbuf);

#ifdef __cplusplus
}
#endif

#endif
