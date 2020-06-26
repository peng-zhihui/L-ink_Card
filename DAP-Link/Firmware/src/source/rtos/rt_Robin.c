/**
 * @file    rt_Robin.c
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
#include "rt_Time.h"
#include "rt_Robin.h"

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

struct OS_ROBIN os_robin;


/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

/*--------------------------- rt_init_robin ---------------------------------*/

__weak void rt_init_robin (void) {
  /* Initialize Round Robin variables. */
  os_robin.task = NULL;
  os_robin.tout = (U16)os_rrobin;
}

/*--------------------------- rt_chk_robin ----------------------------------*/

__weak void rt_chk_robin (void) {
  /* Check if Round Robin timeout expired and switch to the next ready task.*/
  P_TCB p_new;

  if (os_robin.task != os_rdy.p_lnk) {
    /* New task was suspended, reset Round Robin timeout. */
    os_robin.task = os_rdy.p_lnk;
    os_robin.time = (U16)os_time + os_robin.tout - 1;
  }
  if (os_robin.time == (U16)os_time) {
    /* Round Robin timeout has expired, swap Robin tasks. */
    os_robin.task = NULL;
    p_new = rt_get_first (&os_rdy);
    rt_put_prio ((P_XCB)&os_rdy, p_new);
  }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

