/**
 * @file    usbd_core_winusb.c
 * @brief   WinUSB Device driver
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
 *  USB Device Endpoint 0 Event Callback - WinUSB specific handling (Setup Request To Device)
 *    Parameters:      none
 *    Return Value:    TRUE - Setup vendor request ok, FALSE - Setup vendor request not supported
 */

__weak BOOL USBD_EndPoint0_Setup_WinUSB_ReqToDevice(void)
{
    U8  *pD;
    U32 len;

    BOOL success = (__FALSE);
    if (USBD_SetupPacket.bRequest == usbd_winusb_vendor_code) {			/* vendor code correct? */
        switch (USBD_SetupPacket.wIndex) {
            case WINUSB_REQUEST_GET_DESCRIPTOR_SET:
                pD = (U8 *)USBD_WinUSBDescriptorSetDescriptor;
                USBD_EP0Data.pData = pD;
                len = ((WINUSB_DESCRIPTOR_SET_HEADER *)pD)->wTotalLength;
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
