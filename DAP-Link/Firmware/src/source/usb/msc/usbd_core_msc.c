/**
 * @file    usbd_core_msc.c
 * @brief   Mass Storage Class driver
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
 *  Clear Feature USB Device Request - MSC specific handling
 *    Parameters:      EPNum: Endpoint number
 *    Return Value:    None
 */

__weak void USBD_ReqClrFeature_MSC(U32 EPNum)
{
    USBD_MSC_ClrStallEP(EPNum);
}


/*
 *  USB Device Endpoint 0 Event Callback - MSC specific handling (Setup Request To Interface)
 *    Parameters:      none
 *    Return Value:    TRUE - Setup class request ok, FALSE - Setup class request not supported
 */

__weak BOOL USBD_EndPoint0_Setup_MSC_ReqToIF(void)
{
    if (USBD_SetupPacket.wIndexL == usbd_msc_if_num) {         /* IF number correct? */
        switch (USBD_SetupPacket.bRequest) {
            case MSC_REQUEST_RESET:
                if ((USBD_SetupPacket.wValue   == 0) &&              /* RESET with invalid parameters -> STALL */
                        (USBD_SetupPacket.wLength  == 0)) {
                    if (USBD_MSC_Reset()) {
                        USBD_StatusInStage();
                        return (__TRUE);
                    }
                }

                break;

            case MSC_REQUEST_GET_MAX_LUN:
                if ((USBD_SetupPacket.wValue   == 0) &&              /* GET_MAX_LUN with invalid parameters -> STALL */
                        (USBD_SetupPacket.wLength  == 1)) {
                    if (USBD_MSC_GetMaxLUN()) {
                        USBD_EP0Data.pData = USBD_EP0Buf;
                        USBD_DataInStage();
                        return (__TRUE);
                    }
                }

                break;
        }
    }

    return (__FALSE);
}
