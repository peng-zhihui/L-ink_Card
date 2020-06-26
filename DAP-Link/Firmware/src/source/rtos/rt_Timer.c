/**
 * @file    rt_Timer.c
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
#include "rt_Timer.h"
#include "rt_MemBox.h"


/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

/* User Timer list pointer */
struct OS_XTMR os_tmr;

/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/

/*--------------------------- rt_tmr_tick -----------------------------------*/

void rt_tmr_tick (void) {
  /* Decrement delta count of timer list head. Timers having the value of   */
  /* zero are removed from the list and the callback function is called.    */
  P_TMR p;

  if (os_tmr.next == NULL) {
    return;
  }
  os_tmr.tcnt--;
  while (os_tmr.tcnt == 0 && (p = os_tmr.next) != NULL) {
    /* Call a user provided function to handle an elapsed timer */
    os_tmr_call (p->info);
    os_tmr.tcnt = p->tcnt;
    os_tmr.next = p->next;
    rt_free_box ((U32 *)m_tmr, p);
  }
}

/*--------------------------- rt_tmr_create ---------------------------------*/

OS_ID rt_tmr_create (U16 tcnt, U16 info)  {
  /* Create an user timer and put it into the chained timer list using      */
  /* a timeout count value of "tcnt". User parameter "info" is used as a    */
  /* parameter for the user provided callback function "os_tmr_call ()".    */
  P_TMR p_tmr, p;
  U32 delta,itcnt = tcnt;

  if (tcnt == 0 || m_tmr == NULL)  {
    return (NULL);
  }
  p_tmr = rt_alloc_box ((U32 *)m_tmr);
  if (!p_tmr)  {
    return (NULL);
  }
  p_tmr->info = info;
  p = (P_TMR)&os_tmr;
  delta = p->tcnt;
  while (delta < itcnt && p->next != NULL) {
    p = p->next;
    delta += p->tcnt;
  }
  /* Right place found, insert timer into the list */
  p_tmr->next = p->next;
  p_tmr->tcnt = (U16)(delta - itcnt);
  p->next = p_tmr;
  p->tcnt -= p_tmr->tcnt;
  return (p_tmr);
}

/*--------------------------- rt_tmr_kill -----------------------------------*/

OS_ID rt_tmr_kill (OS_ID timer)  {
  /* Remove user timer from the chained timer list. */
  P_TMR p, p_tmr;

  p_tmr = (P_TMR)timer;
  p = (P_TMR)&os_tmr;
  /* Search timer list for requested timer */
  while (p->next != p_tmr)  {
    if (p->next == NULL) {
      /* Failed, "timer" is not in the timer list */
      return (p_tmr);
    }
    p = p->next;
  }
  /* Timer was found, remove it from the list */
  p->next = p_tmr->next;
  p->tcnt += p_tmr->tcnt;
  rt_free_box ((U32 *)m_tmr, p_tmr);
  /* Timer killed */
  return (NULL);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
