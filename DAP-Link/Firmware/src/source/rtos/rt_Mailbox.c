/**
 * @file    rt_Mailbox.c
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
#include "rt_System.h"
#include "rt_List.h"
#include "rt_Mailbox.h"
#include "rt_Task.h"
#include "rt_HAL_CM.h"


/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- rt_mbx_init -----------------------------------*/

void rt_mbx_init (OS_ID mailbox, U16 mbx_size) {
  /* Initialize a mailbox */
  P_MCB p_MCB = mailbox;

  p_MCB->cb_type = MCB;
  p_MCB->isr_st  = 0;
  p_MCB->p_lnk   = NULL;
  p_MCB->first   = 0;
  p_MCB->last    = 0;
  p_MCB->count   = 0;
  p_MCB->size    = (mbx_size + sizeof(void *) - sizeof(struct OS_MCB)) /
                                                     (U32)sizeof (void *);
}


/*--------------------------- rt_mbx_send -----------------------------------*/

OS_RESULT rt_mbx_send (OS_ID mailbox, void *p_msg, U16 timeout) {
  /* Send message to a mailbox */
  P_MCB p_MCB = mailbox;
  P_TCB p_TCB;

  if (p_MCB->p_lnk != NULL && p_MCB->count == 0) {
    /* A task is waiting for message */
    p_TCB = rt_get_first ((P_XCB)p_MCB);
    *p_TCB->msg    = p_msg;
    p_TCB->ret_val = OS_R_MBX;
    rt_rmv_dly (p_TCB);
    rt_dispatch (p_TCB);
    os_tsk.run->ret_val = OS_R_OK;
  }
  else {
    /* Store message in mailbox queue */
    if (p_MCB->count == p_MCB->size) {
      /* No free message entry, wait for one. If message queue is full, */
      /* then no task is waiting for message. The 'p_MCB->p_lnk' list   */
      /* pointer can now be reused for send message waits task list.    */
      if (timeout == 0) {
        return (OS_R_TMO);
      }
      if (p_MCB->p_lnk != NULL) {
        rt_put_prio ((P_XCB)p_MCB, os_tsk.run);
      }
      else {
        p_MCB->p_lnk = os_tsk.run;
        os_tsk.run->p_lnk  = NULL;
        os_tsk.run->p_rlnk = (P_TCB)p_MCB;
        /* Signal the 'isr_mbx_receive ()' that the task is waiting */
        /* to send a message */
        p_MCB->isr_st = 1;
      }
      os_tsk.run->msg = p_msg;
      rt_block (timeout, WAIT_MBX);
      return (OS_R_TMO);
    }
    /* Yes, there is a free entry in a mailbox. */
    p_MCB->msg[p_MCB->first] = p_msg;
    rt_inc (&p_MCB->count);
    if (++p_MCB->first == p_MCB->size) {
      p_MCB->first = 0;
    }
  }
  return (OS_R_OK);
}


/*--------------------------- rt_mbx_wait -----------------------------------*/

OS_RESULT rt_mbx_wait (OS_ID mailbox, void **message, U16 timeout) {
  /* Receive a message; possibly wait for it */
  P_MCB p_MCB = mailbox;
  P_TCB p_TCB;

  /* If a message is available in the fifo buffer */
  /* remove it from the fifo buffer and return. */
  if (p_MCB->count) {
    *message = p_MCB->msg[p_MCB->last];
    if (++p_MCB->last == p_MCB->size) {
      p_MCB->last = 0;
    }
    if (p_MCB->p_lnk != NULL) {
      /* A task is waiting to send message */
      p_TCB = rt_get_first ((P_XCB)p_MCB);
      p_TCB->ret_val = OS_R_OK;
      p_MCB->msg[p_MCB->first] = p_TCB->msg;
      if (++p_MCB->first == p_MCB->size) {
        p_MCB->first = 0;
      }
      rt_rmv_dly (p_TCB);
      rt_dispatch (p_TCB);
      os_tsk.run->ret_val = OS_R_OK;
    }
    else {
      rt_dec (&p_MCB->count);
    }
    return (OS_R_OK);
  }
  /* No message available: wait for one */
  if (timeout == 0) {
    return (OS_R_TMO);
  }
  if (p_MCB->p_lnk != NULL) {
    rt_put_prio ((P_XCB)p_MCB, os_tsk.run);
  }
  else {
    p_MCB->p_lnk = os_tsk.run;
    os_tsk.run->p_lnk = NULL;
    os_tsk.run->p_rlnk = (P_TCB)p_MCB;
  }
  rt_block(timeout, WAIT_MBX);
  os_tsk.run->msg = message;
  return (OS_R_TMO);
}


/*--------------------------- rt_mbx_check ----------------------------------*/

OS_RESULT rt_mbx_check (OS_ID mailbox) {
  /* Check for free space in a mailbox. Returns the number of messages     */
  /* that can be stored to a mailbox. It returns 0 when mailbox is full.   */
  P_MCB p_MCB = mailbox;

  return (p_MCB->size - p_MCB->count);
}


/*--------------------------- isr_mbx_send ----------------------------------*/

void isr_mbx_send (OS_ID mailbox, void *p_msg) {
  /* Same function as "os_mbx_send", but to be called by ISRs. */
  P_MCB p_MCB = mailbox;

  rt_psq_enq (p_MCB, (U32)p_msg);
  rt_psh_req ();
}


/*--------------------------- isr_mbx_receive -------------------------------*/

OS_RESULT isr_mbx_receive (OS_ID mailbox, void **message) {
  /* Receive a message in the interrupt function. The interrupt function   */
  /* should not wait for a message since this would block the rtx os.      */
  P_MCB p_MCB = mailbox;

  if (p_MCB->count) {
    /* A message is available in the fifo buffer. */
    *message = p_MCB->msg[p_MCB->last];
    if (p_MCB->isr_st == 1) {
      /* A task is locked waiting to send message */
      p_MCB->isr_st = 2;
      rt_psq_enq (p_MCB, 0);
      rt_psh_req ();
    }
    rt_dec (&p_MCB->count);
    if (++p_MCB->last == p_MCB->size) {
      p_MCB->last = 0;
    }
    return (OS_R_MBX);
  }
  return (OS_R_OK);
}


/*--------------------------- rt_mbx_psh ------------------------------------*/

void rt_mbx_psh (P_MCB p_CB, void *p_msg) {
  /* Store the message to the mailbox queue or pass it to task directly. */
  P_TCB p_TCB;

  /* Check if this was an 'isr_mbx_receive ()' post service request.   */
  if (p_CB->p_lnk != NULL && p_CB->isr_st == 2) {
    /* A task is waiting to send message, remove it from the waiting list. */
    p_CB->isr_st = 0;
    p_TCB = rt_get_first ((P_XCB)p_CB);
    p_TCB->ret_val = OS_R_OK;
    /* Store the message to the mailbox queue. */
    p_CB->msg[p_CB->first] = p_TCB->msg;
    rt_inc (&p_CB->count);
    if (++p_CB->first == p_CB->size) {
      p_CB->first = 0;
    }
    goto rdy;
  }
  /* A task is waiting for message, pass the message to task directly. */
  if (p_CB->p_lnk != NULL && p_CB->count == 0) {
    p_TCB = rt_get_first ((P_XCB)p_CB);
    *p_TCB->msg = p_msg;
    p_TCB->ret_val = OS_R_MBX;
rdy:p_TCB->state = READY;
    rt_rmv_dly (p_TCB);
    rt_put_prio (&os_rdy, p_TCB);
  }
  else {
    /* No task is waiting for message, store the message to the mailbox queue.*/
    if (p_CB->count < p_CB->size) {
      p_CB->msg[p_CB->first] = p_msg;
      rt_inc (&p_CB->count);
      if (++p_CB->first == p_CB->size) {
        p_CB->first = 0;
      }
    }
    else {
      os_error (OS_ERR_MBX_OVF);
    }
  }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

