/**
 * @file    usbd_user_hid.c
 * @brief   HID driver for CMSIS-DAP packet processing
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
#include "rl_usb.h"
#include "usb.h"
#define __NO_USB_LIB_C
#include "usb_config.c"
#include "DAP_config.h"
#include "DAP.h"
#include "util.h"
#include "DAP_queue.h"
#include "main.h"


#if (USBD_HID_OUTREPORT_MAX_SZ > DAP_PACKET_SIZE)
#error "USB HID Output Report Size must be less than DAP Packet Size"
#endif
#if (USBD_HID_INREPORT_MAX_SZ > DAP_PACKET_SIZE)
#error "USB HID Input Report Size must be less than DAP Packet Size"
#endif

static volatile uint8_t  USB_ResponseIdle;
static DAP_queue DAP_Cmd_queue;

void hid_send_packet()
{
    uint8_t * sbuf;
    int slen;
    if (DAP_queue_get_send_buf(&DAP_Cmd_queue, &sbuf, &slen)) {
        if (slen > USBD_HID_OUTREPORT_MAX_SZ){
            util_assert(0);
        }else {
            usbd_hid_get_report_trigger(0, sbuf, USBD_HID_OUTREPORT_MAX_SZ);
        }
    }
}

// USB HID Callback: when system initializes
void usbd_hid_init(void)
{
    USB_ResponseIdle = 1;
    DAP_queue_init(&DAP_Cmd_queue);
}

// USB HID Callback: when data needs to be prepared for the host
int usbd_hid_get_report(U8 rtype, U8 rid, U8 *buf, U8 req)
{
    uint8_t * sbuf;
    int slen;
    switch (rtype) {
        case HID_REPORT_INPUT:
            switch (req) {
                case USBD_HID_REQ_PERIOD_UPDATE:
                    break;

                case USBD_HID_REQ_EP_CTRL:
                case USBD_HID_REQ_EP_INT:
                    if (DAP_queue_get_send_buf(&DAP_Cmd_queue, &sbuf, &slen)) {
                        if (slen > USBD_HID_OUTREPORT_MAX_SZ){
                            util_assert(0);
                        }else {
                            memcpy(buf, sbuf, slen);
                            return (USBD_HID_OUTREPORT_MAX_SZ);
                        }
                    } else if (req == USBD_HID_REQ_EP_INT) {
                        USB_ResponseIdle = 1;
                    }
                    break;
            }

            break;

        case HID_REPORT_FEATURE:
            break;
    }

    return (0);
}

// USB HID override function return 1 if the activity is trivial or response is null
__attribute__((weak))
uint8_t usbd_hid_no_activity(U8 *buf)
{
    return 0;
}

// USB HID Callback: when data is received from the host
void usbd_hid_set_report(U8 rtype, U8 rid, U8 *buf, int len, U8 req)
{
    uint8_t * rbuf;
    main_led_state_t led_next_state = MAIN_LED_FLASH;
    switch (rtype) {
        case HID_REPORT_OUTPUT:
            if (len == 0) {
                break;
            }

            if (buf[0] == ID_DAP_TransferAbort) {
                DAP_TransferAbort = 1;
                break;
            }

            // execute and store to DAP_queue
            if (DAP_queue_execute_buf(&DAP_Cmd_queue, buf, len, &rbuf)) {
                if(usbd_hid_no_activity(rbuf) == 1){
                    //revert HID LED to default if the response is null
                    led_next_state = MAIN_LED_DEF;
                }
                if (USB_ResponseIdle) {
                    hid_send_packet();
                    USB_ResponseIdle = 0;
                }
            } else {
                util_assert(0);
            }

            main_blink_hid_led(led_next_state);

            break;

        case HID_REPORT_FEATURE:
            break;
    }
}


