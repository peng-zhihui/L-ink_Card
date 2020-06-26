/**
 * @file    usb_winusb.h
 * @brief   WinUSB driver header
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

#ifndef __USB_WINUSB_H__
#define __USB_WINUSB_H__

/* WinUSB Microsoft OS 2.0 Descriptor Types */
#define WINUSB_SET_HEADER_DESCRIPTOR_TYPE           0x00
#define WINUSB_SUBSET_HEADER_CONFIGURATION_TYPE     0x01
#define WINUSB_SUBSET_HEADER_FUNCTION_TYPE          0x02
#define WINUSB_FEATURE_COMPATIBLE_ID_TYPE           0x03
#define WINUSB_FEATURE_REG_PROPERTY_TYPE            0x04
#define WINUSB_FEATURE_MIN_RESUME_TIME_TYPE         0x05
#define WINUSB_FEATURE_MODEL_ID_TYPE                0x06
#define WINUSB_FEATURE_CCGP_DEVICE_TYPE             0x07


#define WINUSB_PROP_DATA_TYPE_REG_SZ                0x01
#define WINUSB_PROP_DATA_TYPE_REG_MULTI_SZ          0x07

/* WinUSB Microsoft OS 2.0 descriptor Platform Capability Descriptor */
typedef __packed struct _WINUSB_PLATFORM_CAPABILITY_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U8  bDevCapabilityType;
    U8  bReserved;
    U8  platformCapabilityUUID[16];
    U32 dwWindowsVersion;
    U16 wDescriptorSetTotalLength;
    U8  bVendorCode;
    U8  bAltEnumCode;
} WINUSB_PLATFORM_CAPABILITY_DESCRIPTOR;

/* WinUSB Microsoft OS 2.0 descriptor set header */
typedef __packed struct _WINUSB_DESCRIPTOR_SET_HEADER {
    U16 wLength;
    U16 wDescriptorType;
    U32 dwWindowsVersion;
    U16 wTotalLength;
} WINUSB_DESCRIPTOR_SET_HEADER;

/* WinUSB Microsoft OS 2.0 descriptor request codes */
#define WINUSB_REQUEST_GET_DESCRIPTOR_SET          0x07
#define WINUSB_REQUEST_SET_ALT_ENUM                0x08

/* WinUSB Microsoft OS 2.0 descriptor sizes */
#define WINUSB_DESCRIPTOR_SET_HEADER_SIZE          10
#define WINUSB_FUNCTION_SUBSET_HEADER_SIZE         8
#define WINUSB_FEATURE_COMPATIBLE_ID_SIZE          20
#endif // __USB_WINUSB_H__
