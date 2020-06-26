/**
 * @file    cmsis_os2_port.c
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
#include "cmsis_os2.h"
#include "SysTick_Handler.h"

osStatus_t osKernelInitialize(void)
{
    sysTickInit();
    return osOK;
}

osThreadId_t osThreadNew(osThreadFunc_t func, void *argument, const osThreadAttr_t *attr)
{
    sysTickRegMainFunc(func);
    return (osThreadId_t)1;
}

osStatus_t osKernelStart(void)
{
    sysTickStartMain();
    return osOK;
}

uint32_t osThreadFlagsSet(osThreadId_t thread_id, uint32_t flags)
{
    sysTickEvtSet(flags);
    return flags;
}

osStatus_t osDelay(uint32_t ticks)
{
    sysTickWait(ticks);
    return osOK;
}

osTimerId_t osTimerNew(osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr)
{
    sysTickRegCallback(func);
    return (osTimerId_t)1;
}

osStatus_t osTimerStart(osTimerId_t timer_id, uint32_t ticks)
{
    sysTickFreqSet(ticks);
    return osOK;
}

uint32_t osThreadFlagsWait(uint32_t flags, uint32_t options, uint32_t timeout)
{
    return sysTickEvtWaitOr(flags);
}

osMutexId_t osMutexNew(const osMutexAttr_t *attr)
{
    return NULL;
}

osStatus_t osMutexAcquire (osMutexId_t mutex_id, uint32_t timeout)
{
    return osOK;
}

osStatus_t osMutexRelease (osMutexId_t mutex_id)
{
    return osOK;
}

osThreadId_t osThreadGetId (void)
{
    return (osThreadId_t)1;
}
