/**
 * @file    usbd_event.h
 * @brief   USB Device Event header
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

#ifndef __USBD_EVENT_H__
#define __USBD_EVENT_H__


/* USB Device - Device Events                                                 */
#define USBD_EVT_POWER_ON    (1 <<  0)  /* USB Power On                       */
#define USBD_EVT_POWER_OFF   (1 <<  1)  /* USB Power Off                      */
#define USBD_EVT_RESET       (1 <<  2)  /* USB Bus Reset                      */
#define USBD_EVT_WAKEUP      (1 <<  3)  /* USB Remote Wakeup                  */
#define USBD_EVT_SUSPEND     (1 <<  4)  /* USB Suspend                        */
#define USBD_EVT_RESUME      (1 <<  5)  /* USB Resume                         */
#define USBD_EVT_SOF         (1 <<  6)  /* USB Start of Frame                 */
#define USBD_EVT_ERROR       (1 <<  7)  /* USB Error                          */

/* USB Device - Endpoint Events                                               */
#define USBD_EVT_SETUP       (1 <<  1)  /* Setup Packet                       */
#define USBD_EVT_OUT         (1 <<  2)  /* OUT Packet                         */
#define USBD_EVT_IN          (1 <<  3)  /*  IN Packet                         */
#define USBD_EVT_OUT_NAK     (1 <<  4)  /* OUT Packet - Not Acknowledged      */
#define USBD_EVT_IN_NAK      (1 <<  5)  /*  IN Packet - Not Acknowledged      */
#define USBD_EVT_OUT_STALL   (1 <<  6)  /* OUT Packet - Stalled               */
#define USBD_EVT_IN_STALL    (1 <<  7)  /*  IN Packet - Stalled               */
#define USBD_EVT_OUT_DMA_EOT (1 <<  8)  /* DMA OUT EP - End of Transfer       */
#define USBD_EVT_IN_DMA_EOT  (1 <<  9)  /* DMA  IN EP - End of Transfer       */
#define USBD_EVT_OUT_DMA_NDR (1 << 10)  /* DMA OUT EP - New Descriptor Request*/
#define USBD_EVT_IN_DMA_NDR  (1 << 11)  /* DMA  IN EP - New Descriptor Request*/
#define USBD_EVT_OUT_DMA_ERR (1 << 12)  /* DMA OUT EP - Error                 */
#define USBD_EVT_IN_DMA_ERR  (1 << 13)  /* DMA  IN EP - Error                 */

/* USB Device - Core Events                                                   */
#define USBD_EVT_SET_CFG     (1 <<  0)  /* Set Configuration                  */
#define USBD_EVT_SET_IF      (1 <<  1)  /* Set Interface                      */
#define USBD_EVT_SET_FEATURE (1 <<  2)  /* Set Feature                        */
#define USBD_EVT_CLR_FEATURE (1 <<  3)  /* Clear Feature                      */

/* USB Device - Device Events Callback Pointers                               */
extern void (* const USBD_P_Power_Event)(BOOL power);
extern void (* const USBD_P_Reset_Event)(void);
extern void (* const USBD_P_Suspend_Event)(void);
extern void (* const USBD_P_Resume_Event)(void);
extern void (* const USBD_P_WakeUp_Event)(void);
extern void (* const USBD_P_SOF_Event)(void);
extern void (* const USBD_P_Error_Event)(U32 error);

/* USB Device - Endpoint Events Callback Pointers                             */
extern void (* const USBD_P_EP[16])(U32 event);

/* USB Device - Core Events Callback Pointers                                 */
extern void (* const USBD_P_Configure_Event)(void);
extern void (* const USBD_P_Interface_Event)(void);
extern void (* const USBD_P_Feature_Event)(void);

/* USB Device - RTX version RTX tasks initialization                          */
extern void USBD_RTX_TaskInit(void);

#endif  /* __USBD_EVENT_H__ */
