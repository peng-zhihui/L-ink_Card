/**
 * @file    usbd_core.h
 * @brief   USB Device Core header
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

#ifndef __USBD_CORE_H__
#define __USBD_CORE_H__


/*--------------------------- Data structures --------------------------------*/

/* USB Device Core Endpoint Data Structure */
typedef struct _USBD_EP_DATA {
    U8 *pData;
    U16 Count;
} USBD_EP_DATA;


/*--------------------------- Global variables -------------------------------*/

/* USB Device Core Global Variables */
extern U16 USBD_DeviceStatus;
extern U8 USBD_DeviceAddress;
extern U8 USBD_Configuration;
extern U32 USBD_EndPointMask;
extern U32 USBD_EndPointHalt;
extern U32 USBD_EndPointStall;
extern U8 USBD_NumInterfaces;
extern U8 USBD_HighSpeed;
extern U8 USBD_ZLP;

extern USBD_EP_DATA USBD_EP0Data;
extern USB_SETUP_PACKET USBD_SetupPacket;

#ifdef __RTX
extern OS_TID USBD_RTX_DevTask;
extern OS_TID USBD_RTX_EPTask[];
extern OS_TID USBD_RTX_CoreTask;
#endif

/*--------------------------- Functions exported to class specific files -----*/

extern void USBD_SetupStage(void);
extern void USBD_DataInStage(void);
extern void USBD_DataOutStage(void);
extern void USBD_StatusInStage(void);
extern void USBD_StatusOutStage(void);


/*--------------------------- Event handling routines ------------------------*/

extern void usbd_class_init(void);
extern void USBD_EndPoint0(U32 event);

#ifdef __RTX
extern void USBD_RTX_EndPoint0(void);
#endif

#endif  /* __USBD_CORE_H__ */
