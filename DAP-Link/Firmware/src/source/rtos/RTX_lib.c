/**
 * @file    RTX_lib.c
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

#pragma thumb
#pragma O3

/*----------------------------------------------------------------------------
 *      Definitions
 *---------------------------------------------------------------------------*/
#if   (__TARGET_ARCH_6S_M || __TARGET_ARCH_7_M || __TARGET_ARCH_7E_M)
 #define __CM__   1
#elif (__TARGET_ARCH_7_R)
 #define __CR__   1
#else
 #define __ARM__  1
#endif

#if (__ARM__ || __CR__)
 #define runtask_id()   os_tsk_self()
 #define mutex_init(m)  os_mut_init(m)
#endif
#if (__CM__)
 #ifndef OS_TIMER
  #define OS_TIMER      0
 #endif
 #define runtask_id()   rt_tsk_self()
 #define mutex_init(m)  rt_mut_init(m)
#endif

#define mutex_wait(m)   os_mut_wait(m,0xFFFF)
#define mutex_rel(m)    os_mut_release(m)


/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/
U16 const os_maxtaskrun = OS_TASKCNT;
U32 const os_stackinfo  = (OS_STKCHECK<<24)| (OS_PRIVCNT<<16) | (OS_STKSIZE*4);
U32 const os_rrobin     = (OS_ROBIN << 16) | OS_ROBINTOUT;
#if (__CM__)
 U32 const os_trv       = OS_TRV;
#endif
#if (__CM__ || __CR__)
 U8  const os_flags     = OS_RUNPRIV;
#endif

/* Export following defines to uVision debugger. */
U32 const os_clockrate __attribute__((used)) =  OS_TICK;
U32 const os_timernum  __attribute__((used)) = (OS_TIMER << 16) | OS_TIMERCNT;

/* Memory pool for TCB allocation    */
_declare_box (mp_tcb, OS_TCB_SIZE, OS_TASKCNT);
U16 const mp_tcb_size = sizeof(mp_tcb);

/* Memory pool for System stack allocation (+ os_idle_demon). */
_declare_box8 (mp_stk, OS_STKSIZE*4, OS_TASKCNT-OS_PRIVCNT+1);
U32 const mp_stk_size = sizeof(mp_stk);

#ifndef OS_FIFOSZ
 #define OS_FIFOSZ        16
#endif

/* Fifo Queue buffer for ISR requests.*/
U32 os_fifo[OS_FIFOSZ*2+1];
U8 const os_fifo_size = OS_FIFOSZ;

/* An array of Active task pointers. */
void *os_active_TCB[OS_TASKCNT];

#if (OS_TIMERCNT != 0)
 /* Memory pool for User Timer allocation */
 _declare_box (mp_tmr, OS_TMR_SIZE, OS_TIMERCNT);
 U16 const mp_tmr_size = sizeof(mp_tmr);
 U32 const *m_tmr = &mp_tmr[0];
#else
 U32 const *m_tmr = NULL;
 U16 const mp_tmr_size = 0;
#endif

#ifndef __MICROLIB
 /* A memory space for arm standard library. */
 static U32    std_libspace[OS_TASKCNT][96/4];
 static OS_MUT std_libmutex[OS_MUTEXCNT];
 static U32    nr_mutex;
 extern void  *__libspace_start;
#endif

/*----------------------------------------------------------------------------
 *      Tick Timer configuration for ARM7/9, Cortex-R4
 *---------------------------------------------------------------------------*/

#if (__ARM__ || __CR__)

extern void os_clock_interrupt (void);

/*--------------------------- os_tmr_init -----------------------------------*/

#ifndef __OS_TMR_INIT
void os_tmr_init (void) {
  /* Initialize hardware timer as system tick timer. */
  OS_TINIT();
}
#endif

/*--------------------------- os_tmr_reload ---------------------------------*/

#if (!defined(__OS_TMR_RELOAD) && defined(OS_TREL))
void os_tmr_reload (void) {
  /* Reload system timer for next period. */
  OS_TREL();
}
#endif

/*--------------------------- os_tmr_force_irq ------------------------------*/

#ifndef __OS_TMR_FORCE_IRQ
void os_tmr_force_irq (void) {
  /* Force a timer interrupt. */
  OS_TFIRQ();
}
#endif

/*--------------------------- os_tmr_inspect_cnt ----------------------------*/

#ifndef __OS_TMR_INSPECT_CNT
U32 os_tmr_inspect_cnt (void) {
  /* Inspect current value of rtx timer. */
  return (OS_TVAL);
}
#endif

/*--------------------------- os_tmr_inspect_ovf ----------------------------*/

#ifndef __OS_TMR_INSPECT_OVF
BOOL os_tmr_inspect_ovf (void) {
  /* Inspect current state of timer overflow flag. */
  return (OS_TOVF);
}
#endif

/*--------------------------- os_irq_ack_lock -------------------------------*/

#ifndef __OS_IRQ_ACK_LOCK
void os_irq_ack_lock (void) {
  /* Acknowledge and disable a timer interrupt. */
  OS_TIACK();
  OS_LOCK();
}
#endif

/*--------------------------- tsk_lock --------------------------------------*/

#ifndef __TSK_LOCK
void __swi(5) tsk_lock (void);
void __SWI_5           (void) {
  /* Disable RTX System Tick Timer interrupts. */
  OS_LOCK();
}
#endif

/*--------------------------- tsk_unlock ------------------------------------*/

#ifndef __TSK_UNLOCK
void tsk_unlock (void) {
  /* Enable RTX System Tick Timer Interrupts. */
  OS_UNLOCK();
}
#endif

#endif /* #if (__ARM__ || __CR__) */


/*----------------------------------------------------------------------------
 *      RT Agent interface for ARM7/9
 *---------------------------------------------------------------------------*/

#if (__ARM__ && __RTA_RTX_CONFIG)

#include "RT_Agent.h"

#if (__RTA_ENABLED && __RTA_RTX_TASK_SWITCHING)
/*
 * RT Agent - Event Viewer Packing
 *
 *    |---+---------+---------+------|
 *    | 0 | os_time | hw_tick | t_id |
 *    |---+---------+---------+------|
 * bit  31          ^          7    0
 *                  +-- OS_TIME_SL
 *
 *  value hw_tick is truncated    by number of bits specified by OS_TVAL_SR
 *  value os_time is shifted left by number of bits specified by OS_TIME_SL
 */

/* Define pack shift values depending on the tick time value OS_TICK.  */
/* Packing results in a time of ~1 second being represented, and an    */
/* accuracy of ~0.15us. (based on the hw tick timer accuracy.          */

#if   ((OS_TICK) < 0x80)        // OS_TICK < 128us  ==> tick 14 bits, timer 9
 #define OS_TIME_SL     17
#elif ((OS_TICK) < 0x100)       // OS_TICK < 256us  ==> tick 13 bits, timer 10
 #define OS_TIME_SL     18
#elif ((OS_TICK) < 0x200)       // OS_TICK < 512us  ==> tick 12 bits, timer 11
 #define OS_TIME_SL     19
#elif ((OS_TICK) < 0x400)       // OS_TICK < 1.024ms ==> tick 11 bits, timer 12
 #define OS_TIME_SL     20
#elif ((OS_TICK) < 0x800)       // OS_TICK < 2.048ms ==> tick 10 bits, timer 13
 #define OS_TIME_SL     21
#elif ((OS_TICK) < 0x1000)      // OS_TICK < 4.096ms ==> tick 9 bits, timer 14
 #define OS_TIME_SL     22
#elif ((OS_TICK) < 0x2000)      // OS_TICK < 8.192ms ==> tick 8 bits, timer 15
 #define OS_TIME_SL     23
#elif ((OS_TICK) < 0x4000)      // OS_TICK < 16.384ms ==> tick 7 bits, timer 16
 #define OS_TIME_SL     24
#elif ((OS_TICK) < 0x8000)      // OS_TICK < 32.768ms ==> tick 6 bits, timer 17
 #define OS_TIME_SL     25
#elif ((OS_TICK) < 0x10000)     // OS_TICK < 65.536ms ==> tick 5 bits, timer 18
 #define OS_TIME_SL     26
#elif ((OS_TICK) < 0x20000)     // OS_TICK < 131.070ms ==> tick 4 bits, timer 19
 #define OS_TIME_SL     27
#elif ((OS_TICK) < 0x40000)     // OS_TICK < 262.140ms ==> tick 3 bits, timer 20
 #define OS_TIME_SL     28
#elif ((OS_TICK) < 0x80000)     // OS_TICK < 524.280ms ==> tick 2 bits, timer 21
 #define OS_TIME_SL     29
#else                           // OS_TICK >= 524.280ms ==> tick 1 bit, timer 22
 #define OS_TIME_SL     30
#endif

/* Define hw_tick truncation depending on the timer reload value OS_TRV */
#if ((OS_TRV) < 0x10)
 #define OS_TVAL_SR     (12 - OS_TIME_SL)
#elif ((OS_TRV) < 0x20)
 #define OS_TVAL_SR     (13 - OS_TIME_SL)
#elif ((OS_TRV) < 0x40)
 #define OS_TVAL_SR     (14 - OS_TIME_SL)
#elif ((OS_TRV) < 0x80)
 #define OS_TVAL_SR     (15 - OS_TIME_SL)
#elif ((OS_TRV) < 0x100)
 #define OS_TVAL_SR     (16 - OS_TIME_SL)
#elif ((OS_TRV) < 0x200)
 #define OS_TVAL_SR     (17 - OS_TIME_SL)
#elif ((OS_TRV) < 0x400)
 #define OS_TVAL_SR     (18 - OS_TIME_SL)
#elif ((OS_TRV) < 0x800)
 #define OS_TVAL_SR     (19 - OS_TIME_SL)
#elif ((OS_TRV) < 0x1000)
 #define OS_TVAL_SR     (20 - OS_TIME_SL)
#elif ((OS_TRV) < 0x2000)
 #define OS_TVAL_SR     (21 - OS_TIME_SL)
#elif ((OS_TRV) < 0x4000)
 #define OS_TVAL_SR     (22 - OS_TIME_SL)
#elif ((OS_TRV) < 0x8000)
 #define OS_TVAL_SR     (23 - OS_TIME_SL)
#elif ((OS_TRV) < 0x10000)
 #define OS_TVAL_SR     (24 - OS_TIME_SL)
#elif ((OS_TRV) < 0x20000)
 #define OS_TVAL_SR     (25 - OS_TIME_SL)
#elif ((OS_TRV) < 0x40000)
 #define OS_TVAL_SR     (26 - OS_TIME_SL)
#elif ((OS_TRV) < 0x80000)
 #define OS_TVAL_SR     (27 - OS_TIME_SL)
#elif ((OS_TRV) < 0x100000)
 #define OS_TVAL_SR     (28 - OS_TIME_SL)
#elif ((OS_TRV) < 0x200000)
 #define OS_TVAL_SR     (29 - OS_TIME_SL)
#elif ((OS_TRV) < 0x400000)
 #define OS_TVAL_SR     (30 - OS_TIME_SL)
#elif ((OS_TRV) < 0x800000)
 #define OS_TVAL_SR     (31 - OS_TIME_SL)
#elif ((OS_TRV) < 0x1000000)
 #define OS_TVAL_SR     (32 - OS_TIME_SL)
#elif ((OS_TRV) < 0x2000000)
 #define OS_TVAL_SR     (33 - OS_TIME_SL)
#elif ((OS_TRV) < 0x4000000)
 #define OS_TVAL_SR     (34 - OS_TIME_SL)
#elif ((OS_TRV) < 0x8000000)
 #define OS_TVAL_SR     (35 - OS_TIME_SL)
#else
 #define OS_TVAL_SR     (36 - OS_TIME_SL)
#endif

#if (OS_TVAL_SR < 0)
 #undef  OS_TVAL_SR
 #define OS_TVAL_SR     0
#endif

extern U32 os_time;

/* Export following defines to uVision debugger. */
U8  const os_time_sl __attribute__((used)) = OS_TIME_SL;
U32 const os_treload __attribute__((used)) = OS_TRV;

__weak extern void RTA_Task_Mon (unsigned int tsk);

#define RTA_CMD_RTX_TASK_INFO   0x0A

void rt_notify (U32 ptask, U32 create_tid) {
  /* Notify RTA user of a task creation/deletion. */
  U32 msg[2];

  /* (BOOL)create << 8 | task_id - parameter packed in RTX library */
  msg[0] = create_tid;
  msg[1] = (U32)ptask;
  RTA_Msg (RTA_CMD_RTX_TASK_INFO, (U32 *)&msg, 2);
}

/*--------------------------- rt_post_taskswitch ----------------------------*/

void rt_post_taskswitch (U32 task_id)  {
  U32 tsk_event;

  /* Add the current timer value (with bottom bits removed) */
  tsk_event = OS_TVAL >> OS_TVAL_SR;
  /* Shif timer value into place, add the task ID. */
  tsk_event = (tsk_event << 8) | task_id;
  /* Add the OS tick time. */
  tsk_event = tsk_event | (os_time << OS_TIME_SL);
  /* Mask off the top bit to indicate a task switch message. */
  tsk_event &= 0x7FFFFFFFU;

  RTA_Task_Mon (tsk_event);
}

#endif /* #if (__RTA_ENABLED && __RTA_RTX_TASK_SWITCHING) */

/*--------------------------- rt_init ---------------------------------------*/

void rt_init (void) {
  RTA_Init ();
}


#include "RT_Agent.c"

#endif /* #if (__ARM__ && __RTA_RTX_CONFIG) */
                                              

/*----------------------------------------------------------------------------
 *      RTX Optimizations (empty functions)
 *---------------------------------------------------------------------------*/

#if (__ARM__ || __CR__) && OS_ROBIN == 0
 void os_init_robin (void) {;}
 void os_chk_robin  (void) {;}
#endif

#if (__ARM__ || __CR__) && OS_STKCHECK == 0
 void os_stk_check  (U32 stk) {;}
#endif

#if (__CM__) && OS_ROBIN == 0
 void rt_init_robin (void) {;}
 void rt_chk_robin  (void) {;}
#endif

#if (__CM__) && OS_STKCHECK == 0
 void rt_stk_check  (void) {;}
#endif


/*----------------------------------------------------------------------------
 *      Standard Library multithreading interface
 *---------------------------------------------------------------------------*/

#ifndef __MICROLIB

/*--------------------------- __user_perthread_libspace ---------------------*/

void *__user_perthread_libspace (void) {
  /* Provide a separate libspace for each task. */
  U32 idx;

  idx = runtask_id ();
  if (idx == 0) {
    /* RTX not running yet. */
    return (&__libspace_start);
  }
  return ((void *)&std_libspace[idx-1]);
}

/*--------------------------- _mutex_initialize -----------------------------*/

int _mutex_initialize (OS_ID *mutex) {
  /* Allocate and initialize a system mutex. */

  if (nr_mutex >= OS_MUTEXCNT) {
    /* If you are here, you need to increase the number OS_MUTEXCNT. */
    for (;;);
  }
  *mutex = &std_libmutex[nr_mutex++];
  mutex_init (*mutex);
  return (1);
}


/*--------------------------- _mutex_acquire --------------------------------*/

__used void _mutex_acquire (OS_ID *mutex) {
  /* Acquire a system mutex, lock stdlib resources. */
  if (runtask_id ()) {
    /* RTX running, acquire a mutex. */
    mutex_wait (*mutex);
  }
}


/*--------------------------- _mutex_release --------------------------------*/

__used void _mutex_release (OS_ID *mutex) {
  /* Release a system mutex, unlock stdlib resources. */
  if (runtask_id ()) {
    /* RTX runnning, release a mutex. */
    mutex_rel (*mutex);
  }
}

#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

