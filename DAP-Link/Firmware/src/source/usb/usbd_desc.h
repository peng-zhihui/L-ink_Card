/**
 * @file    usbd_desc.h
 * @brief   USB Device Descriptor header
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

#ifndef __USBD_DESC_H__
#define __USBD_DESC_H__

#define WBVAL(x)                          (x & 0xFF),((x >> 8) & 0xFF)
#define B3VAL(x)                          (x & 0xFF),((x >> 8) & 0xFF),((x >> 16) & 0xFF)
#define USB_DEVICE_DESC_SIZE              (sizeof(USB_DEVICE_DESCRIPTOR))
#define USB_DEVICE_QUALI_SIZE             (sizeof(USB_DEVICE_QUALIFIER_DESCRIPTOR))
#define USB_CONFIGUARTION_DESC_SIZE       (sizeof(USB_CONFIGURATION_DESCRIPTOR))
#define USB_INTERFACE_ASSOC_DESC_SIZE     (sizeof(USB_INTERFACE_ASSOCIATION_DESCRIPTOR))
#define USB_INTERFACE_DESC_SIZE           (sizeof(USB_INTERFACE_DESCRIPTOR))
#define USB_ENDPOINT_DESC_SIZE            (sizeof(USB_ENDPOINT_DESCRIPTOR))
#define USB_BOS_DESC_SIZE                 (sizeof(USB_BINARY_OBJECT_STORE_DESCRIPTOR))
#define USB_HID_DESC_SIZE                 (sizeof(HID_DESCRIPTOR))
#define USB_HID_REPORT_DESC_SIZE          (sizeof(USBD_HID_ReportDescriptor))

#endif  /* __USBD_DESC_H__ */
