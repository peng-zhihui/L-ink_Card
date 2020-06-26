/**
 * @file    usbd_core_webusb.c
 * @brief   WebUSB Device driver
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

/*
 *  USB Device Endpoint 0 Event Callback - WebUSB specific handling (Setup Request To Device)
 *    Parameters:      none
 *    Return Value:    TRUE - Setup vendor request ok, FALSE - Setup vendor request not supported
 */

__weak BOOL USBD_EndPoint0_Setup_WebUSB_ReqToDevice(void)
{
    U8  *pD;
    U32 len, n;

    BOOL success = (__FALSE);
    if (USBD_SetupPacket.bRequest == usbd_webusb_vendor_code) {			/* vendor code correct? */
        switch (USBD_SetupPacket.wIndex) {
            case WEBUSB_REQUEST_GET_URL:
                pD = (U8 *)USBD_WebUSBURLDescriptor;
                if (USBD_SetupPacket.wValueL == 0) {
                    success = (__FALSE);
                    break;
                }

                for (n = 0; n + 1 < USBD_SetupPacket.wValueL; n++) {
                    if (((WEBUSB_URL_DESCRIPTOR *)pD)->bLength != 0) {
                        pD += ((WEBUSB_URL_DESCRIPTOR *)pD)->bLength;
                    }
                }

                if (((WEBUSB_URL_DESCRIPTOR *)pD)->bLength == 0) {
                    success = (__FALSE);
                    break;
                }
                strcat(((WEBUSB_URL_DESCRIPTOR *)pD)->URL, info_get_board_id());
                USBD_EP0Data.pData = pD;
                len = ((WEBUSB_URL_DESCRIPTOR *)pD)->bLength;

                success = (__TRUE);
                break;

            default:
                break;
        }
    }

    if (success) {
        if (len < USBD_SetupPacket.wLength) {
            USBD_EP0Data.Count = len;
            if (!(len & (usbd_max_packet0 - 1))) {
                USBD_ZLP = 1;
            }
        } else {
            USBD_EP0Data.Count = USBD_SetupPacket.wLength;
        }

        USBD_DataInStage();
    }

    return success;
}
