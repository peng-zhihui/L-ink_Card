/**
 * @file    Systick_Handler.h
 * @brief   
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2019, ARM Limited, All Rights Reserved
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

#ifndef SYSTICK_HANDLER_H_
#define SYSTICK_HANDLER_H_

#include "cmsis_os2.h"


#ifdef  __cplusplus
extern "C"
{
#endif

//functions based on system tick
void sysTickInit(void);
void sysTickRegCallback(osTimerFunc_t callback);
void sysTickFreqSet(uint32_t ticks);
void sysTickWait(uint32_t wait);
void sysTickEvtSet(uint32_t flag);
uint32_t sysTickEvtWaitOr(uint32_t flag);
uint32_t sysTickTime(void);
void sysTickRegMainFunc(osThreadFunc_t func);
void sysTickStartMain(void);

#ifdef  __cplusplus
}
#endif

#endif
