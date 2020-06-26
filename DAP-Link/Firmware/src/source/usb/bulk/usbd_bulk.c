/**
 * @file    usbd_bulk.c
 * @brief   DAPv2 BULK driver
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

#include "rl_usb.h"
#include "usb_for_lib.h"
#include "util.h"
#include "DAP_queue.h"

static U8 *ptrDataIn;
static U16 DataInReceLen;
static DAP_queue DAP_Cmd_queue;

static volatile uint8_t  USB_ResponseIdle;

void usbd_bulk_init(void)
{
    ptrDataIn     = USBD_Bulk_BulkOutBuf;
    DataInReceLen = 0;
    DAP_queue_init(&DAP_Cmd_queue);
    USB_ResponseIdle = 1;
}

/*
 *  USB Device Bulk In Endpoint Event Callback
 *    Parameters:      event: not used (just for compatibility)
 *    Return Value:    None
 */

void USBD_BULK_EP_BULKIN_Event(U32 event)
{
    uint8_t * sbuf = 0;
    int slen;
    if(DAP_queue_get_send_buf(&DAP_Cmd_queue, &sbuf, &slen)){
        USBD_WriteEP(usbd_bulk_ep_bulkin | 0x80, sbuf, slen);
    } else {
        USB_ResponseIdle = 1;
    }
}


/*
 *  USB Device Bulk Out Endpoint Event Callback
 *    Parameters:      event: not used (just for compatibility)
 *    Return Value:    None
 */

void USBD_BULK_EP_BULKOUT_Event(U32 event)
{
    U16 bytes_rece;
    uint8_t * rbuf;

    bytes_rece      = USBD_ReadEP(usbd_bulk_ep_bulkout, ptrDataIn, USBD_Bulk_BulkBufSize - DataInReceLen);
    ptrDataIn      += bytes_rece;
    DataInReceLen  += bytes_rece;

    if ((DataInReceLen >= USBD_Bulk_BulkBufSize) ||
            (bytes_rece    <  usbd_bulk_maxpacketsize[USBD_HighSpeed])) {
        if (DAP_queue_execute_buf(&DAP_Cmd_queue, USBD_Bulk_BulkOutBuf, DataInReceLen, &rbuf)) {
            //Trigger the BULKIn for the reply
            if (USB_ResponseIdle) {
                USBD_BULK_EP_BULKIN_Event(0);
                USB_ResponseIdle = 0;
            }
        }
        //revert the input pointers
        DataInReceLen = 0;
        ptrDataIn     = USBD_Bulk_BulkOutBuf;
    }
}


/*
 *  USB Device Bulk In/Out Endpoint Event Callback
 *    Parameters:      event: USB Device Event
 *                       USBD_EVT_OUT: Output Event
 *                       USBD_EVT_IN:  Input Event
 *    Return Value:    None
 */

void USBD_BULK_EP_BULK_Event(U32 event)
{
    if (event & USBD_EVT_OUT) {
        USBD_BULK_EP_BULKOUT_Event(0);
    }

    if (event & USBD_EVT_IN) {
        USBD_BULK_EP_BULKIN_Event(0);
    }
}
