/**
 * @file    rt_HAL_CM.h
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

/* Definitions */
#define INITIAL_xPSR    0x01000000
#define DEMCR_TRCENA    0x01000000
#define ITM_ITMENA      0x00000001
#define MAGIC_WORD      0xE25A2EA5

// ARMCC has deprecated use for ldrex and strex functions
// from C so do not used them on any devices.
#if (0)
 #define __USE_EXCLUSIVE_ACCESS
#else
 #undef  __USE_EXCLUSIVE_ACCESS
#endif

/* NVIC registers */
#define NVIC_ST_CTRL    (*((volatile U32 *)0xE000E010))
#define NVIC_ST_RELOAD  (*((volatile U32 *)0xE000E014))
#define NVIC_ST_CURRENT (*((volatile U32 *)0xE000E018))
#define NVIC_ISER         ((volatile U32 *)0xE000E100)
#define NVIC_ICER         ((volatile U32 *)0xE000E180)
#if (__TARGET_ARCH_6S_M)
#define NVIC_IP           ((volatile U32 *)0xE000E400)
#else
#define NVIC_IP           ((volatile U8  *)0xE000E400)
#endif
#define NVIC_INT_CTRL   (*((volatile U32 *)0xE000ED04))
#define NVIC_AIR_CTRL   (*((volatile U32 *)0xE000ED0C))
#define NVIC_SYS_PRI2   (*((volatile U32 *)0xE000ED1C))
#define NVIC_SYS_PRI3   (*((volatile U32 *)0xE000ED20))

#define OS_PEND_IRQ()   NVIC_INT_CTRL  = (1<<28)
#define OS_PENDING      ((NVIC_INT_CTRL >> 26) & (1<<2 | 1))
#define OS_UNPEND(fl)   NVIC_INT_CTRL  = (*fl = OS_PENDING) << 25
#define OS_PEND(fl,p)   NVIC_INT_CTRL  = (fl | p<<2) << 26
#define OS_LOCK()       NVIC_ST_CTRL   =  0x0005
#define OS_UNLOCK()     NVIC_ST_CTRL   =  0x0007

#define OS_X_PENDING    ((NVIC_INT_CTRL >> 28) & 1)
#define OS_X_UNPEND(fl) NVIC_INT_CTRL  = (*fl = OS_X_PENDING) << 27
#define OS_X_PEND(fl,p) NVIC_INT_CTRL  = (fl | p) << 28
#if (__TARGET_ARCH_6S_M)
#define OS_X_INIT(n)    NVIC_IP[n>>2] |= 0xFF << (8*(n & 0x03)); \
                        NVIC_ISER[n>>5] = 1 << (n & 0x1F)
#else
#define OS_X_INIT(n)    NVIC_IP[n] = 0xFF; \
                        NVIC_ISER[n>>5] = 1 << (n & 0x1F)
#endif
#define OS_X_LOCK(n)    NVIC_ICER[n>>5] = 1 << (n & 0x1F)
#define OS_X_UNLOCK(n)  NVIC_ISER[n>>5] = 1 << (n & 0x1F)

/* Core Debug registers */
#define DEMCR           (*((volatile U32 *)0xE000EDFC))

/* ITM registers */
#define ITM_CONTROL     (*((volatile U32 *)0xE0000E80))
#define ITM_ENABLE      (*((volatile U32 *)0xE0000E00))
#define ITM_PORT30_U32  (*((volatile U32 *)0xE0000078))
#define ITM_PORT31_U32  (*((volatile U32 *)0xE000007C))
#define ITM_PORT31_U16  (*((volatile U16 *)0xE000007C))
#define ITM_PORT31_U8   (*((volatile U8  *)0xE000007C))

/* Variables */
extern BIT dbg_msg;

/* Functions */
#ifdef __USE_EXCLUSIVE_ACCESS
 #define rt_inc(p)     while(__strex((__ldrex(p)+1),p))
 #define rt_dec(p)     while(__strex((__ldrex(p)-1),p))
#else
 #define rt_inc(p)     __disable_irq();(*p)++;__enable_irq();
 #define rt_dec(p)     __disable_irq();(*p)--;__enable_irq();
#endif

static inline U32 rt_inc_qi (U32 size, U8 *count, U8 *first) {
  U32 cnt,c2;
#ifdef __USE_EXCLUSIVE_ACCESS
  do {
    if ((cnt = __ldrex(count)) == size) {
      __clrex();
      return (cnt); }
  } while (__strex(cnt+1, count));
  do {
    c2 = (cnt = __ldrex(first)) + 1;
    if (c2 == size) c2 = 0;
  } while (__strex(c2, first));
#else
  __disable_irq();
  if ((cnt = *count) < size) {
    *count = cnt+1;
    c2 = (cnt = *first) + 1;
    if (c2 == size) c2 = 0;
    *first = c2;
  }
  __enable_irq ();
#endif
  return (cnt);
}

static inline void rt_systick_init (void) {
  NVIC_ST_RELOAD  = os_trv;
  NVIC_ST_CURRENT = 0;
  NVIC_ST_CTRL    = 0x0007;
  NVIC_SYS_PRI3  |= 0xFF000000;
}

static inline void rt_svc_init (void) {
#if !(__TARGET_ARCH_6S_M)
  int sh,prigroup;
#endif
  NVIC_SYS_PRI3 |= 0x00FF0000;
#if (__TARGET_ARCH_6S_M)
  NVIC_SYS_PRI2 |= (NVIC_SYS_PRI3<<(8+1)) & 0xFC000000;
#else
  sh       = 8 - __clz (~((NVIC_SYS_PRI3 << 8) & 0xFF000000));
  prigroup = ((NVIC_AIR_CTRL >> 8) & 0x07);
  if (prigroup >= sh) {
    sh = prigroup + 1;
  }
  NVIC_SYS_PRI2 = ((0xFEFFFFFF << sh) & 0xFF000000) | (NVIC_SYS_PRI2 & 0x00FFFFFF);
#endif
}

extern void rt_init_stack (P_TCB p_TCB, FUNCP task_body);
extern void rt_set_PSP (U32 stack);
extern U32  rt_get_PSP (void);
extern void os_set_env (void);
extern void *_alloc_box (void *box_mem);
extern int  _free_box (void *box_mem, void *box);

extern void dbg_init (void);
extern void dbg_task_notify (P_TCB p_tcb, BOOL create);
extern void dbg_task_switch (U32 task_id);

#ifdef DBG_MSG
#define DBG_INIT() dbg_init()
#define DBG_TASK_NOTIFY(p_tcb,create) if (dbg_msg) dbg_task_notify(p_tcb,create)
#define DBG_TASK_SWITCH(task_id)      if (dbg_msg && (os_tsk.new!=os_tsk.run)) \
                                                   dbg_task_switch(task_id)
#else
#define DBG_INIT()
#define DBG_TASK_NOTIFY(p_tcb,create)
#define DBG_TASK_SWITCH(task_id)
#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

