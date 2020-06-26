/**
 * @file    usbd_lib_cdc.h
 * @brief   USB Device CDC Library header
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

#ifndef __USBD_LIB_CDC_H__
#define __USBD_LIB_CDC_H__


/*--------------------------- USB Requests -----------------------------------*/

extern int32_t USBD_CDC_ACM_SendEncapsulatedCommand(void);
extern int32_t USBD_CDC_ACM_GetEncapsulatedResponse(void);
extern int32_t USBD_CDC_ACM_SetCommFeature(uint16_t feat);
extern int32_t USBD_CDC_ACM_GetCommFeature(uint16_t feat);
extern int32_t USBD_CDC_ACM_ClearCommFeature(uint16_t feat);
extern int32_t USBD_CDC_ACM_SetLineCoding(void);
extern int32_t USBD_CDC_ACM_GetLineCoding(void);
extern int32_t USBD_CDC_ACM_SetControlLineState(uint16_t ctrl_bmp);
extern int32_t USBD_CDC_ACM_SendBreak(uint16_t dur);


#endif  /* __USBD_LIB_CDC_H__ */
