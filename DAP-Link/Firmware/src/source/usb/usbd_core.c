/**
 * @file    usbd_core.c
 * @brief   USB Device core
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
#include "usb_for_lib.h"
#include "info.h"

U16 USBD_DeviceStatus;
U8 USBD_DeviceAddress;
U8 USBD_Configuration;
U32 USBD_EndPointMask;
U32 USBD_EndPointHalt;
U32 USBD_EndPointStall;          /* EP must stay stalled */
U8 USBD_NumInterfaces;
U8 USBD_HighSpeed;
U8 USBD_ZLP;

USBD_EP_DATA USBD_EP0Data;
USB_SETUP_PACKET USBD_SetupPacket;

#ifdef __RTX
OS_TID USBD_RTX_DevTask;            /* USB Device Task ID */
OS_TID USBD_RTX_EPTask[16];         /* USB Endpoint Task ID's */
OS_TID USBD_RTX_CoreTask;           /* USB Core Task ID */
#endif


__asm void $$USBD$$version(void)
{
    /* Export a version number symbol for a version control. */
    EXPORT  __RL_USBD_VER
__RL_USBD_VER   EQU     0x470
}


/*
 *  Init USB Device Core and Hardware
 *    Parameters:      None
 *    Return Value:    None
 */

void usbd_init(void)
{
    USBD_HighSpeed = __FALSE;
    usbd_class_init();
    USBD_RTX_TaskInit();
    USBD_Init();
}


/*
 *  USB Device Connect/Disconnect Function
 *   Called by the User to Connect/Disconnect USB Device
 *    Parameters:      con:   Connect/Disconnect
 *    Return Value:    None
 */

void usbd_connect(BOOL con)
{
    USBD_Connect(con);
}


/*
 *  Reset USB Device Core
 *    Parameters:      None
 *    Return Value:    None
 */

void usbd_reset_core(void)
{
    USBD_DeviceStatus  = usbd_power;
    USBD_DeviceAddress = 0;
    USBD_Configuration = 0;
    USBD_EndPointMask  = 0x00010001;
    USBD_EndPointHalt  = 0x00000000;
    USBD_EndPointStall = 0x00000000;
}


/*
 *  USB Device Configured Function
 *   Called by the User to check id USB Device is configured
 *    Parameters:
 *    Return Value:    Configurated state (FALSE = unconfigured, TRUE = configured)
 */

BOOL usbd_configured(void)
{
    if (USBD_Configuration) {
        return (__TRUE);
    }

    return (__FALSE);
}


/*
 *  USB Device Request - Setup Stage
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_SetupStage(void)
{
    USBD_ReadEP(0x00, (U8 *)&USBD_SetupPacket, sizeof(USBD_SetupPacket));
}


/*
 *  USB Device Request - Data In Stage
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_DataInStage(void)
{
    U32 cnt;

    if (USBD_EP0Data.Count > usbd_max_packet0) {
        cnt = usbd_max_packet0;
    } else {
        cnt = USBD_EP0Data.Count;
    }

    if (!cnt) {
        USBD_ZLP = 0;
    }

    cnt = USBD_WriteEP(0x80, USBD_EP0Data.pData, cnt);
    USBD_EP0Data.pData += cnt;
    USBD_EP0Data.Count -= cnt;
}


/*
 *  USB Device Request - Data Out Stage
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_DataOutStage(void)
{
    U32 cnt;
    cnt = USBD_ReadEP(0x00, USBD_EP0Data.pData, USBD_EP0Data.Count);
    USBD_EP0Data.pData += cnt;
    USBD_EP0Data.Count -= cnt;
}


/*
 *  USB Device Request - Status In Stage
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_StatusInStage(void)
{
    USBD_WriteEP(0x80, NULL, 0);
}


/*
 *  USB Device Request - Status Out Stage
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_StatusOutStage(void)
{
    USBD_ReadEP(0x00, USBD_EP0Buf, usbd_max_packet0);
}


/*
 *  Get Status USB Device Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

static inline BOOL USBD_ReqGetStatus(void)
{
    U32 n, m;

    switch (USBD_SetupPacket.bmRequestType.Recipient) {
        case REQUEST_TO_DEVICE:
            USBD_EP0Data.pData = (U8 *)&USBD_DeviceStatus;
            break;

        case REQUEST_TO_INTERFACE:
            if ((USBD_Configuration != 0) && (USBD_SetupPacket.wIndexL < USBD_NumInterfaces)) {
                *((__packed U16 *)USBD_EP0Buf) = 0;
                USBD_EP0Data.pData = USBD_EP0Buf;
            } else {
                return (__FALSE);
            }

            break;

        case REQUEST_TO_ENDPOINT:
            n = USBD_SetupPacket.wIndexL & 0x8F;
            m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);

            if (((USBD_Configuration != 0) || ((n & 0x0F) == 0)) && (USBD_EndPointMask & m)) {
                *((__packed U16 *)USBD_EP0Buf) = (USBD_EndPointHalt & m) ? 1 : 0;
                USBD_EP0Data.pData = USBD_EP0Buf;
            } else {
                return (__FALSE);
            }

            break;

        default:
            return (__FALSE);
    }

    return (__TRUE);
}


/*
 *  Set/Clear Feature USB Device Request
 *    Parameters:      sc:    0 - Clear, 1 - Set
 *    Return Value:    TRUE - Success, FALSE - Error
 */

static inline BOOL USBD_ReqSetClrFeature(U32 sc)
{
    U32 n, m;

    switch (USBD_SetupPacket.bmRequestType.Recipient) {
        case REQUEST_TO_DEVICE:
            if (USBD_SetupPacket.wValue == USB_FEATURE_REMOTE_WAKEUP) {
                if (sc) {
                    USBD_WakeUpCfg(__TRUE);
                    USBD_DeviceStatus |=  USB_GETSTATUS_REMOTE_WAKEUP;
                } else {
                    USBD_WakeUpCfg(__FALSE);
                    USBD_DeviceStatus &= ~USB_GETSTATUS_REMOTE_WAKEUP;
                }
            } else {
                return (__FALSE);
            }

            break;

        case REQUEST_TO_INTERFACE:
            return (__FALSE);

        case REQUEST_TO_ENDPOINT:
            n = USBD_SetupPacket.wIndexL & 0x8F;
            m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);

            if ((USBD_Configuration != 0) && ((n & 0x0F) != 0) && (USBD_EndPointMask & m)) {
                if (USBD_SetupPacket.wValue == USB_FEATURE_ENDPOINT_STALL) {
                    if (sc) {
                        USBD_SetStallEP(n);
                        USBD_EndPointHalt |=  m;
                    } else {
                        if ((USBD_EndPointStall & m) != 0) {
                            return (__TRUE);
                        }

                        USBD_ClrStallEP(n);
                        USBD_ReqClrFeature_MSC(n);
                        USBD_EndPointHalt &= ~m;
                    }
                } else {
                    return (__FALSE);
                }
            } else {
                return (__FALSE);
            }

            break;

        default:
            return (__FALSE);
    }

    return (__TRUE);
}


/*
 *  Set Address USB Device Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

static inline BOOL USBD_ReqSetAddress(void)
{
    switch (USBD_SetupPacket.bmRequestType.Recipient) {
        case REQUEST_TO_DEVICE:
            USBD_DeviceAddress = 0x80 | USBD_SetupPacket.wValueL;
            break;

        default:
            return (__FALSE);
    }

    return (__TRUE);
}


/*
 *  Get Descriptor USB Device Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

static inline BOOL USBD_ReqGetDescriptor(void)
{
    U8  *pD;
    U32  len, n;

    switch (USBD_SetupPacket.bmRequestType.Recipient) {
        case REQUEST_TO_DEVICE:
            switch (USBD_SetupPacket.wValueH) {
                case USB_DEVICE_DESCRIPTOR_TYPE:
                    USBD_EP0Data.pData = (U8 *)USBD_DeviceDescriptor;
                    len = USB_DEVICE_DESC_SIZE;
                    break;

                case USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE:
                    if (!usbd_hs_enable) {
                        return (__FALSE);  /* High speed not enabled */
                    }

                    if (USBD_HighSpeed == __FALSE) {
                        USBD_EP0Data.pData = (U8 *)USBD_DeviceQualifier;
                    } else {
                        USBD_EP0Data.pData = (U8 *)USBD_DeviceQualifier_HS;
                    }

                    len = USB_DEVICE_QUALI_SIZE;
                    break;

                case USB_CONFIGURATION_DESCRIPTOR_TYPE:
                    if ((!usbd_hs_enable) && (USBD_HighSpeed == __TRUE)) {
                        return (__FALSE);  /* High speed request but high-speed not enabled */
                    }

                    if (USBD_HighSpeed == __FALSE) {
                        pD = (U8 *)USBD_ConfigDescriptor;
                        ((USB_CONFIGURATION_DESCRIPTOR *)pD)->bDescriptorType = USB_CONFIGURATION_DESCRIPTOR_TYPE; //same descriptor is used in other configuration
                    } else {
                        pD = (U8 *)USBD_ConfigDescriptor_HS;
                        ((USB_CONFIGURATION_DESCRIPTOR *)pD)->bDescriptorType = USB_CONFIGURATION_DESCRIPTOR_TYPE; //same descriptor is used in other configuration
                    }

                    for (n = 0; n != USBD_SetupPacket.wValueL; n++) {
                        if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bLength != 0) {
                            pD += ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
                        }
                    }

                    if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bLength == 0) {
                        return (__FALSE);
                    }

                    USBD_EP0Data.pData = pD;
                    len = ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
                    break;

                case USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE:
                    if (!usbd_hs_enable) {
                        return (__FALSE);  /* High speed not enabled */
                    }

                    if (USBD_HighSpeed == __FALSE) {
                        pD = (U8 *)USBD_ConfigDescriptor_HS;
                        ((USB_CONFIGURATION_DESCRIPTOR *)pD)->bDescriptorType = USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE; //same descriptor is used in main configuration
                    } else {
                        pD = (U8 *)USBD_ConfigDescriptor;
                        ((USB_CONFIGURATION_DESCRIPTOR *)pD)->bDescriptorType = USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE; //same descriptor is used in main configuration
                    }

                    for (n = 0; n != USBD_SetupPacket.wValueL; n++) {
                        if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bLength != 0) {
                            pD += ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
                        }
                    }

                    if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bLength == 0) {
                        return (__FALSE);
                    }

                    USBD_EP0Data.pData = pD;
                    len = ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
                    break;

                case USB_STRING_DESCRIPTOR_TYPE:
                    pD = (U8 *)USBD_StringDescriptor;

                    // added by sam to send unique id string descriptor
                    if (USBD_SetupPacket.wValueL == 3) {
                        USBD_EP0Data.pData = (uint8_t *)info_get_unique_id_string_descriptor();
                        len = ((USB_STRING_DESCRIPTOR *)USBD_EP0Data.pData)->bLength;
                        break;
                    }

                    for (n = 0; n != USBD_SetupPacket.wValueL; n++) {
                        if (((USB_STRING_DESCRIPTOR *)pD)->bLength != 0) {
                            pD += ((USB_STRING_DESCRIPTOR *)pD)->bLength;
                        }
                    }

                    if (((USB_STRING_DESCRIPTOR *)pD)->bLength == 0) {
                        return (__FALSE);
                    }

                    USBD_EP0Data.pData = pD;
                    len = ((USB_STRING_DESCRIPTOR *)pD)->bLength;
                    break;

                case USB_BINARY_OBJECT_STORE_DESCRIPTOR_TYPE:
                    if (!usbd_bos_enable) {
                        return (__FALSE);  /* High speed not enabled */
                    }

                    pD = (U8 *)USBD_BinaryObjectStoreDescriptor;
                    USBD_EP0Data.pData = pD;

                    if (((USB_BINARY_OBJECT_STORE_DESCRIPTOR *)pD)->bLength == 0) {
                        return (__FALSE);
                    }

                    len = ((USB_BINARY_OBJECT_STORE_DESCRIPTOR *)pD)->wTotalLength;
                    break;

                default:
                    return (__FALSE);
            }

            break;

        case REQUEST_TO_INTERFACE:
            if (!USBD_ReqGetDescriptor_HID(&pD, &len)) {
                return (__FALSE);
            }

            break;

        default:
            return (__FALSE);
    }

    if (USBD_EP0Data.Count > len) {
        USBD_EP0Data.Count = len;

        if (!(USBD_EP0Data.Count & (usbd_max_packet0 - 1))) {
            USBD_ZLP = 1;
        }
    }

    return (__TRUE);
}


/*
 *  Get Configuration USB Device Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

static inline BOOL USBD_ReqGetConfiguration(void)
{
    switch (USBD_SetupPacket.bmRequestType.Recipient) {
        case REQUEST_TO_DEVICE:
            USBD_EP0Data.pData = &USBD_Configuration;
            break;

        default:
            return (__FALSE);
    }

    return (__TRUE);
}


/*
 *  Set Configuration USB Device Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

static inline BOOL USBD_ReqSetConfiguration(void)
{
    USB_CONFIGURATION_DESCRIPTOR *pD;
    U32                           alt = 0;
    U32                           n, m;

    switch (USBD_SetupPacket.bmRequestType.Recipient) {
        case REQUEST_TO_DEVICE:
            if (USBD_SetupPacket.wValueL) {
                if ((!usbd_hs_enable) && (USBD_HighSpeed == __TRUE)) {
                    return (__FALSE);  /* High speed request but high-speed not enabled */
                }

                if (USBD_HighSpeed == __FALSE) {
                    pD = (USB_CONFIGURATION_DESCRIPTOR *)USBD_ConfigDescriptor;
                } else {
                    pD = (USB_CONFIGURATION_DESCRIPTOR *)USBD_ConfigDescriptor_HS;
                }

                while (pD->bLength) {
                    switch (pD->bDescriptorType) {
                        case USB_CONFIGURATION_DESCRIPTOR_TYPE:
                        case USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE:
                            if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bConfigurationValue == USBD_SetupPacket.wValueL) {
                                USBD_Configuration = USBD_SetupPacket.wValueL;
                                USBD_NumInterfaces = ((USB_CONFIGURATION_DESCRIPTOR *)pD)->bNumInterfaces;

                                for (n = 0; n < usbd_if_num; n++) {
                                    USBD_AltSetting[n] = 0;
                                }

                                for (n = 1; n < 16; n++) {
                                    if (USBD_EndPointMask & (1 << n)) {
                                        USBD_DisableEP(n);
                                    }

                                    if (USBD_EndPointMask & ((1 << 16) << n)) {
                                        USBD_DisableEP(n | 0x80);
                                    }
                                }

                                USBD_EndPointMask = 0x00010001;
                                USBD_EndPointHalt = 0x00000000;
                                USBD_EndPointStall = 0x00000000;
                                USBD_Configure(__TRUE);

                                if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bmAttributes & USB_CONFIG_POWERED_MASK) {
                                    USBD_DeviceStatus |=  USB_GETSTATUS_SELF_POWERED;
                                } else {
                                    USBD_DeviceStatus &= ~USB_GETSTATUS_SELF_POWERED;
                                }
                            } else {
                                pD = (USB_CONFIGURATION_DESCRIPTOR *)((U8 *)pD + ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength);
                                continue;
                            }

                            break;

                        case USB_INTERFACE_DESCRIPTOR_TYPE:
                            alt = ((USB_INTERFACE_DESCRIPTOR *)pD)->bAlternateSetting;
                            break;

                        case USB_ENDPOINT_DESCRIPTOR_TYPE:
                            if (alt == 0) {
                                n = ((USB_ENDPOINT_DESCRIPTOR *)pD)->bEndpointAddress & 0x8F;
                                m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);
                                USBD_EndPointMask |= m;
                                USBD_ConfigEP((void *)pD);
                                USBD_EnableEP(n);
                                USBD_ResetEP(n);
                            }

                            break;
                    }

                    pD = (USB_CONFIGURATION_DESCRIPTOR *)((U8 *)pD + pD->bLength);
                }
            } else {
                USBD_Configuration = 0;

                for (n = 1; n < 16; n++) {
                    if (USBD_EndPointMask & (1 << n)) {
                        USBD_DisableEP(n);
                    }

                    if (USBD_EndPointMask & ((1 << 16) << n)) {
                        USBD_DisableEP(n | 0x80);
                    }
                }

                USBD_EndPointMask  = 0x00010001;
                USBD_EndPointHalt  = 0x00000000;
                USBD_EndPointStall = 0x00000000;
                USBD_Configure(__FALSE);
            }

            if (USBD_Configuration != USBD_SetupPacket.wValueL) {
                return (__FALSE);
            }

            break;

        default:
            return (__FALSE);
    }

    return (__TRUE);
}


/*
 *  Get Interface USB Device Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

static inline BOOL USBD_ReqGetInterface(void)
{
    switch (USBD_SetupPacket.bmRequestType.Recipient) {
        case REQUEST_TO_INTERFACE:
            if ((USBD_Configuration != 0) && (USBD_SetupPacket.wIndexL < USBD_NumInterfaces)) {
                USBD_EP0Data.pData = USBD_AltSetting + USBD_SetupPacket.wIndexL;
            } else {
                return (__FALSE);
            }

            break;

        default:
            return (__FALSE);
    }

    return (__TRUE);
}


/*
 *  Set Interface USB Device Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

static inline BOOL USBD_ReqSetInterface(void)
{
    USB_COMMON_DESCRIPTOR *pD;
    U32                    ifn = 0, alt = 0, old = 0, msk = 0;
    U32                    n, m;
    BOOL                   set;

    switch (USBD_SetupPacket.bmRequestType.Recipient) {
        case REQUEST_TO_INTERFACE:
            if (USBD_Configuration == 0) {
                return (__FALSE);
            }

            set = __FALSE;

            if ((!usbd_hs_enable) && (USBD_HighSpeed == __TRUE)) {
                return (__FALSE);  /* High speed request but high-speed not enabled */
            }

            if (USBD_HighSpeed == __FALSE) {
                pD = (USB_COMMON_DESCRIPTOR *)USBD_ConfigDescriptor;
            } else {
                pD = (USB_COMMON_DESCRIPTOR *)USBD_ConfigDescriptor_HS;
            }

            while (pD->bLength) {
                switch (pD->bDescriptorType) {
                    case USB_CONFIGURATION_DESCRIPTOR_TYPE:
                    case USB_OTHER_SPEED_CONFIG_DESCRIPTOR_TYPE:
                        if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bConfigurationValue != USBD_Configuration) {
                            pD = (USB_COMMON_DESCRIPTOR *)((U8 *)pD + ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength);
                            continue;
                        }

                        break;

                    case USB_INTERFACE_DESCRIPTOR_TYPE:
                        ifn = ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber;
                        alt = ((USB_INTERFACE_DESCRIPTOR *)pD)->bAlternateSetting;
                        msk = 0;

                        if ((ifn == USBD_SetupPacket.wIndexL) && (alt == USBD_SetupPacket.wValueL)) {
                            set = __TRUE;
                            old = USBD_AltSetting[ifn];
                            USBD_AltSetting[ifn] = (U8)alt;
                        }

                        break;

                    case USB_ENDPOINT_DESCRIPTOR_TYPE:
                        if (ifn == USBD_SetupPacket.wIndexL) {
                            n = ((USB_ENDPOINT_DESCRIPTOR *)pD)->bEndpointAddress & 0x8F;
                            m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);

                            if (alt == USBD_SetupPacket.wValueL) {
                                USBD_EndPointMask |=  m;
                                USBD_EndPointHalt &= ~m;
                                USBD_ConfigEP((USB_ENDPOINT_DESCRIPTOR *)pD);
                                USBD_EnableEP(n);
                                USBD_ResetEP(n);
                                msk |= m;
                            } else if ((alt == old) && ((msk & m) == 0)) {
                                USBD_EndPointMask &= ~m;
                                USBD_EndPointHalt &= ~m;
                                USBD_DisableEP(n);
                            }
                        }

                        break;
                }

                pD = (USB_COMMON_DESCRIPTOR *)((U8 *)pD + pD->bLength);
            }

            break;

        default:
            return (__FALSE);
    }

    return (set);
}


/*
 *  USB Device Endpoint 0 Event Callback
 *    Parameters:      event
 *    Return Value:    none
 */

void USBD_EndPoint0(U32 event)
{
    if (event & USBD_EVT_SETUP) {
        USBD_SetupStage();
        USBD_DirCtrlEP(USBD_SetupPacket.bmRequestType.Dir);
        USBD_EP0Data.Count = USBD_SetupPacket.wLength;       /* Number of bytes to transfer */

        switch (USBD_SetupPacket.bmRequestType.Type) {
            case REQUEST_STANDARD:
                switch (USBD_SetupPacket.bRequest) {
                    case USB_REQUEST_GET_STATUS:
                        if (!USBD_ReqGetStatus()) {
                            goto stall;
                        }

                        USBD_DataInStage();
                        break;

                    case USB_REQUEST_CLEAR_FEATURE:
                        if (!USBD_ReqSetClrFeature(0)) {
                            goto stall;
                        }

                        USBD_StatusInStage();
#ifdef __RTX

                        if (__rtx) {
                            if (USBD_RTX_CoreTask) {
                                usbd_os_evt_set(USBD_EVT_CLR_FEATURE, USBD_RTX_CoreTask);
                            }
                        } else {
#endif

                            if (USBD_P_Feature_Event) {
                                USBD_P_Feature_Event();
                            }

#ifdef __RTX
                        }

#endif
                        break;

                    case USB_REQUEST_SET_FEATURE:
                        if (!USBD_ReqSetClrFeature(1)) {
                            goto stall;
                        }

                        USBD_StatusInStage();
#ifdef __RTX

                        if (__rtx) {
                            if (USBD_RTX_CoreTask) {
                                usbd_os_evt_set(USBD_EVT_SET_FEATURE, USBD_RTX_CoreTask);
                            }
                        } else {
#endif

                            if (USBD_P_Feature_Event) {
                                USBD_P_Feature_Event();
                            }

#ifdef __RTX
                        }

#endif
                        break;

                    case USB_REQUEST_SET_ADDRESS:
                        if (!USBD_ReqSetAddress()) {
                            goto stall;
                        }

                        USBD_SetAddress(USBD_DeviceAddress & 0x7F, 1);
                        USBD_StatusInStage();
                        break;

                    case USB_REQUEST_GET_DESCRIPTOR:
                        if (!USBD_ReqGetDescriptor()) {
                            goto stall;
                        }

                        USBD_DataInStage();
                        break;

                    case USB_REQUEST_SET_DESCRIPTOR:
                        goto stall;

                    case USB_REQUEST_GET_CONFIGURATION:
                        if (!USBD_ReqGetConfiguration()) {
                            goto stall;
                        }

                        USBD_DataInStage();
                        break;

                    case USB_REQUEST_SET_CONFIGURATION:
                        if (!USBD_ReqSetConfiguration()) {
                            goto stall;
                        }

                        USBD_StatusInStage();
#ifdef __RTX

                        if (__rtx) {
                            if (USBD_RTX_CoreTask) {
                                usbd_os_evt_set(USBD_EVT_SET_CFG, USBD_RTX_CoreTask);
                            }
                        } else {
#endif

                            if (USBD_P_Configure_Event) {
                                USBD_P_Configure_Event();
                            }

#ifdef __RTX
                        }

#endif
                        break;

                    case USB_REQUEST_GET_INTERFACE:
                        if (!USBD_ReqGetInterface()) {
                            goto stall;
                        }

                        USBD_DataInStage();
                        break;

                    case USB_REQUEST_SET_INTERFACE:
                        if (!USBD_ReqSetInterface()) {
                            goto stall;
                        }

                        USBD_StatusInStage();
#ifdef __RTX

                        if (__rtx) {
                            if (USBD_RTX_CoreTask) {
                                usbd_os_evt_set(USBD_EVT_SET_IF, USBD_RTX_CoreTask);
                            }
                        } else {
#endif

                            if (USBD_P_Interface_Event) {
                                USBD_P_Interface_Event();
                            }

#ifdef __RTX
                        }

#endif
                        break;

                    default:
                        goto stall;
                }

                break;  /* end case REQUEST_STANDARD */

            case REQUEST_CLASS:
                switch (USBD_SetupPacket.bmRequestType.Recipient) {
                    case REQUEST_TO_DEVICE:
                        goto stall;                                                  /* not supported */

                    case REQUEST_TO_INTERFACE:
                        if (USBD_EndPoint0_Setup_HID_ReqToIF()) {
                            goto setup_class_ok;
                        }

                        if (USBD_EndPoint0_Setup_MSC_ReqToIF()) {
                            goto setup_class_ok;
                        }

                        if (USBD_EndPoint0_Setup_CDC_ReqToIF()) {
                            goto setup_class_ok;
                        }

                        goto stall;                                                  /* not supported */

                    /* end case REQUEST_TO_INTERFACE */

                    case REQUEST_TO_ENDPOINT:
                        goto stall;

                    /* end case REQUEST_TO_ENDPOINT */

                    default:
                        goto stall;
                }

setup_class_ok:                                                          /* request finished successfully */
                break;  /* end case REQUEST_CLASS */

            case REQUEST_VENDOR:
                switch (USBD_SetupPacket.bmRequestType.Recipient) {
                    case REQUEST_TO_DEVICE:
                        if (USBD_EndPoint0_Setup_WebUSB_ReqToDevice()) {
                            goto setup_vendor_ok;
                        }

                        if (USBD_EndPoint0_Setup_WinUSB_ReqToDevice()) {
                            goto setup_vendor_ok;
                        }

                        goto stall;

                    default:
                        goto stall;
                }
setup_vendor_ok:
                break; /* end case REQUEST_VENDOR */

            default:
stall:
                if ((USBD_SetupPacket.bmRequestType.Dir == REQUEST_HOST_TO_DEVICE) &&
                        (USBD_SetupPacket.wLength != 0)) {
                    USBD_SetStallEP(0x00);
                } else {
                    USBD_SetStallEP(0x80);
                }

                USBD_EP0Data.Count = 0;
                break;
        }
    }

    if (event & USBD_EVT_OUT) {
        if (USBD_SetupPacket.bmRequestType.Dir == REQUEST_HOST_TO_DEVICE) {
            if (USBD_EP0Data.Count) {                                          /* still data to receive ? */
                USBD_DataOutStage();                                             /* receive data */

                if (USBD_EP0Data.Count == 0) {                                   /* data complete ? */
                    switch (USBD_SetupPacket.bmRequestType.Type) {
                        case REQUEST_STANDARD:
                            goto stall_i;                                              /* not supported */

                        case REQUEST_CLASS:
                            switch (USBD_SetupPacket.bmRequestType.Recipient) {
                                case REQUEST_TO_DEVICE:
                                    goto stall_i;                                          /* not supported */

                                case REQUEST_TO_INTERFACE:
                                    if (USBD_EndPoint0_Out_HID_ReqToIF()) {
                                        goto out_class_ok;
                                    }

                                    if (USBD_EndPoint0_Out_CDC_ReqToIF()) {
                                        goto out_class_ok;
                                    }

                                    goto stall_i;

                                /* end case REQUEST_TO_INTERFACE */

                                case REQUEST_TO_ENDPOINT:
                                    goto stall_i;

                                /* end case REQUEST_TO_ENDPOINT */

                                default:
                                    goto stall_i;
                            }

out_class_ok:                                                            /* request finished successfully */
                            break; /* end case REQUEST_CLASS */

                        default:
stall_i:
                            USBD_SetStallEP(0x80);
                            USBD_EP0Data.Count = 0;
                            break;
                    }
                }
            }
        } else {
            USBD_StatusOutStage();                                             /* receive Acknowledge */
        }
    }  /* end USBD_EVT_OUT */

    if (event & USBD_EVT_IN) {
        if (USBD_SetupPacket.bmRequestType.Dir == REQUEST_DEVICE_TO_HOST) {
            if (USBD_EP0Data.Count || USBD_ZLP) {
                USBD_DataInStage();    /* send data */
            }
        } else {
            if (USBD_DeviceAddress & 0x80) {
                USBD_DeviceAddress &= 0x7F;
                USBD_SetAddress(USBD_DeviceAddress, 0);
            }
        }
    }  /* end USBD_EVT_IN */

    if (event & USBD_EVT_OUT_STALL) {
        USBD_ClrStallEP(0x00);
    }

    if (event & USBD_EVT_IN_STALL) {
        USBD_ClrStallEP(0x80);
    }
}


/*
 *  USB Device Endpoint 0 RTX Task
 *    Parameters:      none
 *    Return Value:    none
 */

#ifdef __RTX
void USBD_RTX_EndPoint0(void)
{
    for (;;) {
        usbd_os_evt_wait_or(0xFFFF, 0xFFFF);
        USBD_EndPoint0(usbd_os_evt_get());
    }
}
#endif
