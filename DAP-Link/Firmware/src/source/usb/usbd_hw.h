/**
 * @file    usbd_hw.h
 * @brief   USB Device Hardware header
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

#ifndef __USBD_HW_H__
#define __USBD_HW_H__


/* USB Hardware Functions */
extern void USBD_Init(void);
extern void USBD_Connect(BOOL con);
extern void USBD_Reset(void);
extern void USBD_Suspend(void);
extern void USBD_Resume(void);
extern void USBD_WakeUp(void);
extern void USBD_WakeUpCfg(BOOL cfg);
extern void USBD_SetAddress(U32 adr, U32 setup);
extern void USBD_Configure(BOOL cfg);
extern void USBD_ConfigEP(USB_ENDPOINT_DESCRIPTOR *pEPD);
extern void USBD_DirCtrlEP(U32 dir);
extern void USBD_EnableEP(U32 EPNum);
extern void USBD_DisableEP(U32 EPNum);
extern void USBD_ResetEP(U32 EPNum);
extern void USBD_SetStallEP(U32 EPNum);
extern void USBD_ClrStallEP(U32 EPNum);
extern void USBD_ClearEPBuf(U32 EPNum);
extern U32 USBD_ReadEP(U32 EPNum, U8 *pData, U32 cnt);
extern U32 USBD_WriteEP(U32 EPNum, U8 *pData, U32 cnt);
extern U32 USBD_GetFrame(void);
extern U32 USBD_GetError(void);
extern void USBD_SignalHandler(void);
extern void USBD_Handler(void);

#endif  /* __USBD_HW_H__ */
