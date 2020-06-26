/**
 * @file    rt_TypeDef.h
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

/* Types */
typedef char               S8;
typedef unsigned char      U8;
typedef short              S16;
typedef unsigned short     U16;
typedef int                S32;
typedef unsigned int       U32;
typedef long long          S64;
typedef unsigned long long U64;
typedef unsigned char      BIT;
typedef unsigned int       BOOL;
typedef void               (*FUNCP)(void);

typedef U32     OS_TID;
typedef void    *OS_ID;
typedef U32     OS_RESULT;

typedef struct OS_TCB {
  /* General part: identical for all implementations.                        */
  U8     cb_type;                 /* Control Block Type                      */
  U8     state;                   /* Task state                              */
  U8     prio;                    /* Execution priority                      */
  U8     task_id;                 /* Task ID value for optimized TCB access  */
  struct OS_TCB *p_lnk;           /* Link pointer for ready/sem. wait list   */
  struct OS_TCB *p_rlnk;          /* Link pointer for sem./mbx lst backwards */
  struct OS_TCB *p_dlnk;          /* Link pointer for delay list             */
  struct OS_TCB *p_blnk;          /* Link pointer for delay list backwards   */
  U16    delta_time;              /* Time until time out                     */
  U16    interval_time;           /* Time interval for periodic waits        */
  U16    events;                  /* Event flags                             */
  U16    waits;                   /* Wait flags                              */
  void   **msg;                   /* Direct message passing when task waits  */
  U8     ret_val;                 /* Return value upon completion of a wait  */

  /* Hardware dependant part: specific for CM processor                      */
  U8     ret_upd;                 /* Updated return value                    */
  U16    priv_stack;              /* Private stack size, 0= system assigned  */
  U32    tsk_stack;               /* Current task Stack pointer (R13)        */
  U32    *stack;                  /* Pointer to Task Stack memory block      */

  /* Task entry point used for uVision debugger                              */
  FUNCP  ptask;                   /* Task entry address                      */
} *P_TCB;
#define TCB_RETVAL      32        /* 'ret_val' offset                        */
#define TCB_RETUPD      33        /* 'ret_upd' offset                        */
#define TCB_TSTACK      36        /* 'tsk_stack' offset                      */

typedef struct OS_PSFE {          /* Post Service Fifo Entry                 */
  void  *id;                      /* Object Identification                   */
  U32    arg;                     /* Object Argument                         */
} *P_PSFE;

typedef struct OS_PSQ {           /* Post Service Queue                      */
  U8     first;                   /* FIFO Head Index                         */
  U8     last;                    /* FIFO Tail Index                         */
  U8     count;                   /* Number of stored items in FIFO          */
  U8     size;                    /* FIFO Size                               */
  struct OS_PSFE q[1];            /* FIFO Content                            */
} *P_PSQ;

typedef struct OS_TSK {
  P_TCB  run;                     /* Current running task                    */
  P_TCB  new;                     /* Scheduled task to run                   */
} *P_TSK;

typedef struct OS_ROBIN {         /* Round Robin Control                     */
  P_TCB  task;                    /* Round Robin task                        */
  U16    time;                    /* Round Robin switch time                 */
  U16    tout;                    /* Round Robin timeout                     */
} *P_ROBIN;

typedef struct OS_XCB {
  U8     cb_type;                 /* Control Block Type                      */
  struct OS_TCB *p_lnk;           /* Link pointer for ready/sem. wait list   */
  struct OS_TCB *p_rlnk;          /* Link pointer for sem./mbx lst backwards */
  struct OS_TCB *p_dlnk;          /* Link pointer for delay list             */
  struct OS_TCB *p_blnk;          /* Link pointer for delay list backwards   */
  U16    delta_time;              /* Time until time out                     */
} *P_XCB;

typedef struct OS_MCB {
  U8     cb_type;                 /* Control Block Type                      */
  U8     isr_st;                  /* State flag variable for isr functions   */
  struct OS_TCB *p_lnk;           /* Chain of tasks waiting for message      */
  U16    first;                   /* Index of the message list begin         */
  U16    last;                    /* Index of the message list end           */
  U16    count;                   /* Actual number of stored messages        */
  U16    size;                    /* Maximum number of stored messages       */
  void   *msg[1];                 /* FIFO for Message pointers 1st element   */
} *P_MCB;

typedef struct OS_SCB {
  U8     cb_type;                 /* Control Block Type                      */
  U16    tokens;                  /* Semaphore tokens                        */
  struct OS_TCB *p_lnk;           /* Chain of tasks waiting for tokens       */
} *P_SCB;

typedef struct OS_MUCB {
  U8     cb_type;                 /* Control Block Type                      */
  U8     prio;                    /* Owner task default priority             */
  U16    level;                   /* Call nesting level                      */
  struct OS_TCB *p_lnk;           /* Chain of tasks waiting for mutex        */
  struct OS_TCB *owner;           /* Mutex owner task                        */
} *P_MUCB;

typedef struct OS_XTMR {
  struct OS_TMR  *next;
  U16    tcnt;
} *P_XTMR;

typedef struct OS_TMR {
  struct OS_TMR  *next;           /* Link pointer to Next timer              */
  U16    tcnt;                    /* Timer delay count                       */
  U16    info;                    /* User defined call info                  */
} *P_TMR;

typedef struct OS_BM {
  void *free;                     /* Pointer to first free memory block      */
  void *end;                      /* Pointer to memory block end             */
  U32  blk_size;                  /* Memory block size                       */
} *P_BM;

/* Definitions */
#define __TRUE          1
#define __FALSE         0
#define NULL            ((void *) 0)

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

