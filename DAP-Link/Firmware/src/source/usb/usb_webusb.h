/**
 * @file    usb_webusb.h
 * @brief   WebUSB driver header
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

#ifndef __USB_WEBUSB_H__
#define __USB_WEBUSB_H__

/* WebUSB Descriptor Types */
#define WEBUSB_DESCRIPTOR_SET_HEADER_TYPE       0x00
#define WEBUSB_CONFIGURATION_SUBSET_HEADER_TYPE 0x01
#define WEBUSB_FUNCTION_SUBSET_HEADER_TYPE      0x02
#define WEBUSB_URL_TYPE                         0x03

/* WebUSB Platform Capability Descriptor */
typedef __packed struct _WEBUSB_PLATFORM_CAPABILITY_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U8  bDevCapabilityType;
    U8  bReserved;
    U8  platformCapabilityUUID[16];
    U16 bcdVersion;
    U8  bVendorCode;
    U8  iLandingPage;
} WEBUSB_PLATFORM_CAPABILITY_DESCRIPTOR;

typedef __packed struct _WEBUSB_URL_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U8  bScheme;
    char URL[];
} WEBUSB_URL_DESCRIPTOR;

/* WebUSB Request Codes */
#define WEBUSB_REQUEST_GET_URL                  0x02

/* bScheme in URL descriptor */
#define WEBUSB_URL_SCHEME_HTTP                  0x00
#define WEBUSB_URL_SCHEME_HTTPS                 0x01

/* WebUSB Descriptor sizes */
#define WEBUSB_DESCRIPTOR_SET_HEADER_SIZE       5
#define WEBUSB_CONFIGURATION_SUBSET_HEADER_SIZE 4
#define WEBUSB_FUNCTION_SUBSET_HEADER_SIZE      3

#endif  /* __USB_WEBUSB_H__ */
