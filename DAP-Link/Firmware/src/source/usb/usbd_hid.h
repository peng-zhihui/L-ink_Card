/**
 * @file    usbd_hid.h
 * @brief   USB Device HID header
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

#ifndef __USBD_HID_H__
#define __USBD_HID_H__


/*--------------------------- Global constants -------------------------------*/

/* USB HID Class API enumerated constants                                     */
enum {
    USBD_HID_REQ_EP_CTRL = 0,             /* Request from control endpoint      */
    USBD_HID_REQ_EP_INT,                  /* Request from interrupt endpoint    */
    USBD_HID_REQ_PERIOD_UPDATE            /* Request from periodic update       */
};


/*--------------------------- Event handling routines ------------------------*/

extern void USBD_HID_Configure_Event(void);
extern void USBD_HID_SOF_Event(void);

extern void USBD_HID_EP_INTIN_Event(U32 event);
extern void USBD_HID_EP_INTOUT_Event(U32 event);
extern void USBD_HID_EP_INT_Event(U32 event);
#ifdef __RTX
extern void USBD_RTX_HID_EP_INTIN_Event(void);
extern void USBD_RTX_HID_EP_INTOUT_Event(void);
extern void USBD_RTX_HID_EP_INT_Event(void);
#endif

#endif  /* __USBD_HID_H__ */
