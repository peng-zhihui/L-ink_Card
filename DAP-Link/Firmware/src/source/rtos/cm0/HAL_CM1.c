/**
 * @file    HAL_CM1.c
 * @brief   HAL for Cortex-M0(p)
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
#include "rt_HAL_CM.h"
#include "rt_Task.h"
#include "rt_List.h"
#include "rt_MemBox.h"


/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- rt_set_PSP ------------------------------------*/

__asm void rt_set_PSP (U32 stack) {
        MSR     PSP,R0
        BX      LR
}


/*--------------------------- rt_get_PSP ------------------------------------*/

__asm U32 rt_get_PSP (void) {
        MRS     R0,PSP
        BX      LR
}


/*--------------------------- os_set_env ------------------------------------*/

__asm void os_set_env (void) {
   /* Switch to Unprivileged/Privileged Thread mode, use PSP. */
        MOV     R0,SP                   ; PSP = MSP
        MSR     PSP,R0
        LDR     R0,=__cpp(&os_flags)
        LDRB    R0,[R0]
        LSLS    R0,#31
        BNE     PrivilegedE
        MOVS    R0,#0x03                ; Unprivileged Thread mode, use PSP
        MSR     CONTROL,R0
        BX      LR
PrivilegedE
        MOVS    R0,#0x02                ; Privileged Thread mode, use PSP
        MSR     CONTROL,R0
        BX      LR

        ALIGN
}


/*--------------------------- _alloc_box ------------------------------------*/

__asm void *_alloc_box (void *box_mem) {
   /* Function wrapper for Unprivileged/Privileged mode. */
        LDR     R3,=__cpp(rt_alloc_box)
        MOV     R12,R3
        MRS     R3,IPSR
        LSLS    R3,#24
        BNE     PrivilegedA
        MRS     R3,CONTROL
        LSLS    R3,#31
        BEQ     PrivilegedA
        SVC     0
        BX      LR
PrivilegedA
        BX      R12

        ALIGN
}


/*--------------------------- _free_box -------------------------------------*/

__asm int _free_box (void *box_mem, void *box) {
   /* Function wrapper for Unprivileged/Privileged mode. */
        LDR     R3,=__cpp(rt_free_box)
        MOV     R12,R3
        MRS     R3,IPSR
        LSLS    R3,#24
        BNE     PrivilegedF
        MRS     R3,CONTROL
        LSLS    R3,#31
        BEQ     PrivilegedF
        SVC     0
        BX      LR
PrivilegedF
        BX      R12

        ALIGN
}


/*-------------------------- SVC_Handler ------------------------------------*/

__asm void SVC_Handler (void) {
        PRESERVE8

        IMPORT  SVC_Count
        IMPORT  SVC_Table
        IMPORT  rt_stk_check

        MRS     R0,PSP                  ; Read PSP
        LDR     R1,[R0,#24]             ; Read Saved PC from Stack
        SUBS    R1,R1,#2                ; Point to SVC Instruction
        LDRB    R1,[R1]                 ; Load SVC Number
        CMP     R1,#0
        BNE     SVC_User                ; User SVC Number > 0

        MOV     LR,R4
        LDMIA   R0,{R0-R3,R4}           ; Read R0-R3,R12 from stack
        MOV     R12,R4
        MOV     R4,LR
        BLX     R12                     ; Call SVC Function

        LDR     R3,=__cpp(&os_tsk)
        LDMIA   R3!,{R1,R2}             ; os_tsk.run, os_tsk.new
        CMP     R1,R2
        BEQ     SVC_Exit                ; no task switch

        SUBS    R3,#8
        CMP     R1,#0                   ; Runtask deleted?
        BEQ     SVC_Restore

        PUSH    {R2,R3}
        ADDS    R1,#32                  ; for STRB offset < 32
        MOVS    R3,#1
        STRB    R3,[R1,#TCB_RETUPD-32]  ; os_tsk.run->ret_upd = 1

        MRS     R3,PSP                  ; Read PSP
        SUBS    R3,R3,#32               ; Adjust Start Address
        STR     R3,[R1,#TCB_TSTACK-32]  ; Update os_tsk.run->tsk_stack
        STMIA   R3!,{R4-R7}             ; Save old context (R4-R7)
        MOV     R4,R8
        MOV     R5,R9
        MOV     R6,R10
        MOV     R7,R11
        STMIA   R3!,{R4-R7}             ; Save old context (R8-R11)
        BL      rt_stk_check            ; Check for Stack overflow
        POP     {R2,R3}

SVC_Restore
        STR     R2,[R3]                 ; os_tsk.run = os_tsk.new

        ADDS    R2,#32                  ; for STRB offset < 32
        LDR     R3,[R2,#TCB_TSTACK-32]  ; os_tsk.new->tsk_stack
        ADDS    R3,R3,#16               ; Adjust Start Address
        LDMIA   R3!,{R4-R7}             ; Restore new Context (R8-R11)
        MOV     R8,R4
        MOV     R9,R5
        MOV     R10,R6
        MOV     R11,R7
        MSR     PSP,R3                  ; Write PSP
        SUBS    R3,R3,#32               ; Adjust Start Address
        LDMIA   R3!,{R4-R7}             ; Restore new Context (R4-R7)

        LDRB    R0,[R2,#TCB_RETUPD-32]  ; Update ret_val ?
        CMP     R0,#0
        BEQ     SVC_Return
        LDRB    R0,[R2,#TCB_RETVAL-32]  ; Write os_tsk.new->ret_val

SVC_Exit
        MRS     R3,PSP                  ; Read PSP
        STR     R0,[R3]                 ; Function return value

SVC_Return
        MOVS    R3,#:NOT:0xFFFFFFFD     ; Set EXC_RETURN value
        MVNS    R3,R3
        BX      R3                      ; RETI to Thread Mode, use PSP

        /*------------------- User SVC ------------------------------*/

SVC_User
        PUSH    {R4,LR}                 ; Save Registers
        LDR     R2,=SVC_Count
        LDR     R2,[R2]
        CMP     R1,R2
        BHI     SVC_Done                ; Overflow

        LDR     R4,=SVC_Table-4
        LSLS    R1,R1,#2
        LDR     R4,[R4,R1]              ; Load SVC Function Address
        MOV     LR,R4

        LDMIA   R0,{R0-R3,R4}           ; Read R0-R3,R12 from stack
        MOV     R12,R4
        BLX     LR                      ; Call SVC Function

        MRS     R4,PSP                  ; Read PSP
        STMIA   R4!,{R0-R3}             ; Function return values
SVC_Done
        POP     {R4,PC}                 ; RETI

        ALIGN
}


/*-------------------------- PendSV_Handler ---------------------------------*/

__asm void PendSV_Handler (void) {
        PRESERVE8

        BL      __cpp(rt_pop_req)

Sys_Switch
        LDR     R3,=__cpp(&os_tsk)
        LDMIA   R3!,{R1,R2}             ; os_tsk.run, os_tsk.new
        CMP     R1,R2
        BEQ     Sys_Exit                ; no task switch

        SUBS    R3,#8
        PUSH    {R2,R3}
        ADDS    R1,#32                  ; for STRB offset < 32

        MOVS    R3,#0
        STRB    R3,[R1,#TCB_RETUPD-32]  ; os_tsk.run->ret_upd = 0
        MRS     R3,PSP                  ; Read PSP
        SUBS    R3,R3,#32               ; Adjust Start Address
        STR     R3,[R1,#TCB_TSTACK-32]  ; Update os_tsk.run->tsk_stack
        STMIA   R3!,{R4-R7}             ; Save old context (R4-R7)
        MOV     R4,R8
        MOV     R5,R9
        MOV     R6,R10
        MOV     R7,R11
        STMIA   R3!,{R4-R7}             ; Save old context (R8-R11)
        BL      rt_stk_check            ; Check for Stack overflow

        POP     {R2,R3}
        STR     R2,[R3]                 ; os_tsk.run = os_tsk.new
        ADDS    R2,#32                  ; for STRB offset < 32

        LDR     R3,[R2,#TCB_TSTACK-32]  ; os_tsk.new->tsk_stack
        ADDS    R3,R3,#16               ; Adjust Start Address
        LDMIA   R3!,{R4-R7}             ; Restore new Context (R8-R11)
        MOV     R8,R4
        MOV     R9,R5
        MOV     R10,R6
        MOV     R11,R7
        MSR     PSP,R3                  ; Write PSP
        SUBS    R3,R3,#32               ; Adjust Start Address
        LDMIA   R3!,{R4-R7}             ; Restore new Context (R4-R7)

        LDRB    R0,[R2,#TCB_RETUPD-32]  ; Update ret_val ?
        CMP     R0,#0
        BEQ     Sys_Exit
        LDRB    R0,[R2,#TCB_RETVAL-32]  ; Write os_tsk.new->ret_val
        STR     R0,[R3,#16]
Sys_Exit
        MOVS    R3,#:NOT:0xFFFFFFFD     ; Set EXC_RETURN value
        MVNS    R3,R3
        BX      R3                      ; RETI to Thread Mode, use PSP

        ALIGN
}


/*-------------------------- SysTick_Handler --------------------------------*/

__asm void SysTick_Handler (void) {
        PRESERVE8

        BL      __cpp(rt_systick)
        B       Sys_Switch

        ALIGN
}


/*-------------------------- OS_Tick_Handler --------------------------------*/

__asm void OS_Tick_Handler (void) {
        PRESERVE8

        BL      __cpp(os_tick_irqack)
        BL      __cpp(rt_systick)
        B       Sys_Switch

        ALIGN
}


/*--------------------------- rt_init_stack ---------------------------------*/

void rt_init_stack (P_TCB p_TCB, FUNCP task_body) {
  /* Prepare TCB and saved context for a first time start of a task. */
  U32 *stk,i,size;

  /* Prepare a complete interrupt frame for first task start */
  size = p_TCB->priv_stack >> 2;
  if (size == 0) {
    size = (U16)os_stackinfo >> 2;
  }

  /* Write to the top of stack. */
  stk = &p_TCB->stack[size];

  /* Auto correct to 8-byte ARM stack alignment. */
  if ((U32)stk & 0x04) {
    stk--;
  }

  stk -= 16;

  /* Default xPSR and initial PC */
  stk[15] = INITIAL_xPSR;
  stk[14] = (U32)task_body;

  /* Clear R1-R12,LR registers. */
  for (i = 0; i < 14; i++) {
    stk[i] = 0;
  }

  /* Assign a void pointer to R0. */
  stk[8] = (U32)p_TCB->msg;

  /* Initial Task stack pointer. */
  p_TCB->tsk_stack = (U32)stk;

  /* Task entry point. */
  p_TCB->ptask = task_body;

  /* Set a magic word for checking of stack overflow. */
  p_TCB->stack[0] = MAGIC_WORD;
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

