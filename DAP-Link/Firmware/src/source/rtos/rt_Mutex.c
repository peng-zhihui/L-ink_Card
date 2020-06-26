/**
 * @file    rt_Mutex.c
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
#include "rt_List.h"
#include "rt_Task.h"
#include "rt_Mutex.h"


/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- rt_mut_init -----------------------------------*/

void rt_mut_init (OS_ID mutex) {
  /* Initialize a mutex object */
  P_MUCB p_MCB = mutex;

  p_MCB->cb_type = MUCB;
  p_MCB->prio    = 0;
  p_MCB->level   = 0;
  p_MCB->p_lnk   = NULL;
  p_MCB->owner   = NULL;
}


/*--------------------------- rt_mut_release --------------------------------*/

OS_RESULT rt_mut_release (OS_ID mutex) {
  /* Release a mutex object */
  P_MUCB p_MCB = mutex;
  P_TCB p_TCB;

  if (p_MCB->level == 0 || p_MCB->owner != os_tsk.run) {
    /* Unbalanced mutex release or task is not the owner */
    return (OS_R_NOK);
  }
  if (--p_MCB->level != 0) {
    return (OS_R_OK);
  }
  /* Restore owner task's priority. */
  os_tsk.run->prio = p_MCB->prio;
  if (p_MCB->p_lnk != NULL) {
    /* A task is waiting for mutex. */
    p_TCB = rt_get_first ((P_XCB)p_MCB);
    p_TCB->ret_val = OS_R_MUT;
    rt_rmv_dly (p_TCB);
    /* A waiting task becomes the owner of this mutex. */
    p_MCB->level     = 1;
    p_MCB->owner     = p_TCB;
    p_MCB->prio      = p_TCB->prio;
    /* Priority inversion, check which task continues. */
    if (os_tsk.run->prio >= rt_rdy_prio()) {
      rt_dispatch (p_TCB);
    }
    else {
      /* Ready task has higher priority than running task. */
      rt_put_prio (&os_rdy, os_tsk.run);
      rt_put_prio (&os_rdy, p_TCB);
      os_tsk.run->state = READY;
      p_TCB->state      = READY;
      rt_dispatch (NULL);
    }
    os_tsk.run->ret_val = OS_R_OK;
  }
  else {
    /* Check if own priority raised by priority inversion. */
    if (rt_rdy_prio() > os_tsk.run->prio) {
      rt_put_prio (&os_rdy, os_tsk.run);
      os_tsk.run->state = READY;
      rt_dispatch (NULL);
      os_tsk.run->ret_val = OS_R_OK;
    }
  }
  return (OS_R_OK);
}


/*--------------------------- rt_mut_wait -----------------------------------*/

OS_RESULT rt_mut_wait (OS_ID mutex, U16 timeout) {
  /* Wait for a mutex, continue when mutex is free. */
  P_MUCB p_MCB = mutex;

  if (p_MCB->level == 0) {
    p_MCB->owner = os_tsk.run;
    p_MCB->prio  = os_tsk.run->prio;
    goto inc;
  }
  if (p_MCB->owner == os_tsk.run) {
    /* OK, running task is the owner of this mutex. */
inc:p_MCB->level++;
    return (OS_R_OK);
  }
  /* Mutex owned by another task, wait until released. */
  if (timeout == 0) {
    return (OS_R_TMO);
  }
  /* Raise the owner task priority if lower than current priority. */
  /* This priority inversion is called priority inheritance.       */
  if (p_MCB->prio < os_tsk.run->prio) {
    p_MCB->owner->prio = os_tsk.run->prio;
    rt_resort_prio (p_MCB->owner);
  }
  if (p_MCB->p_lnk != NULL) {
    rt_put_prio ((P_XCB)p_MCB, os_tsk.run);
  }
  else {
    p_MCB->p_lnk = os_tsk.run;
    os_tsk.run->p_lnk  = NULL;
    os_tsk.run->p_rlnk = (P_TCB)p_MCB;
  }
  rt_block(timeout, WAIT_MUT);
  return (OS_R_TMO);
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

