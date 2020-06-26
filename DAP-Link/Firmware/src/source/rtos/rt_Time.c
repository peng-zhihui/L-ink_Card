/**
 * @file    rt_Time.c
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
#include "rt_Time.h"

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

/* Free running system tick counter */
U32 os_time;


/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- rt_time_get -----------------------------------*/

U32 rt_time_get (void) {
  /* Get system time tick */
  return (os_time);
}


/*--------------------------- rt_dly_wait -----------------------------------*/

void rt_dly_wait (U16 delay_time) {
  /* Delay task by "delay_time" */
  rt_block (delay_time, WAIT_DLY);
}


/*--------------------------- rt_itv_set ------------------------------------*/

void rt_itv_set (U16 interval_time) {
  /* Set interval length and define start of first interval */
  os_tsk.run->interval_time = interval_time;
  os_tsk.run->delta_time = interval_time + (U16)os_time;
}


/*--------------------------- rt_itv_wait -----------------------------------*/

void rt_itv_wait (void) {
  /* Wait for interval end and define start of next one */
  U16 delta;

  delta = os_tsk.run->delta_time - (U16)os_time;
  os_tsk.run->delta_time += os_tsk.run->interval_time;
  if ((delta & 0x8000) == 0) {
    rt_block (delta, WAIT_ITV);
  }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

