/**
 * @file    usbd_core_hid.c
 * @brief   Human Interface Device driver
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


/*
 *  Get Descriptor USB Device Request - HID specific handling
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

__weak BOOL USBD_ReqGetDescriptor_HID(U8 **pD, U32 *len)
{
    switch (USBD_SetupPacket.wValueH) {
        case HID_HID_DESCRIPTOR_TYPE:
            if (USBD_SetupPacket.wIndexL != usbd_hid_if_num &&
                USBD_SetupPacket.wIndexL != usbd_webusb_if_num) {
                return (__FALSE);
            }

            if ((!usbd_hs_enable) && (USBD_HighSpeed == __TRUE)) {
                return (__FALSE);  /* High speed request but high-speed not enabled */
            }

            if (USBD_HighSpeed == __FALSE) {
                *pD = (U8 *)USBD_ConfigDescriptor;
            } else {
                *pD = (U8 *)USBD_ConfigDescriptor_HS;
            }

            USBD_EP0Data.pData = *pD + USBD_HID_DescriptorOffset;
            *len = USB_HID_DESC_SIZE;
            break;

        case HID_REPORT_DESCRIPTOR_TYPE:
            if (USBD_SetupPacket.wIndexL != usbd_hid_if_num &&
                USBD_SetupPacket.wIndexL != usbd_webusb_if_num) {
                return (__FALSE);
            }

            USBD_EP0Data.pData = (U8 *)USBD_HID_ReportDescriptor;
            *len = USBD_HID_ReportDescriptorSize;
            break;

        case HID_PHYSICAL_DESCRIPTOR_TYPE:
            return (__FALSE);    /* HID Physical Descriptor is not supported */

        default:
            return (__FALSE);
    }

    return (__TRUE);
}


/*
 *  USB Device Endpoint 0 Event Callback - HID specific handling (Setup Request To Interface)
 *    Parameters:      none
 *    Return Value:    TRUE - Setup class request ok, FALSE - Setup class request not supported
 */

__weak BOOL USBD_EndPoint0_Setup_HID_ReqToIF(void)
{
    if (USBD_SetupPacket.wIndexL == usbd_hid_if_num ||
        USBD_SetupPacket.wIndexL == usbd_webusb_if_num) {
        switch (USBD_SetupPacket.bRequest) {
            case HID_REQUEST_GET_REPORT:
                if (USBD_HID_GetReport()) {
                    if (USBD_SetupPacket.wValueH == HID_REPORT_INPUT) {
                        USBD_EP0Data.pData = &USBD_HID_InReport[1];      /* point to data to be sent (skip ReportID) */
                    } else if (USBD_SetupPacket.wValueH == HID_REPORT_FEATURE) {
                        USBD_EP0Data.pData = &USBD_HID_FeatReport[1];    /* point to data to be sent (skip ReportID) */
                    }

                    USBD_DataInStage();                                /* send requested data */
                    return (__TRUE);
                }

                break;

            case HID_REQUEST_SET_REPORT:
                if (USBD_SetupPacket.wValueH == HID_REPORT_OUTPUT) {
                    USBD_EP0Data.pData = &USBD_HID_OutReport[1];       /* out data to be received (skip ReportID) */
                } else if (USBD_SetupPacket.wValueH == HID_REPORT_FEATURE) {
                    USBD_EP0Data.pData = &USBD_HID_FeatReport[1];      /* out data to be received (skip ReportID) */
                }

                return (__TRUE);

            case HID_REQUEST_GET_IDLE:
                if (USBD_HID_GetIdle()) {
                    USBD_EP0Data.pData = USBD_EP0Buf;                  /* point to data to be sent */
                    USBD_DataInStage();                                /* send requested data */
                    return (__TRUE);
                }

                break;

            case HID_REQUEST_SET_IDLE:
                if (USBD_HID_SetIdle()) {
                    USBD_StatusInStage();                              /* send Acknowledge */
                    return (__TRUE);
                }

                break;

            case HID_REQUEST_GET_PROTOCOL:
                if (USBD_HID_GetProtocol()) {
                    USBD_EP0Data.pData = USBD_EP0Buf;                  /* point to data to be sent */
                    USBD_DataInStage();                                /* send requested data */
                    return (__TRUE);
                }

                break;

            case HID_REQUEST_SET_PROTOCOL:
                if (USBD_HID_SetProtocol()) {
                    USBD_StatusInStage();                              /* send Acknowledge */
                    return (__TRUE);
                }

                break;
        }
    }

    return (__FALSE);
}


/*
 *  USB Device Endpoint 0 Event Callback - HID specific handling (Out Request To Interface)
 *    Parameters:      none
 *    Return Value:    TRUE - Out class request ok, FALSE - Out class request not supported
 */

__weak BOOL USBD_EndPoint0_Out_HID_ReqToIF(void)
{
    if (USBD_SetupPacket.wIndexL == usbd_hid_if_num ||
        USBD_SetupPacket.wIndexL == usbd_webusb_if_num) {
        switch (USBD_SetupPacket.bRequest) {
            case HID_REQUEST_SET_REPORT:
                if (USBD_HID_SetReport()) {
                    USBD_StatusInStage();                        /* send Acknowledge */
                    return (__TRUE);
                }

                break;
        }
    }

    return (__FALSE);
}
