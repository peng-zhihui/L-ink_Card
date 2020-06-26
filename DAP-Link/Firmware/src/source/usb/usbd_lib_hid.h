/**
 * @file    usbd_lib_hid.h
 * @brief   USB Device HID Library header
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

#ifndef __USBD_LIB_HID_H__
#define __USBD_LIB_HID_H__


/*--------------------------- USB Requests -----------------------------------*/

extern BOOL USBD_HID_GetReport(void);
extern BOOL USBD_HID_SetReport(void);
extern BOOL USBD_HID_GetIdle(void);
extern BOOL USBD_HID_SetIdle(void);
extern BOOL USBD_HID_GetProtocol(void);
extern BOOL USBD_HID_SetProtocol(void);


#endif  /* __USBD_LIB_HID_H__ */
