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
#include "RTL.h"
#include "cortex_m.h"

#define MAIN_TASK_PRIORITY      (10)
#define MAIN_TASK_STACK         (800)
static uint64_t stk_main_task [MAIN_TASK_STACK / sizeof(uint64_t)];

#define TIMER_TASK_30_PRIORITY  (11)
#define TIMER_TASK_STACK        (136)
static uint64_t stk_timer_task[TIMER_TASK_STACK / sizeof(uint64_t)];

static uint32_t taskCount = 0; 
static osTimerFunc_t onlyTimerFunction = NULL;
static uint32_t timerTick = 0;

static OS_MUT onlyMutex;

osStatus_t osKernelInitialize(void)
{
    taskCount = 0;
    return osOK;
}

osThreadId_t osThreadNew(osThreadFunc_t func, void *argument, const osThreadAttr_t *attr)
{
    OS_TID tid = 0;
    //first task will init the rtx
    if (taskCount == 0) {
        os_sys_init_user((void (*)(void))func, MAIN_TASK_PRIORITY, stk_main_task, MAIN_TASK_STACK);
    }
    else {
        tid = os_tsk_create((void (*)(void))func, MAIN_TASK_PRIORITY+1);
    }
    taskCount++;
    return (osThreadId_t) tid;
}

uint32_t osThreadFlagsSet(osThreadId_t thread_id, uint32_t flags)
{
    if (cortex_in_isr()){
        isr_evt_set(flags, (OS_TID)thread_id);
    }else {
        os_evt_set(flags, (OS_TID)thread_id);
    }
    return flags;
}

osStatus_t osKernelStart(void)
{
    //first thread already started the kernel
    return osOK;
}

// Timer task
static void rt_timer_task(void)
{
    os_itv_set(timerTick);
    while (1) {
        os_itv_wait();
        if (onlyTimerFunction) {
            onlyTimerFunction(NULL);
        }
    }
}

osTimerId_t osTimerNew(osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr)
{
    OS_TID tid = 0;
    onlyTimerFunction = func;
    return (osTimerId_t)tid;
}

osStatus_t osTimerStart(osTimerId_t timer_id, uint32_t ticks)
{
    timerTick = ticks;
    OS_TID tid = os_tsk_create_user(rt_timer_task, TIMER_TASK_30_PRIORITY, (void *)stk_timer_task, TIMER_TASK_STACK);
    return osOK;
}

uint32_t osThreadFlagsWait(uint32_t flags, uint32_t options, uint32_t timeout)
{
    os_evt_wait_or(flags, timeout);
    return os_evt_get();
}

osMutexId_t osMutexNew(const osMutexAttr_t *attr)
{
    os_mut_init(onlyMutex);
    return (osMutexId_t)onlyMutex;
}

osStatus_t osMutexAcquire(osMutexId_t mutex_id, uint32_t timeout)
{
    os_mut_wait((OS_ID)mutex_id, timeout);
    return osOK;
}

osStatus_t osMutexRelease(osMutexId_t mutex_id)
{
    os_mut_release((OS_ID)mutex_id);
    return osOK;
}

osThreadId_t osThreadGetId(void)
{
    return (osThreadId_t)os_tsk_self();
}

osStatus_t osDelay(uint32_t ticks)
{
    os_dly_wait(ticks);
    return osOK;
}

uint32_t osKernelGetSysTimerCount(void)
{
    return os_time_get();
}

