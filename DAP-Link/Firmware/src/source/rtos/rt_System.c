/**
 * @file    rt_System.c
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

#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_Task.h"
#include "rt_System.h"
#include "rt_Event.h"
#include "rt_List.h"
#include "rt_Mailbox.h"
#include "rt_Semaphore.h"
#include "rt_Time.h"
#include "rt_Timer.h"
#include "rt_Robin.h"
#include "rt_HAL_CM.h"

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

int os_tick_irqn;

/*----------------------------------------------------------------------------
 *      Local Variables
 *---------------------------------------------------------------------------*/

static volatile BIT os_lock;
static volatile BIT os_psh_flag;
static          U8  pend_flags;

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

__asm void $$RTX$$version (void) {
   /* Export a version number symbol for a version control. */

                EXPORT  __RL_RTX_VER

__RL_RTX_VER    EQU     0x470
}


/*--------------------------- rt_suspend ------------------------------------*/

U32 rt_suspend (void) {
  /* Suspend OS scheduler */
  U32 delta = 0xFFFF;

  rt_tsk_lock();

  if (os_dly.p_dlnk) {
    delta = os_dly.delta_time;
  }
  if (os_tmr.next) {
    if (os_tmr.tcnt < delta) delta = os_tmr.tcnt;
  }

  return (delta);
}


/*--------------------------- rt_resume -------------------------------------*/

void rt_resume (U32 sleep_time) {
  /* Resume OS scheduler after suspend */
  P_TCB next;
  U32   delta;

  os_tsk.run->state = READY;
  rt_put_rdy_first (os_tsk.run);

  os_robin.task = NULL;

  /* Update delays. */
  if (os_dly.p_dlnk) {
    delta = sleep_time;
    if (delta >= os_dly.delta_time) {
      delta   -= os_dly.delta_time;
      os_time += os_dly.delta_time;
      os_dly.delta_time = 1;
      while (os_dly.p_dlnk) {
        rt_dec_dly();
        if (delta == 0) break;
        delta--;
        os_time++;
      }
    } else {
      os_time           += delta;
      os_dly.delta_time -= delta;
    }
  } else {
    os_time += sleep_time;
  }

  /* Check the user timers. */
  if (os_tmr.next) {
    delta = sleep_time;
    if (delta >= os_tmr.tcnt) {
      delta   -= os_tmr.tcnt;
      os_tmr.tcnt = 1;
      while (os_tmr.next) {
        rt_tmr_tick();
        if (delta == 0) break;
        delta--;
      }
    } else {
      os_tmr.tcnt -= delta;
    }
  }

  /* Switch back to highest ready task */
  next = rt_get_first (&os_rdy);
  rt_switch_req (next);

  rt_tsk_unlock();
}


/*--------------------------- rt_tsk_lock -----------------------------------*/

void rt_tsk_lock (void) {
  /* Prevent task switching by locking out scheduler */
  if (os_tick_irqn < 0) {
    OS_LOCK();
    os_lock = __TRUE;
    OS_UNPEND (&pend_flags);
  } else {
    OS_X_LOCK(os_tick_irqn);
    os_lock = __TRUE;
    OS_X_UNPEND (&pend_flags);
  }
}


/*--------------------------- rt_tsk_unlock ---------------------------------*/

void rt_tsk_unlock (void) {
  /* Unlock scheduler and re-enable task switching */
  if (os_tick_irqn < 0) {
    OS_UNLOCK();
    os_lock = __FALSE;
    OS_PEND (pend_flags, os_psh_flag);
    os_psh_flag = __FALSE;
  } else {
    OS_X_UNLOCK(os_tick_irqn);
    os_lock = __FALSE;
    OS_X_PEND (pend_flags, os_psh_flag);
    os_psh_flag = __FALSE;
  }
}


/*--------------------------- rt_psh_req ------------------------------------*/

void rt_psh_req (void) {
  /* Initiate a post service handling request if required. */
  if (os_lock == __FALSE) {
    OS_PEND_IRQ ();
  }
  else {
    os_psh_flag = __TRUE;
  }
}


/*--------------------------- rt_pop_req ------------------------------------*/

void rt_pop_req (void) {
  /* Process an ISR post service requests. */
  struct OS_XCB *p_CB;
  P_TCB next;
  U32  idx;

  os_tsk.run->state = READY;
  rt_put_rdy_first (os_tsk.run);

  idx = os_psq->last;
  while (os_psq->count) {
    p_CB = os_psq->q[idx].id;
    if (p_CB->cb_type == TCB) {
      /* Is of TCB type */
      rt_evt_psh ((P_TCB)p_CB, (U16)os_psq->q[idx].arg);
    }
    else if (p_CB->cb_type == MCB) {
      /* Is of MCB type */
      rt_mbx_psh ((P_MCB)p_CB, (void *)os_psq->q[idx].arg);
    }
    else {
      /* Must be of SCB type */
      rt_sem_psh ((P_SCB)p_CB);
    }
    if (++idx == os_psq->size) idx = 0;
    rt_dec (&os_psq->count);
  }
  os_psq->last = idx;

  next = rt_get_first (&os_rdy);
  rt_switch_req (next);
}


/*--------------------------- os_tick_init ----------------------------------*/

__weak int os_tick_init (void) {
  /* Initialize SysTick timer as system tick timer. */
  rt_systick_init ();
  return (-1);  /* Return IRQ number of SysTick timer */
}


/*--------------------------- os_tick_irqack --------------------------------*/

__weak void os_tick_irqack (void) {
  /* Acknowledge timer interrupt. */
}


/*--------------------------- rt_systick ------------------------------------*/

void rt_systick (void) {
  /* Check for system clock update, suspend running task. */
  P_TCB next;

  os_tsk.run->state = READY;
  rt_put_rdy_first (os_tsk.run);

  /* Check Round Robin timeout. */
  rt_chk_robin ();

  /* Update delays. */
  os_time++;
  rt_dec_dly ();

  /* Check the user timers. */
  rt_tmr_tick ();

  /* Switch back to highest ready task */
  next = rt_get_first (&os_rdy);
  rt_switch_req (next);
}

/*--------------------------- rt_stk_check ----------------------------------*/

__weak void rt_stk_check (void) {
  /* Check for stack overflow. */
  if ((os_tsk.run->tsk_stack < (U32)os_tsk.run->stack) ||
      (os_tsk.run->stack[0] != MAGIC_WORD)) {
    os_error (OS_ERR_STK_OVF);
  }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

