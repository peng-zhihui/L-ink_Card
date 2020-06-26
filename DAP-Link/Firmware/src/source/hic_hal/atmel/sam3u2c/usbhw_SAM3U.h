/**
 * @file    usbhw_SAM3U.h
 * @brief   
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

#ifndef __USBHW_H_SAM3U
#define __USBHW_H_SAM3U


/* USB Hardware Variables */
extern volatile BOOL USB_HighSpeed;

/* USB Hardware Functions */
extern void USB_Init(void);
extern void USB_Connect(BOOL con);
extern void USB_Reset(void);
extern void USB_Suspend(void);
extern void USB_Resume(void);
extern void USB_WakeUp(void);
extern void USB_WakeUpCfg(BOOL cfg);
extern void USB_SetAddress(U32  adr);
extern void USB_Configure(BOOL cfg);
extern void USB_ConfigEP(USB_ENDPOINT_DESCRIPTOR *pEPD);
extern void USB_DirCtrlEP(U32  dir);
extern void USB_EnableEP(U32  EPNum);
extern void USB_DisableEP(U32  EPNum);
extern void USB_ResetEP(U32  EPNum);
extern void USB_SetStallEP(U32  EPNum);
extern void USB_ClrStallEP(U32  EPNum);
extern void USB_ClearEPBuf(U32  EPNum);
extern U32  USB_ReadEP(U32  EPNum, U8 *pData);
extern U32  USB_WriteEP(U32  EPNum, U8 *pData, U32 cnt);
extern U32  USB_GetFrame(void);


#endif  /* __USBHW_H_SAM3U */
