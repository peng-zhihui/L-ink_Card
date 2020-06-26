;/**
; * @file    startup_LPC11Uxx.s
; * @brief   
; *
; * DAPLink Interface Firmware
; * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the "License"); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * http://www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000100

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                IMPORT  g_board_info
__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     DAPLINK_BUILD_KEY         ; Build type - BL/IF
                DCD     DAPLINK_HIC_ID            ; Compatibility
                DCD     DAPLINK_VERSION           ; Version
                DCD     SVC_Handler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     g_board_info              ; Ptr to Board info, family info other target details
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     FLEX_INT0_IRQHandler      ; All GPIO pin can be routed to FLEX_INTx
                DCD     FLEX_INT1_IRQHandler
                DCD     FLEX_INT2_IRQHandler
                DCD     FLEX_INT3_IRQHandler
                DCD     FLEX_INT4_IRQHandler
                DCD     FLEX_INT5_IRQHandler
                DCD     FLEX_INT6_IRQHandler
                DCD     FLEX_INT7_IRQHandler
                DCD     GINT0_IRQHandler
                DCD     GINT1_IRQHandler          ; PIO0 (0:7)
                DCD     Reserved_IRQHandler		  ; Reserved
                DCD     Reserved_IRQHandler
                DCD     Reserved_IRQHandler
                DCD     Reserved_IRQHandler
                DCD     SSP1_IRQHandler           ; SSP1
                DCD     I2C_IRQHandler            ; I2C
                DCD     TIMER16_0_IRQHandler      ; 16-bit Timer0
                DCD     TIMER16_1_IRQHandler      ; 16-bit Timer1
                DCD     TIMER32_0_IRQHandler      ; 32-bit Timer0
                DCD     TIMER32_1_IRQHandler      ; 32-bit Timer1
                DCD     SSP0_IRQHandler           ; SSP0
                DCD     UART_IRQHandler           ; UART
                DCD     USB_IRQHandler            ; USB IRQ
                DCD     USB_FIQHandler            ; USB FIQ
                DCD     ADC_IRQHandler            ; A/D Converter
                DCD     WDT_IRQHandler            ; Watchdog timer
                DCD     BOD_IRQHandler            ; Brown Out Detect
                DCD     FMC_IRQHandler            ; IP2111 Flash Memory Controller
                DCD     Reserved_IRQHandler	       ; Reserved
                DCD     Reserved_IRQHandler       ; Reserved
                DCD     USBWakeup_IRQHandler      ; USB wake up
                DCD     Reserved_IRQHandler       ; Reserved
				
				
                IF      :DEF:MBED_BOOTLOADER
	;; 48 vector entries. We pad to 128 to fill the 0x0 - 0x1FF REMAP address space
                
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill

        	DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill

        	DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill

        	DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill

        	DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill

        	DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill

        	DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill

        	DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
                DCD     0xFFFFFFFF                ; Datafill
				
                ENDIF

                IF      :LNOT::DEF:NO_CRP
                AREA    |.ARM.__at_0x02FC|, CODE, READONLY
CRP_Key         DCD     0xFFFFFFFF
                ENDIF


                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
				
				; Power on RAM1 and USBRAM area
				LDR     R0, =0x40048080 ; System clock control
				LDR     R1, =0x0C00485F ; boot default + RAM1, USBRAM
				STR     R1, [R0]
				
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

; now, under COMMON NMI.c and NMI.h, a real NMI handler is created if NMI is enabled 
; for particular peripheral.
;NMI_Handler     PROC
;                EXPORT  NMI_Handler               [WEAK]
;                B       .
;                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP
Reserved_IRQHandler PROC
                EXPORT  Reserved_IRQHandler       [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT  NMI_Handler               [WEAK]
                EXPORT  FLEX_INT0_IRQHandler      [WEAK]
                EXPORT  FLEX_INT1_IRQHandler      [WEAK]
                EXPORT  FLEX_INT2_IRQHandler      [WEAK]
                EXPORT  FLEX_INT3_IRQHandler      [WEAK]
                EXPORT  FLEX_INT4_IRQHandler      [WEAK]
                EXPORT  FLEX_INT5_IRQHandler      [WEAK]
                EXPORT  FLEX_INT6_IRQHandler      [WEAK]
                EXPORT  FLEX_INT7_IRQHandler      [WEAK]
                EXPORT  GINT0_IRQHandler          [WEAK]
                EXPORT  GINT1_IRQHandler          [WEAK]
                EXPORT  SSP1_IRQHandler           [WEAK]
                EXPORT  I2C_IRQHandler            [WEAK]
                EXPORT  TIMER16_0_IRQHandler      [WEAK]
                EXPORT  TIMER16_1_IRQHandler      [WEAK]
                EXPORT  TIMER32_0_IRQHandler      [WEAK]
                EXPORT  TIMER32_1_IRQHandler      [WEAK]
                EXPORT  SSP0_IRQHandler           [WEAK]
                EXPORT  UART_IRQHandler           [WEAK]

                EXPORT  USB_IRQHandler            [WEAK]
                EXPORT  USB_FIQHandler            [WEAK]
                EXPORT  ADC_IRQHandler            [WEAK]
                EXPORT  WDT_IRQHandler            [WEAK]
                EXPORT  BOD_IRQHandler            [WEAK]
                EXPORT  FMC_IRQHandler            [WEAK]
                EXPORT	USBWakeup_IRQHandler      [WEAK]

NMI_Handler
FLEX_INT0_IRQHandler
FLEX_INT1_IRQHandler
FLEX_INT2_IRQHandler
FLEX_INT3_IRQHandler
FLEX_INT4_IRQHandler
FLEX_INT5_IRQHandler
FLEX_INT6_IRQHandler
FLEX_INT7_IRQHandler
GINT0_IRQHandler
GINT1_IRQHandler
SSP1_IRQHandler
I2C_IRQHandler
TIMER16_0_IRQHandler
TIMER16_1_IRQHandler
TIMER32_0_IRQHandler
TIMER32_1_IRQHandler
SSP0_IRQHandler
UART_IRQHandler
USB_IRQHandler
USB_FIQHandler
ADC_IRQHandler
WDT_IRQHandler
BOD_IRQHandler
FMC_IRQHandler
USBWakeup_IRQHandler

                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END
