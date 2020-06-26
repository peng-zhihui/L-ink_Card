/**
 * @file    usb_config.h
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
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

#include "util.h"

// <e> USB Device
//   <i> Enable the USB Device functionality
#define USBD_ENABLE                 1
#define USBD_RTX_CORE_STACK         0
#define USBD_RTX_DEVICE_STACK       0
#define USBD_RTX_ENDPOINT0_STACK    0

//   <o0.0> High-speed
//     <i> Enable high-speed functionality (if device supports it)
#define USBD_HS_ENABLE              0
#if (defined(WEBUSB_INTERFACE) || defined(WINUSB_INTERFACE) || defined(BULK_ENDPOINT))
#define USBD_BOS_ENABLE             1
#else
#define USBD_BOS_ENABLE             0
#endif
//   <h> Device Settings
//     <i> These settings affect Device Descriptor
//     <o0> Power
//       <i> Default Power Setting
//       <0=> Bus-powered
//       <1=> Self-powered
//     <o1> Max Endpoint 0 Packet Size
//       <i> Maximum packet size for endpoint zero (bMaxPacketSize0)
//       <8=> 8 Bytes <16=> 16 Bytes <32=> 32 Bytes <64=> 64 Bytes
//     <o2.0..15> Vendor ID <0x0000-0xFFFF>
//       <i> Vendor ID assigned by the USB-IF (idVendor)
//     <o3.0..15> Product ID <0x0000-0xFFFF>
//       <i> Product ID assigned by the manufacturer (idProduct)
//     <o4.0..15> Device Release Number <0x0000-0xFFFF>
//       <i> Device release number in binary-coded decimal (bcdDevice)
//   </h>
#define USBD_POWER                  0
#define USBD_MAX_PACKET0            64
#define USBD_DEVDESC_IDVENDOR       0x0D28
#define USBD_DEVDESC_IDPRODUCT      0x0204
#define USBD_DEVDESC_BCDDEVICE      0x1000 //was 0x0100

//   <h> Configuration Settings
//     <i> These settings affect Configuration Descriptor
//     <o0.5> Remote Wakeup
//       <i> Configuration support for remote wakeup (D5: of bmAttributes)
//     <o1.0..7> Maximum Power Consumption (in mA) <0-510><#/2>
//       <i> Maximum power consumption of the USB device
//       <i> from the bus in this specific configuration
//       <i> when the device is fully operational (bMaxPower)
//   </h>
#define USBD_CFGDESC_BMATTRIBUTES   0x80
#define USBD_CFGDESC_BMAXPOWER      0xFA

//   <h> String Settings
//     <i> These settings affect String Descriptor
//     <o0.0..15> Language ID <0x0000-0xFCFF>
//       <i> English (United States) = 0x0409
//     <s0.126> Manufacturer String
//       <i> String descriptor describing manufacturer
//     <s1.126> Product String
//       <i> String descriptor describing product
//     <e1.0> Serial Number
//       <i> Enable serial number string
//       <i> If disabled serial number string will not be assigned to the USB Device
//       <s2.126> Serial Number String
//         <i> String descriptor describing device's serial number
//     </e>
//   </h>
#define USBD_STRDESC_LANGID         0x0409
#define USBD_STRDESC_MAN            L"ARM"
#ifndef USB_PROD_STR
#define USBD_STRDESC_PROD           L"DAPLink CMSIS-DAP"
#else
#define _TOWIDE(x)                   L ## #x
#define TOWIDE(x)                   _TOWIDE(x)
#define USBD_STRDESC_PROD           TOWIDE(USB_PROD_STR)
#endif
#define USBD_STRDESC_SER_ENABLE     1
#define USBD_STRDESC_SER            L"0001A0000000"

//   <e0> Class Support
//     <i> Enables USB Device Class specific Requests
#define USBD_CLASS_ENABLE           1

//     <e0.0> Human Interface Device (HID)
//       <i> Enable class support for Human Interface Device (HID)
//       <h> Interrupt Endpoint Settings
//         <o1.0..4> Interrupt In Endpoint Number                  <1=>   1 <2=>   2 <3=>   3
//                                                 <4=>   4        <5=>   5 <6=>   6 <7=>   7
//                                                 <8=>   8        <9=>   9 <10=> 10 <11=> 11
//                                                 <12=>  12       <13=> 13 <14=> 14 <15=> 15
//         <o2.0..4> Interrupt Out Endpoint Number <0=>   Not used <1=>   1 <2=>   2 <3=>   3
//                                                 <4=>   4        <5=>   5 <6=>   6 <7=>   7
//                                                 <8=>   8        <9=>   9 <10=> 10 <11=> 11
//                                                 <12=>  12       <13=> 13 <14=> 14 <15=> 15
//           <i> If interrupt out endpoint is not used select "Not used"
//         <h> Endpoint Settings
//           <o3.0..7> Maximum Endpoint Packet Size (in bytes) <0-64>
//           <o4.0..7> Endpoint polling Interval (in ms) <1-255>
//           <e5> High-speed
//             <i> If high-speed is enabled set endpoint settings for it
//             <o6.0..10> Maximum Endpoint Packet Size (in bytes) <0-1024>
//             <o6.11..12> Additional transactions per microframe <0=> None <1=> 1 additional <2=> 2 additional
//             <o7.0..7> Endpoint polling Interval (in ms) <1=>      1 <2=>      2 <3=>      4 <4=>      8
//                                                         <5=>     16 <6=>     32 <7=>     64 <8=>    128
//                                                         <9=>    256 <10=>   512 <11=>  1024 <12=>  2048
//                                                         <13=>  4096 <14=>  8192 <15=> 16384 <16=> 32768
//           </e>
//         </h>
//       </h>
//       <h> Human Interface Device Settings
//         <i> Device specific settings
//         <s0.126> HID Interface String
//         <o8.0..4> Number of Input Reports <1-32>
//         <o9.0..4> Number of Output Reports <1-32>
//         <o10.0..15> Maximum Input Report Size (in bytes) <1-65535>
//         <o11.0..15> Maximum Output Report Size (in bytes) <1-65535>
//         <o12.0..15> Maximum Feature Report Size (in bytes) <1-65535>
//       </h>
//     </e>
#ifndef HID_ENDPOINT
#define HID_ENDPOINT 0
#else
#define HID_ENDPOINT 1
#endif

#ifndef WEBUSB_INTERFACE
#define WEBUSB_INTERFACE 0
#else
#define WEBUSB_INTERFACE 1
#endif

#define USBD_HID_ENABLE             HID_ENDPOINT
#define USBD_HID_EP_INTIN           1
#define USBD_HID_EP_INTOUT          1

#define USBD_HID_EP_INTIN_STACK     0
#define USBD_HID_WMAXPACKETSIZE     64
#define USBD_HID_BINTERVAL          1
#define USBD_HID_HS_ENABLE          0
#define USBD_HID_HS_WMAXPACKETSIZE  64
#define USBD_HID_HS_BINTERVAL       6
#define USBD_HID_STRDESC            L"CMSIS-DAP v1"
#define USBD_WEBUSB_STRDESC         L"WebUSB: CMSIS-DAP"
#define USBD_HID_INREPORT_NUM       1
#define USBD_HID_OUTREPORT_NUM      1
#define USBD_HID_INREPORT_MAX_SZ    64
#define USBD_HID_OUTREPORT_MAX_SZ   64
#define USBD_HID_FEATREPORT_MAX_SZ  1

//     <e0.0> Mass Storage Device (MSC)
//       <i> Enable class support for Mass Storage Device (MSC)
//       <h> Bulk Endpoint Settings
//         <o1.0..4> Bulk In Endpoint Number                  <1=>   1 <2=>   2 <3=>   3
//                                            <4=>   4        <5=>   5 <6=>   6 <7=>   7
//                                            <8=>   8        <9=>   9 <10=> 10 <11=> 11
//                                            <12=>  12       <13=> 13 <14=> 14 <15=> 15
//         <o2.0..4> Bulk Out Endpoint Number                 <1=>   1 <2=>   2 <3=>   3
//                                            <4=>   4        <5=>   5 <6=>   6 <7=>   7
//                                            <8=>   8        <9=>   9 <10=> 10 <11=> 11
//                                            <12=>  12       <13=> 13 <14=> 14 <15=> 15
//         <h> Endpoint Settings
//           <o3> Maximum Packet Size <1-1024>
//           <e4> High-speed
//             <i> If high-speed is enabled set endpoint settings for it
//             <o5> Maximum Packet Size <1-1024>
//             <o6> Maximum NAK Rate <0-255>
//           </e>
//         </h>
//       </h>
//       <h> Mass Storage Device Settings
//         <i> Device specific settings
//         <s0.126> MSC Interface String
//         <h> Inquiry Data
//           <s1.8>  Vendor Identification
//           <s2.16> Product Identification
//           <s3.4>  Product Revision Level
//         </h>
//       </h>
//     </e>
#ifndef MSC_ENDPOINT
#define MSC_ENDPOINT 0
#else
#define MSC_ENDPOINT 1
#endif
#define USBD_MSC_ENABLE             MSC_ENDPOINT
#define USBD_MSC_EP_BULKIN          2
#define USBD_MSC_EP_BULKOUT         2
#define USBD_MSC_EP_BULKIN_STACK    0
#define USBD_MSC_WMAXPACKETSIZE     64
#define USBD_MSC_HS_ENABLE          0
#define USBD_MSC_HS_WMAXPACKETSIZE  512
#define USBD_MSC_HS_BINTERVAL       0
#define USBD_MSC_STRDESC            L"USB_MSC"
// Make sure changes to USBD_MSC_INQUIRY_DATA are coordinated with mbed-ls
// since this is used to detect DAPLink drives
#define USBD_MSC_INQUIRY_DATA       "MBED    "         \
                                    "VFS             " \
                                    "0.1"

//     <e0.0> Audio Device (ADC)
//       <i> Enable class support for Audio Device (ADC)
//       <h> Isochronous Endpoint Settings
//         <o1.0..4> Isochronous Out Endpoint Number                 <1=>   1 <2=>   2 <3=>   3
//                                                   <4=>   4        <5=>   5 <6=>   6 <7=>   7
//                                                   <8=>   8        <9=>   9 <10=> 10 <11=> 11
//                                                   <12=>  12       <13=> 13 <14=> 14 <15=> 15
//         <h> Endpoint Settings
//           <o2.0..10> Maximum Endpoint Packet Size (in bytes) <0-1024>
//           <o3.0..10> Endpoint polling Interval (in ms) <1=>      1 <2=>      2 <3=>      4 <4=>      8
//                                                        <5=>     16 <6=>     32 <7=>     64 <8=>    128
//                                                        <9=>    256 <10=>   512 <11=>  1024 <12=>  2048
//                                                        <13=>  4096 <14=>  8192 <15=> 16384 <16=> 32768
//           <e4> High-speed
//             <i> If high-speed is enabled set endpoint settings for it
//             <o5.0..10> Maximum Endpoint Packet Size (in bytes) <0-1024>
//             <o5.11..12> Additional transactions per microframe <0=> None <1=> 1 additional <2=> 2 additional
//           </e>
//         </h>
//       </h>
//       <h> Audio Device Settings
//         <i> Device specific settings
//         <s0.126> Audio Control Interface String
//         <s1.126> Audio Streaming (Zero Bandwidth) Interface String
//         <s2.126> Audio Streaming (Operational) Interface String
//         <o6.0..7> Audio Subframe Size (in bytes) <0-255>
//         <o7.0..7> Sample Resolution (in bits) <0-255>
//         <o8.0..23> Sample Frequency (in Hz) <0-16777215>
//         <o9> Packet Size (in bytes) <1-256>
//         <o10> Packet Count <1-16>
//       </h>
//     </e>
#define USBD_ADC_ENABLE             0
#define USBD_ADC_EP_ISOOUT          3
#define USBD_ADC_WMAXPACKETSIZE     64
#define USBD_ADC_BINTERVAL          1
#define USBD_ADC_HS_ENABLE          0
#define USBD_ADC_HS_WMAXPACKETSIZE  64
#define USBD_ADC_CIF_STRDESC        L"USB_ADC"
#define USBD_ADC_SIF1_STRDESC       L"USB_ADC1"
#define USBD_ADC_SIF2_STRDESC       L"USB_ADC2"
#define USBD_ADC_BSUBFRAMESIZE      2
#define USBD_ADC_BBITRESOLUTION     16
#define USBD_ADC_TSAMFREQ           32000
#define USBD_ADC_CFG_P_S            32
#define USBD_ADC_CFG_P_C            1

//     <e0> Communication Device (CDC) - Abstract Control Model (ACM)
//       <i> Enable class support for Communication Device (CDC) - Abstract Control Model (ACM)
//       <h> Interrupt Endpoint Settings
//         <o1.0..4> Interrupt In Endpoint Number                 <1=>   1 <2=>   2 <3=>   3
//                                                <4=>   4        <5=>   5 <6=>   6 <7=>   7
//                                                <8=>   8        <9=>   9 <10=> 10 <11=> 11
//                                                <12=>  12       <13=> 13 <14=> 14 <15=> 15
//         <h> Endpoint Settings
//           <o2.0..10> Maximum Endpoint Packet Size (in bytes) <0-1024>
//           <o3.0..10> Endpoint polling Interval (in ms) <0-255>
//           <e4> High-speed
//             <i> If high-speed is enabled set endpoint settings for it
//             <o5.0..10> Maximum Endpoint Packet Size (in bytes) <0-1024>
//             <o5.11..12> Additional transactions per microframe <0=> None <1=> 1 additional <2=> 2 additional
//             <o6.0..10> Endpoint polling Interval (in ms) <1=>      1 <2=>      2 <3=>      4 <4=>      8
//                                                          <5=>     16 <6=>     32 <7=>     64 <8=>    128
//                                                          <9=>    256 <10=>   512 <11=>  1024 <12=>  2048
//                                                          <13=>  4096 <14=>  8192 <15=> 16384 <16=> 32768
//           </e4>
//         </h>
//       </h>
//       <h> Bulk Endpoint Settings
//         <o7.0..4> Bulk In Endpoint Number                  <1=>   1 <2=>   2 <3=>   3
//                                            <4=>   4        <5=>   5 <6=>   6 <7=>   7
//                                            <8=>   8        <9=>   9 <10=> 10 <11=> 11
//                                            <12=>  12       <13=> 13 <14=> 14 <15=> 15
//         <o8.0..4> Bulk Out Endpoint Number                 <1=>   1 <2=>   2 <3=>   3
//                                            <4=>   4        <5=>   5 <6=>   6 <7=>   7
//                                            <8=>   8        <9=>   9 <10=> 10 <11=> 11
//                                            <12=>  12       <13=> 13 <14=> 14 <15=> 15
//         <h> Endpoint Settings
//           <o9> Maximum Packet Size <1-1024>
//           <e10> High-speed
//             <i> If high-speed is enabled set endpoint settings for it
//             <o11> Maximum Packet Size <1-1024>
//             <o12> Maximum NAK Rate <0-255>
//           </e10>
//         </h>
//       </h>
//       <h> Communication Device Settings
//         <i> Device specific settings
//         <s0.126> Communication Class Interface String
//         <s1.126> Data Class Interface String
//         <o13> Maximum Communication Device Send Buffer Size
//            <8=>     8 Bytes <16=>   16 Bytes <32=>     32 Bytes <64=>  64 Bytes <128=> 128 Bytes
//            <256=> 256 Bytes <512=> 512 Bytes <1024=> 1024 Bytes
//         <o14> Maximum Communication Device Receive Buffer Size
//            <i> Minimum size must be as big as maximum packet size for Bulk Out Endpoint
//            <8=>     8 Bytes <16=>   16 Bytes <32=>     32 Bytes <64=>  64 Bytes <128=> 128 Bytes
//            <256=> 256 Bytes <512=> 512 Bytes <1024=> 1024 Bytes
//       </h>
//     </e>

#ifndef CDC_ENDPOINT
#define CDC_ENDPOINT 0
#else
#define CDC_ENDPOINT 1
#endif
#define USBD_CDC_ACM_ENABLE             CDC_ENDPOINT
#define USBD_CDC_ACM_EP_INTIN           3
#define USBD_CDC_ACM_EP_INTIN_STACK     0
#define USBD_CDC_ACM_WMAXPACKETSIZE     16
#define USBD_CDC_ACM_BINTERVAL          32
#define USBD_CDC_ACM_HS_ENABLE          0
#define USBD_CDC_ACM_HS_WMAXPACKETSIZE  16
#define USBD_CDC_ACM_HS_BINTERVAL       2
#define USBD_CDC_ACM_EP_BULKIN          4
#define USBD_CDC_ACM_EP_BULKOUT         4
#define USBD_CDC_ACM_EP_BULKIN_STACK    0
#define USBD_CDC_ACM_WMAXPACKETSIZE1    64
#define USBD_CDC_ACM_HS_ENABLE1         0
#define USBD_CDC_ACM_HS_WMAXPACKETSIZE1 64
#define USBD_CDC_ACM_HS_BINTERVAL1      0
#define USBD_CDC_ACM_CIF_STRDESC        L"mbed Serial Port"
#define USBD_CDC_ACM_DIF_STRDESC        L"mbed Serial Port"
#define USBD_CDC_ACM_SENDBUF_SIZE       64
#define USBD_CDC_ACM_RECEIVEBUF_SIZE    64
#if (((USBD_CDC_ACM_HS_ENABLE1) && (USBD_CDC_ACM_SENDBUF_SIZE    < USBD_CDC_ACM_HS_WMAXPACKETSIZE1)) || (USBD_CDC_ACM_SENDBUF_SIZE    < USBD_CDC_ACM_WMAXPACKETSIZE1))
#error "Send Buffer size must be larger or equal to Bulk In maximum packet size!"
#endif
#if (((USBD_CDC_ACM_HS_ENABLE1) && (USBD_CDC_ACM_RECEIVEBUF_SIZE < USBD_CDC_ACM_HS_WMAXPACKETSIZE1)) || (USBD_CDC_ACM_RECEIVEBUF_SIZE < USBD_CDC_ACM_WMAXPACKETSIZE1))
#error "Receive Buffer size must be larger or equal to Bulk Out maximum packet size!"
#endif

//     <e0> Custom Class Device
//       <i> Enables USB Custom Class Requests
//       <i> Class IDs:
//       <i> 0x00 - Class Reserved ID
//       <i> 0x01 - Class Audio ID
//       <i> 0x02 - Class Communications ID
//       <i> 0x03 - Class Human Interface ID
//       <i> 0x04 - Class Monitor ID
//       <i> 0x05 - Class Physical Interface ID
//       <i> 0x06 - Class Power ID
//       <i> 0x07 - Class Printer ID
//       <i> 0x08 - Class Storage ID
//       <i> 0x09 - Class HUB ID
//       <i> 0xEF - Class Miscellaneous ID
//       <i> 0xFF - Class Vendor Specific ID
//     </e>
#define USBD_CLS_ENABLE             0

//     WebUSB support
#define USBD_WEBUSB_ENABLE          WEBUSB_INTERFACE
#define USBD_WEBUSB_VENDOR_CODE     0x21
#define USBD_WEBUSB_LANDING_URL     "os.mbed.com/webusb/landing-page/?bid="
#define USBD_WEBUSB_ORIGIN_URL      "os.mbed.com/"

//     Microsoft OS Descriptors 2.0 (WinUSB) support
#define USBD_WINUSB_ENABLE          WINUSB_INTERFACE
#define USBD_WINUSB_VENDOR_CODE     0x20
//   </e>
// </e>

#ifndef BULK_ENDPOINT
#define BULK_ENDPOINT 0
#else
#define BULK_ENDPOINT 1
#endif
#define USBD_BULK_ENABLE             BULK_ENDPOINT
#define USBD_BULK_EP_BULKIN          5
#define USBD_BULK_EP_BULKOUT         5
#define USBD_BULK_WMAXPACKETSIZE     64
#define USBD_BULK_HS_ENABLE          0
#define USBD_BULK_HS_WMAXPACKETSIZE  512
#define USBD_BULK_STRDESC            L"CMSIS-DAP v2"

/* USB Device Calculations ---------------------------------------------------*/

#define USBD_IF_NUM_MAX             (USBD_BULK_ENABLE+USBD_WEBUSB_ENABLE+USBD_HID_ENABLE+USBD_MSC_ENABLE+(USBD_ADC_ENABLE*2)+(USBD_CDC_ACM_ENABLE*2)+USBD_CLS_ENABLE)
#define USBD_MULTI_IF               (USBD_CDC_ACM_ENABLE*(USBD_HID_ENABLE|USBD_MSC_ENABLE|USBD_ADC_ENABLE|USBD_CLS_ENABLE|USBD_WEBUSB_ENABLE|USBD_BULK_ENABLE))
// #define MAX(x, y)                   (((x) < (y)) ? (y) : (x))
#define USBD_EP_NUM_CALC0           MAX((USBD_HID_ENABLE    *(USBD_HID_EP_INTIN     )), (USBD_HID_ENABLE    *(USBD_HID_EP_INTOUT!=0)*(USBD_HID_EP_INTOUT)))
#define USBD_EP_NUM_CALC1           MAX((USBD_MSC_ENABLE    *(USBD_MSC_EP_BULKIN    )), (USBD_MSC_ENABLE    *(USBD_MSC_EP_BULKOUT)))
#define USBD_EP_NUM_CALC2           MAX((USBD_ADC_ENABLE    *(USBD_ADC_EP_ISOOUT    )), (USBD_CDC_ACM_ENABLE*(USBD_CDC_ACM_EP_INTIN)))
#define USBD_EP_NUM_CALC3           MAX((USBD_CDC_ACM_ENABLE*(USBD_CDC_ACM_EP_BULKIN)), (USBD_CDC_ACM_ENABLE*(USBD_CDC_ACM_EP_BULKOUT)))
#define USBD_EP_NUM_CALC4           MAX(USBD_EP_NUM_CALC0, USBD_EP_NUM_CALC1)
#define USBD_EP_NUM_CALC5           MAX(USBD_EP_NUM_CALC2, USBD_EP_NUM_CALC3)
#define USBD_EP_NUM_CALC6           MAX(USBD_EP_NUM_CALC4, USBD_EP_NUM_CALC5)
#define USBD_EP_NUM_CALC7           MAX((USBD_BULK_ENABLE*(USBD_BULK_EP_BULKIN)), (USBD_BULK_ENABLE*(USBD_BULK_EP_BULKOUT)))
#define USBD_EP_NUM                 MAX(USBD_EP_NUM_CALC6, USBD_EP_NUM_CALC7)

#if    (USBD_HID_ENABLE)
#if    (USBD_MSC_ENABLE)
#if ((((USBD_HID_EP_INTIN   == USBD_MSC_EP_BULKIN)      || \
       (USBD_HID_EP_INTIN   == USBD_MSC_EP_BULKOUT)))   || \
      ((USBD_HID_EP_INTOUT  != 0)                       && \
       (USBD_HID_EP_INTOUT  == USBD_MSC_EP_BULKIN)      || \
       (USBD_HID_EP_INTOUT  == USBD_MSC_EP_BULKOUT)))
#error "HID and Mass Storage Device Interface can not use same Endpoints!"
#endif
#endif
#if    (USBD_ADC_ENABLE)
#if   ((USBD_HID_EP_INTIN   == USBD_ADC_EP_ISOOUT)  || \
      ((USBD_HID_EP_INTOUT  != 0)                   && \
       (USBD_HID_EP_INTOUT  == USBD_ADC_EP_ISOOUT)))
#error "HID and Audio Device Interface can not use same Endpoints!"
#endif
#endif
#if    (USBD_CDC_ACM_ENABLE)
#if  (((USBD_HID_EP_INTIN   == USBD_CDC_ACM_EP_INTIN)   || \
       (USBD_HID_EP_INTIN   == USBD_CDC_ACM_EP_BULKIN)  || \
       (USBD_HID_EP_INTIN   == USBD_CDC_ACM_EP_BULKOUT))|| \
      ((USBD_HID_EP_INTOUT  != 0)                       && \
      ((USBD_HID_EP_INTOUT  == USBD_CDC_ACM_EP_INTIN)   || \
       (USBD_HID_EP_INTOUT  == USBD_CDC_ACM_EP_BULKIN)  || \
       (USBD_HID_EP_INTOUT  == USBD_CDC_ACM_EP_BULKOUT))))
#error "HID and Communication Device Interface can not use same Endpoints!"
#endif
#endif
#endif

#if    (USBD_MSC_ENABLE)
#if    (USBD_ADC_ENABLE)
#if   ((USBD_MSC_EP_BULKIN  == USBD_ADC_EP_ISOOUT)  || \
       (USBD_MSC_EP_BULKOUT == USBD_ADC_EP_ISOOUT))
#error "Mass Storage Device and Audio Device Interface can not use same Endpoints!"
#endif
#endif
#if    (USBD_CDC_ACM_ENABLE)
#if   ((USBD_MSC_EP_BULKIN  == USBD_CDC_ACM_EP_INTIN)   || \
       (USBD_MSC_EP_BULKIN  == USBD_CDC_ACM_EP_BULKIN)  || \
       (USBD_MSC_EP_BULKIN  == USBD_CDC_ACM_EP_BULKOUT) || \
       (USBD_MSC_EP_BULKOUT == USBD_CDC_ACM_EP_INTIN)   || \
       (USBD_MSC_EP_BULKOUT == USBD_CDC_ACM_EP_BULKIN)  || \
       (USBD_MSC_EP_BULKOUT == USBD_CDC_ACM_EP_BULKOUT))
#error "Mass Storage Device and Communication Device Interface can not use same Endpoints!"
#endif
#endif
#endif

#if    (USBD_ADC_ENABLE)
#if    (USBD_CDC_ACM_ENABLE)
#if   ((USBD_ADC_EP_ISOOUT  == USBD_CDC_ACM_EP_INTIN)   || \
       (USBD_ADC_EP_ISOOUT  == USBD_CDC_ACM_EP_BULKIN)  || \
       (USBD_ADC_EP_ISOOUT  == USBD_CDC_ACM_EP_BULKOUT))
#error "Audio Device and Communication Device Interface can not use same Endpoints!"
#endif
#endif
#endif

#define USBD_ADC_CIF_NUM           (0)
#define USBD_ADC_SIF1_NUM          (1)
#define USBD_ADC_SIF2_NUM          (2)

#define USBD_ADC_CIF_STR_NUM       (3+USBD_STRDESC_SER_ENABLE+0)
#define USBD_ADC_SIF1_STR_NUM      (3+USBD_STRDESC_SER_ENABLE+1)
#define USBD_ADC_SIF2_STR_NUM      (3+USBD_STRDESC_SER_ENABLE+2)
#define USBD_CDC_ACM_CIF_STR_NUM   (3+USBD_STRDESC_SER_ENABLE+USBD_ADC_ENABLE*3+0)
#define USBD_CDC_ACM_DIF_STR_NUM   (3+USBD_STRDESC_SER_ENABLE+USBD_ADC_ENABLE*3+1)
#define USBD_HID_IF_STR_NUM        (3+USBD_STRDESC_SER_ENABLE+USBD_ADC_ENABLE*3+USBD_CDC_ACM_ENABLE*2)
#define USBD_WEBUSB_IF_STR_NUM     (3+USBD_STRDESC_SER_ENABLE+USBD_ADC_ENABLE*3+USBD_CDC_ACM_ENABLE*2+USBD_HID_ENABLE)
#define USBD_MSC_IF_STR_NUM        (3+USBD_STRDESC_SER_ENABLE+USBD_ADC_ENABLE*3+USBD_CDC_ACM_ENABLE*2+USBD_HID_ENABLE+USBD_WEBUSB_ENABLE)
#define USBD_BULK_IF_STR_NUM       (3+USBD_STRDESC_SER_ENABLE+USBD_ADC_ENABLE*3+USBD_CDC_ACM_ENABLE*2+USBD_HID_ENABLE+USBD_WEBUSB_ENABLE+USBD_BULK_ENABLE)

#if    (USBD_HID_ENABLE)
#if    (USBD_HID_HS_ENABLE)
#define USBD_HID_MAX_PACKET       ((USBD_HID_HS_WMAXPACKETSIZE > USBD_HID_WMAXPACKETSIZE) ? USBD_HID_HS_WMAXPACKETSIZE : USBD_HID_WMAXPACKETSIZE)
#else
#define USBD_HID_MAX_PACKET        (USBD_HID_WMAXPACKETSIZE)
#endif
#else
#define USBD_HID_MAX_PACKET        (0)
#endif
#if    (USBD_MSC_ENABLE)
#if    (USBD_MSC_HS_ENABLE)
#define USBD_MSC_MAX_PACKET       ((USBD_MSC_HS_WMAXPACKETSIZE > USBD_MSC_WMAXPACKETSIZE) ? USBD_MSC_HS_WMAXPACKETSIZE : USBD_MSC_WMAXPACKETSIZE)
#else
#define USBD_MSC_MAX_PACKET        (USBD_MSC_WMAXPACKETSIZE)
#endif
#else
#define USBD_MSC_MAX_PACKET        (0)
#endif
#if    (USBD_ADC_ENABLE)
#if    (USBD_ADC_HS_ENABLE)
#define USBD_ADC_MAX_PACKET       ((USBD_ADC_HS_WMAXPACKETSIZE > USBD_ADC_WMAXPACKETSIZE) ? USBD_ADC_HS_WMAXPACKETSIZE : USBD_ADC_WMAXPACKETSIZE)
#else
#define USBD_ADC_MAX_PACKET        (USBD_ADC_WMAXPACKETSIZE)
#endif
#else
#define USBD_ADC_MAX_PACKET        (0)
#endif
#if    (USBD_CDC_ACM_ENABLE)
#if    (USBD_CDC_ACM_HS_ENABLE)
#define USBD_CDC_ACM_MAX_PACKET   ((USBD_CDC_ACM_HS_WMAXPACKETSIZE > USBD_CDC_ACM_WMAXPACKETSIZE) ? USBD_CDC_ACM_HS_WMAXPACKETSIZE : USBD_CDC_ACM_WMAXPACKETSIZE)
#else
#define USBD_CDC_ACM_MAX_PACKET    (USBD_CDC_ACM_WMAXPACKETSIZE)
#endif
#if    (USBD_CDC_ACM_HS_ENABLE1)
#define USBD_CDC_ACM_MAX_PACKET1  ((USBD_CDC_ACM_HS_WMAXPACKETSIZE1 > USBD_CDC_ACM_WMAXPACKETSIZE1) ? USBD_CDC_ACM_HS_WMAXPACKETSIZE1 : USBD_CDC_ACM_WMAXPACKETSIZE1)
#else
#define USBD_CDC_ACM_MAX_PACKET1   (USBD_CDC_ACM_WMAXPACKETSIZE1)
#endif
#else
#define USBD_CDC_ACM_MAX_PACKET    (0)
#define USBD_CDC_ACM_MAX_PACKET1   (0)
#endif
#if    (USBD_BULK_ENABLE)
#if    (USBD_BULK_HS_ENABLE)
#define USBD_BULK_MAX_PACKET       ((USBD_BULK_HS_WMAXPACKETSIZE > USBD_BULK_WMAXPACKETSIZE) ? USBD_BULK_HS_WMAXPACKETSIZE : USBD_BULK_WMAXPACKETSIZE)
#else
#define USBD_BULK_MAX_PACKET        (USBD_BULK_WMAXPACKETSIZE)
#endif
#else
#define USBD_BULK_MAX_PACKET        (0)
#endif
#define USBD_MAX_PACKET_CALC0     ((USBD_HID_MAX_PACKET   > USBD_HID_MAX_PACKET      ) ? (USBD_HID_MAX_PACKET  ) : (USBD_HID_MAX_PACKET      ))
#define USBD_MAX_PACKET_CALC1     ((USBD_ADC_MAX_PACKET   > USBD_CDC_ACM_MAX_PACKET  ) ? (USBD_ADC_MAX_PACKET  ) : (USBD_CDC_ACM_MAX_PACKET  ))
#define USBD_MAX_PACKET_CALC2     ((USBD_MAX_PACKET_CALC0 > USBD_MAX_PACKET_CALC1    ) ? (USBD_MAX_PACKET_CALC0) : (USBD_MAX_PACKET_CALC1    ))
#define USBD_MAX_PACKET_CALC3     ((USBD_BULK_MAX_PACKET > USBD_CDC_ACM_MAX_PACKET1 ) ? (USBD_BULK_MAX_PACKET) : (USBD_CDC_ACM_MAX_PACKET1 ))
#define USBD_MAX_PACKET           ((USBD_MAX_PACKET_CALC3 > USBD_MAX_PACKET_CALC2    ) ? (USBD_MAX_PACKET_CALC3) : (USBD_MAX_PACKET_CALC2    ))



/*------------------------------------------------------------------------------
 *      USB Config Functions
 *----------------------------------------------------------------------------*/

#ifndef  __USB_CONFIG___
#define  __USB_CONFIG__

#ifndef  __NO_USB_LIB_C
#include "usb_lib.c"
#endif

#endif  /* __USB_CONFIG__ */
