/**
 * @file    RTX_Config.c
 * @brief   Project settings for the RTX kernel
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

#include "RTL.h"
#include "util.h"
#include "cortex_m.h"

/*----------------------------------------------------------------------------
 *      RTX User configuration part BEGIN
 *---------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>Task Configuration
// =====================
//
//   <o>Number of concurrent running tasks <0-250>
//   <i> Define max. number of tasks that will run at the same time.
//   <i> Default: 6
#ifndef OS_TASKCNT
#define OS_TASKCNT    4
// Threads with user provided stacks:
// -serial_process
// -hid_process
// -timer_task_30mS
// -main_task
#endif

//   <o>Number of tasks with user-provided stack <0-250>
//   <i> Define the number of tasks that will use a bigger stack.
//   <i> The memory space for the stack is provided by the user.
//   <i> Default: 0
#ifndef OS_PRIVCNT
#define OS_PRIVCNT     OS_TASKCNT
// All tasks use private stacks (aside from background thread)
#endif

//   <o>Task stack size [bytes] <20-4096:8><#/4>
//   <i> Set the stack size for tasks which is assigned by the system.
//   <i> Default: 200
#ifndef OS_STKSIZE
#define OS_STKSIZE     34
// Used by:
// -os_idle_demon
#endif

// <q>Check for the stack overflow
// ===============================
// <i> Include the stack checking code for a stack overflow.
// <i> Note that additional code reduces the Kernel performance.
#ifndef OS_STKCHECK
#define OS_STKCHECK    1
#endif

// <q>Run in privileged mode
// =========================
// <i> Run all Tasks in privileged mode.
// <i> Default: Unprivileged
#ifndef OS_RUNPRIV
#define OS_RUNPRIV     1
#endif

// </h>
// <h>SysTick Timer Configuration
// =============================
//   <o>Timer clock value [Hz] <1-1000000000>
//   <i> Set the timer clock value for selected timer.
#ifndef OS_CLOCK
#error "OS_CLOCK should be defined by HIC configuration"
#endif

//   <o>Timer tick value [us] <1-1000000>
//   <i> Set the timer tick value for selected timer.
#ifndef OS_TICK
#error "OS_TICK should be defined by RTOS configuration"
#endif

// </h>

// <h>System Configuration
// =======================
// <e>Round-Robin Task switching
// =============================
// <i> Enable Round-Robin Task switching.
#ifndef OS_ROBIN
#define OS_ROBIN       0
#endif

//   <o>Round-Robin Timeout [ticks] <1-1000>
//   <i> Define how long a task will execute before a task switch.
//   <i> Default: 5
#ifndef OS_ROBINTOUT
#define OS_ROBINTOUT   10
#endif

// </e>

//   <o>Number of user timers <0-250>
//   <i> Define max. number of user timers that will run at the same time.
//   <i> Default: 0  (User timers disabled)
#ifndef OS_TIMERCNT
#define OS_TIMERCNT    0
#endif

//   <o>ISR FIFO Queue size<4=>   4 entries  <8=>   8 entries
//                         <12=> 12 entries  <16=> 16 entries
//                         <24=> 24 entries  <32=> 32 entries
//                         <48=> 48 entries  <64=> 64 entries
//                         <96=> 96 entries
//   <i> ISR functions store requests to this buffer,
//   <i> when they are called from the iterrupt handler.
//   <i> Default: 16 entries
#ifndef OS_FIFOSZ
#define OS_FIFOSZ      4
#endif

// </h>

//------------- <<< end of configuration section >>> -----------------------

// Standard library system mutexes
// ===============================
//  Define max. number system mutexes that are used to protect
//  the arm standard runtime library. For microlib they are not used.
#ifndef OS_MUTEXCNT
#define OS_MUTEXCNT    1
#endif

/*----------------------------------------------------------------------------
 *      RTX User configuration part END
 *---------------------------------------------------------------------------*/

#define OS_TRV          ((U32)(((double)OS_CLOCK*(double)OS_TICK)/1E6)-1)

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

/*--------------------------- os_idle_demon ---------------------------------*/

__task void os_idle_demon(void)
{
    /* The idle demon is a system task, running when no other task is ready */
    /* to run. The 'os_xxx' function calls are not allowed from this task.  */
    for (;;) {
        /* HERE: include optional user code to be executed when no task runs.*/
    }
}


/*--------------------------- os_tmr_call -----------------------------------*/

void os_tmr_call(U16 info)
{
    /* This function is called when the user timer has expired. Parameter   */
    /* 'info' holds the value, defined when the timer was created.          */
    /* HERE: include optional user code to be executed on timeout. */
}


/*--------------------------- os_error --------------------------------------*/

void os_error(U32 err_code)
{
    /* This function is called when a runtime error is detected. Parameter */
    /* 'err_code' holds the runtime error code (defined in RTL.H).         */
    switch (err_code) {
        case OS_ERR_STK_OVF:
            util_assert(0);
            break;

        case OS_ERR_FIFO_OVF:
            util_assert(0);
            break;

        case OS_ERR_MBX_OVF:
            util_assert(0);
            break;

        default:
            util_assert(0);
            break;
    }

    SystemReset();

    for (;;); // Wait for reset
}


/*----------------------------------------------------------------------------
 *      RTX Configuration Functions
 *---------------------------------------------------------------------------*/

#include "RTX_lib.c"

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
