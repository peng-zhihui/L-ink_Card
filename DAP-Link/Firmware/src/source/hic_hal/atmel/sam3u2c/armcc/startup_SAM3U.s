;/*****************************************************************************
; * @file:    startup_SAM3U.s
; * @purpose: CMSIS Cortex-M3 Core Device Startup File
; *           for the Atmel SAM3U Device Series
; * @version: V1.10
; * @date:    17. April 2013
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; * Copyright (C) 2009-2013 ARM Limited. All rights reserved.
; * ARM Limited (ARM) is supplying this software for use with Cortex-M3
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; *****************************************************************************/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000200

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
__Vectors       DCD     __initial_sp              ;  0: Top of Stack
                DCD     Reset_Handler             ;  1: Reset Handler
                DCD     NMI_Handler               ;  2: NMI Handler
                DCD     HardFault_Handler         ;  3: Hard Fault Handler
                DCD     MemManage_Handler         ;  4: MPU Fault Handler
                DCD     BusFault_Handler          ;  5: Bus Fault Handler
                DCD     UsageFault_Handler        ;  6: Usage Fault Handler
                DCD     0                         ;  7: Reserved
                DCD     DAPLINK_BUILD_KEY         ;  8: Build type - BL/IF
                DCD     DAPLINK_HIC_ID            ;  9: Compatibility
                DCD     DAPLINK_VERSION           ; 10:Version
                DCD     SVC_Handler               ; 11: SVCall Handler
                DCD     DebugMon_Handler          ; 12: Debug Monitor Handler
                DCD     g_board_info              ; 13: Ptr to Board info, family info other target details
                DCD     PendSV_Handler            ; 14: PendSV Handler
                DCD     SysTick_Handler           ; 15: SysTick Handler

                ; External Interrupts
                DCD     SUPC_IRQHandler           ;  0: Supply Controller
                DCD     RSTC_IRQHandler           ;  1: Reset Controller
                DCD     RTC_IRQHandler            ;  2: Real Time Clock
                DCD     RTT_IRQHandler            ;  3: Real Time Timer
                DCD     WDT_IRQHandler            ;  4: Watchdog Timer
                DCD     PMC_IRQHandler            ;  5: Power Management Controller
                DCD     EEFC0_IRQHandler          ;  6: Enhanced Embedded Flash Controller 0
                DCD     EEFC1_IRQHandler          ;  7: Enhanced Embedded Flash Controller 1
                DCD     UART_IRQHandler           ;  8: UART
                DCD     SMC_IRQHandler            ;  9: Static Memory Controller
                DCD     PIOA_IRQHandler           ; 10: Parallel I/O Controller A
                DCD     PIOB_IRQHandler           ; 11: Parallel I/O Controller B
                DCD     PIOC_IRQHandler           ; 12: Parallel I/O Controller C
                DCD     USART0_IRQHandler         ; 13: USART 0
                DCD     USART1_IRQHandler         ; 14: USART 1
                DCD     USART2_IRQHandler         ; 15: USART 2
                DCD     USART3_IRQHandler         ; 16: USART 3
                DCD     HSMCI_IRQHandler          ; 17: High Speed Multimedia Card Interface
                DCD     TWI0_IRQHandler           ; 18: Two-wire Interface 0
                DCD     TWI1_IRQHandler           ; 19: Two-wire Interface 1
                DCD     SPI_IRQHandler            ; 20: Synchronous Peripheral Interface
                DCD     SSC_IRQHandler            ; 21: Synchronous Serial Controller
                DCD     TC0_IRQHandler            ; 22: Timer Counter 0
                DCD     TC1_IRQHandler            ; 23: Timer Counter 1
                DCD     TC2_IRQHandler            ; 24: Timer Counter 2
                DCD     PWM_IRQHandler            ; 25: Pulse Width Modulation Controller
                DCD     ADC12B_IRQHandler         ; 26: 12-bit ADC Controller
                DCD     ADC_IRQHandler            ; 27: 10-bit ADC Controller
                DCD     DMAC_IRQHandler           ; 28: DMA Controller
                DCD     UDPHS_IRQHandler          ; 29: USB Device High Speed


                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
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

Default_Handler PROC

                EXPORT  SUPC_IRQHandler           [WEAK]
                EXPORT  RSTC_IRQHandler           [WEAK]
                EXPORT  RTC_IRQHandler            [WEAK]
                EXPORT  RTT_IRQHandler            [WEAK]
                EXPORT  WDT_IRQHandler            [WEAK]
                EXPORT  PMC_IRQHandler            [WEAK]
                EXPORT  EEFC0_IRQHandler          [WEAK]
                EXPORT  EEFC1_IRQHandler          [WEAK]
                EXPORT  UART_IRQHandler           [WEAK]
                EXPORT  SMC_IRQHandler            [WEAK]
                EXPORT  PIOA_IRQHandler           [WEAK]
                EXPORT  PIOB_IRQHandler           [WEAK]
                EXPORT  PIOC_IRQHandler           [WEAK]
                EXPORT  USART0_IRQHandler         [WEAK]
                EXPORT  USART1_IRQHandler         [WEAK]
                EXPORT  USART2_IRQHandler         [WEAK]
                EXPORT  USART3_IRQHandler         [WEAK]
                EXPORT  HSMCI_IRQHandler          [WEAK]
                EXPORT  TWI0_IRQHandler           [WEAK]
                EXPORT  TWI1_IRQHandler           [WEAK]
                EXPORT  SPI_IRQHandler            [WEAK]
                EXPORT  SSC_IRQHandler            [WEAK]
                EXPORT  TC0_IRQHandler            [WEAK]
                EXPORT  TC1_IRQHandler            [WEAK]
                EXPORT  TC2_IRQHandler            [WEAK]
                EXPORT  PWM_IRQHandler            [WEAK]
                EXPORT  ADC12B_IRQHandler         [WEAK]
                EXPORT  ADC_IRQHandler            [WEAK]
                EXPORT  DMAC_IRQHandler           [WEAK]
                EXPORT  UDPHS_IRQHandler          [WEAK]

SUPC_IRQHandler
RSTC_IRQHandler
RTC_IRQHandler
RTT_IRQHandler
WDT_IRQHandler
PMC_IRQHandler
EEFC0_IRQHandler
EEFC1_IRQHandler
UART_IRQHandler
SMC_IRQHandler
PIOA_IRQHandler
PIOB_IRQHandler
PIOC_IRQHandler
USART0_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
USART3_IRQHandler
HSMCI_IRQHandler
TWI0_IRQHandler
TWI1_IRQHandler
SPI_IRQHandler
SSC_IRQHandler
TC0_IRQHandler
TC1_IRQHandler
TC2_IRQHandler
PWM_IRQHandler
ADC12B_IRQHandler
ADC_IRQHandler
DMAC_IRQHandler
UDPHS_IRQHandler
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
