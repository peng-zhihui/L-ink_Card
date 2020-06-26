/**
 * @file    usb_def.h
 * @brief   USB defines
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

#ifndef __USB_DEF_H__
#define __USB_DEF_H__

#pragma anon_unions

#include <stdint.h>
#include <stddef.h>

#ifndef NULL
 #ifdef __cplusplus
  #define NULL          0
 #else
  #define NULL          ((void *) 0)
 #endif
#endif

#ifndef EOF
 #define EOF            (-1)
#endif

#ifndef __size_t
 #define __size_t       1
 typedef unsigned int   size_t;
#endif

typedef signed char     S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
typedef long long       S64;
typedef unsigned long long U64;
typedef unsigned char   BIT;
typedef unsigned int    BOOL;

#ifndef __TRUE
 #define __TRUE         1
#endif
#ifndef __FALSE
 #define __FALSE        0
#endif


/* bmRequestType.Dir */
#define REQUEST_HOST_TO_DEVICE     0
#define REQUEST_DEVICE_TO_HOST     1

/* bmRequestType.Type */
#define REQUEST_STANDARD           0
#define REQUEST_CLASS              1
#define REQUEST_VENDOR             2
#define REQUEST_RESERVED           3

/* bmRequestType.Recipient */
#define REQUEST_TO_DEVICE          0
#define REQUEST_TO_INTERFACE       1
#define REQUEST_TO_ENDPOINT        2
#define REQUEST_TO_OTHER           3

/* bmRequestType Definition */
typedef __packed struct _REQUEST_TYPE {
    U8 Recipient : 5;                     /* D4..0: Recipient */
    U8 Type      : 2;                     /* D6..5: Type */
    U8 Dir       : 1;                     /* D7:    Data Phase Txsfer Direction */
} REQUEST_TYPE;

/* USB Standard Request Codes */
#define USB_REQUEST_GET_STATUS                 0
#define USB_REQUEST_CLEAR_FEATURE              1
#define USB_REQUEST_SET_FEATURE                3
#define USB_REQUEST_SET_ADDRESS                5
#define USB_REQUEST_GET_DESCRIPTOR             6
#define USB_REQUEST_SET_DESCRIPTOR             7
#define USB_REQUEST_GET_CONFIGURATION          8
#define USB_REQUEST_SET_CONFIGURATION          9
#define USB_REQUEST_GET_INTERFACE              10
#define USB_REQUEST_SET_INTERFACE              11
#define USB_REQUEST_SYNC_FRAME                 12

/* USB GET_STATUS Bit Values */
#define USB_GETSTATUS_SELF_POWERED             0x01
#define USB_GETSTATUS_REMOTE_WAKEUP            0x02
#define USB_GETSTATUS_ENDPOINT_STALL           0x01

/* USB Standard Feature selectors */
#define USB_FEATURE_ENDPOINT_STALL             0
#define USB_FEATURE_REMOTE_WAKEUP              1

/* USB Default Control Pipe Setup Packet */
typedef __packed struct _USB_SETUP_PACKET {
    REQUEST_TYPE bmRequestType;           /* bmRequestType */
    U8  bRequest;                         /* bRequest */
    __packed union {
        U16        wValue;                  /* wValue */
        __packed struct {
            U8         wValueL;
            U8         wValueH;
        };
    };
    __packed union {
        U16        wIndex;                  /* wIndex */
        __packed struct {
            U8         wIndexL;
            U8         wIndexH;
        };
    };
    U16          wLength;                 /* wLength */
} USB_SETUP_PACKET;


/* USB Descriptor Types */
#define USB_DEVICE_DESCRIPTOR_TYPE                  1
#define USB_CONFIGURATION_DESCRIPTOR_TYPE           2
#define USB_STRING_DESCRIPTOR_TYPE                  3
#define USB_INTERFACE_DESCRIPTOR_TYPE               4
#define USB_ENDPOINT_DESCRIPTOR_TYPE                5
#define USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE        6
#define USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE      7
#define USB_INTERFACE_POWER_DESCRIPTOR_TYPE         8
#define USB_OTG_DESCRIPTOR_TYPE                     9
#define USB_DEBUG_DESCRIPTOR_TYPE                  10
#define USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE  11
#define USB_BINARY_OBJECT_STORE_DESCRIPTOR_TYPE    15
#define USB_DEVICE_CAPABILITY_DESCRIPTOR_TYPE      16

/* USB Device Classes */
#define USB_DEVICE_CLASS_RESERVED              0x00
#define USB_DEVICE_CLASS_AUDIO                 0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS        0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE       0x03
#define USB_DEVICE_CLASS_MONITOR               0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE    0x05
#define USB_DEVICE_CLASS_POWER                 0x06
#define USB_DEVICE_CLASS_PRINTER               0x07
#define USB_DEVICE_CLASS_STORAGE               0x08
#define USB_DEVICE_CLASS_HUB                   0x09
#define USB_DEVICE_CLASS_MISCELLANEOUS         0xEF
#define USB_DEVICE_CLASS_APPLICATION_SPECIFIC  0xFE
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC       0xFF

/* bmAttributes in Configuration Descriptor */
#define USB_CONFIG_POWERED_MASK                0x40
#define USB_CONFIG_BUS_POWERED                 0x80
#define USB_CONFIG_SELF_POWERED                0xC0
#define USB_CONFIG_REMOTE_WAKEUP               0x20

/* bMaxPower in Configuration Descriptor */
#define USB_CONFIG_POWER_MA(mA)                ((mA)/2)

/* bEndpointAddress in Endpoint Descriptor */
#define USB_ENDPOINT_DIRECTION_MASK            0x80
#define USB_ENDPOINT_OUT(addr)                 ((addr) | 0x00)
#define USB_ENDPOINT_IN(addr)                  ((addr) | 0x80)

/* bmAttributes in Endpoint Descriptor */
#define USB_ENDPOINT_TYPE_MASK                 0x03
#define USB_ENDPOINT_TYPE_CONTROL              0x00
#define USB_ENDPOINT_TYPE_ISOCHRONOUS          0x01
#define USB_ENDPOINT_TYPE_BULK                 0x02
#define USB_ENDPOINT_TYPE_INTERRUPT            0x03
#define USB_ENDPOINT_SYNC_MASK                 0x0C
#define USB_ENDPOINT_SYNC_NO_SYNCHRONIZATION   0x00
#define USB_ENDPOINT_SYNC_ASYNCHRONOUS         0x04
#define USB_ENDPOINT_SYNC_ADAPTIVE             0x08
#define USB_ENDPOINT_SYNC_SYNCHRONOUS          0x0C
#define USB_ENDPOINT_USAGE_MASK                0x30
#define USB_ENDPOINT_USAGE_DATA                0x00
#define USB_ENDPOINT_USAGE_FEEDBACK            0x10
#define USB_ENDPOINT_USAGE_IMPLICIT_FEEDBACK   0x20
#define USB_ENDPOINT_USAGE_RESERVED            0x30

/* bDevCapabilityType in Device Capability Descriptor */
#define USB_DEVICE_CAPABILITY_WIRELESS_USB                  1
#define USB_DEVICE_CAPABILITY_USB_2_0_EXTENSION             2
#define USB_DEVICE_CAPABILITY_SUPERSPEED_USB                3
#define USB_DEVICE_CAPABILITY_CONTAINER_ID                  4
#define USB_DEVICE_CAPABILITY_PLATFORM                      5
#define USB_DEVICE_CAPABILITY_POWER_DELIVERY_CAPABILITY     6
#define USB_DEVICE_CAPABILITY_BATTERY_INFO_CAPABILITY       7
#define USB_DEVICE_CAPABILITY_PD_CONSUMER_PORT_CAPABILITY   8
#define USB_DEVICE_CAPABILITY_PD_PROVIDER_PORT_CAPABILITY   9
#define USB_DEVICE_CAPABILITY_SUPERSPEED_PLUS               10
#define USB_DEVICE_CAPABILITY_PRECISION_TIME_MEASUREMENT    11
#define USB_DEVICE_CAPABILITY_WIRELESS_USB_EXT              12

/* USB Standard Device Descriptor */
typedef __packed struct _USB_DEVICE_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U16 bcdUSB;
    U8  bDeviceClass;
    U8  bDeviceSubClass;
    U8  bDeviceProtocol;
    U8  bMaxPacketSize0;
    U16 idVendor;
    U16 idProduct;
    U16 bcdDevice;
    U8  iManufacturer;
    U8  iProduct;
    U8  iSerialNumber;
    U8  bNumConfigurations;
} USB_DEVICE_DESCRIPTOR;

/* USB 2.0 Device Qualifier Descriptor */
typedef __packed struct _USB_DEVICE_QUALIFIER_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U16 bcdUSB;
    U8  bDeviceClass;
    U8  bDeviceSubClass;
    U8  bDeviceProtocol;
    U8  bMaxPacketSize0;
    U8  bNumConfigurations;
    U8  bReserved;
} USB_DEVICE_QUALIFIER_DESCRIPTOR;

/* USB Standard Configuration Descriptor */
typedef __packed struct _USB_CONFIGURATION_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U16 wTotalLength;
    U8  bNumInterfaces;
    U8  bConfigurationValue;
    U8  iConfiguration;
    U8  bmAttributes;
    U8  bMaxPower;
} USB_CONFIGURATION_DESCRIPTOR;

/* USB Standard Interface Descriptor */
typedef __packed struct _USB_INTERFACE_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U8  bInterfaceNumber;
    U8  bAlternateSetting;
    U8  bNumEndpoints;
    U8  bInterfaceClass;
    U8  bInterfaceSubClass;
    U8  bInterfaceProtocol;
    U8  iInterface;
} USB_INTERFACE_DESCRIPTOR;

/* USB Standard Endpoint Descriptor */
typedef __packed struct _USB_ENDPOINT_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U8  bEndpointAddress;
    U8  bmAttributes;
    U16 wMaxPacketSize;
    U8  bInterval;
} USB_ENDPOINT_DESCRIPTOR;

/* USB String Descriptor */
typedef __packed struct _USB_STRING_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U16 bString/*[]*/;
} USB_STRING_DESCRIPTOR;

/* USB Common Descriptor */
typedef __packed struct _USB_COMMON_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
} USB_COMMON_DESCRIPTOR;

/* USB Interface Association Descriptor */
typedef __packed struct _USB_INTERFACE_ASSOCIATION_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U8  bFirstInterface;
    U8  bInterfaceCount;
    U8  bFunctionClass;
    U8  bFunctionSubclass;
    U8  bFunctionProtocol;
    U8  iFunction;
} USB_INTERFACE_ASSOCIATION_DESCRIPTOR;

/* USB Binary Object Store Descriptor */
typedef __packed struct _USB_BINARY_OBJECT_STORE_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U16 wTotalLength;
    U8  bNumDeviceCaps;
} USB_BINARY_OBJECT_STORE_DESCRIPTOR;

/* Union Functional Descriptor */ 
typedef __packed struct _UNION_FUNCTIONAL_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U8  bDescriptorSubtype;
    U8  bMasterInterface;
    U8  bSlaveInterface0;
} UNION_FUNCTIONAL_DESCRIPTOR;

typedef __packed struct _WINUSB_FUNCTION_SUBSET_HEADER {
    U16 wLength;
    U16 wDescriptorType; 
    U8  bFirstInterface;
    U8  bReserved;
} WINUSB_FUNCTION_SUBSET_HEADER;

/* USB Device Capability Descriptor */
typedef __packed struct _USB_DEVICE_CAPABILITY_DESCRIPTOR {
    U8  bLength;
    U8  bDescriptorType;
    U8  bDevCapabilityType;
} USB_DEVICE_CAPABILITY_DESCRIPTOR;

#endif  /* __USB_DEF_H__ */
