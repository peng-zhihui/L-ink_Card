/**
 * @file    DAP_queue.c
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

#include <string.h>
#include "DAP_queue.h"
void DAP_queue_init(DAP_queue * queue)
{
    queue->recv_idx = 0;
    queue->send_idx = 0;
    queue->free_count = FREE_COUNT_INIT;
    queue->send_count = SEND_COUNT_INIT;
}

/*
 *  Get the a buffer from the DAP_queue where the response to the request is stored
 *    Parameters:      queue - DAP queue, buf = return the buffer location, len = return the len of the response
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL DAP_queue_get_send_buf(DAP_queue * queue, uint8_t ** buf, int * len)
{
    if (queue->send_count) {
        queue->send_count--;
        *buf = queue->USB_Request[queue->send_idx];
        *len = queue->resp_size[queue->send_idx];
        queue->send_idx = (queue->send_idx + 1) % DAP_PACKET_COUNT;
        queue->free_count++;
        return (__TRUE);
    }
    return (__FALSE);
}

/*
 *  Execute a request and store result to the DAP_queue
 *    Parameters:      queue - DAP queue, reqbuf = buffer with DAP request, len = of the request buffer, retbuf = buffer to peek on the result of the DAP operation
 *    Return Value:    TRUE - Success, FALSE - Error
 */


BOOL DAP_queue_execute_buf(DAP_queue * queue, const uint8_t *reqbuf, int len, uint8_t ** retbuf)
{
    uint32_t rsize;
    if (queue->free_count > 0) {
        if (len > DAP_PACKET_SIZE) {
            len = DAP_PACKET_SIZE;
        }
        queue->free_count--;
        memcpy(queue->USB_Request[queue->recv_idx], reqbuf, len);
        rsize = DAP_ExecuteCommand(reqbuf, queue->USB_Request[queue->recv_idx]);
        queue->resp_size[queue->recv_idx] = rsize & 0xFFFF; //get the response size
        *retbuf = queue->USB_Request[queue->recv_idx];
        queue->recv_idx = (queue->recv_idx + 1) % DAP_PACKET_COUNT;
        queue->send_count++;
        return (__TRUE);
    }
    return (__FALSE);
}
