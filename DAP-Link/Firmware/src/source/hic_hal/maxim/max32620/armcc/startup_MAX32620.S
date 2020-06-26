;*******************************************************************************
; Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
;
; Permission is hereby granted, free of charge, to any person obtaining a
; copy of this software and associated documentation files (the "Software"),
; to deal in the Software without restriction, including without limitation
; the rights to use, copy, modify, merge, publish, distribute, sublicense,
; and/or sell copies of the Software, and to permit persons to whom the
; Software is furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included
; in all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
; OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
; MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
; IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
; OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
; ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
; OTHER DEALINGS IN THE SOFTWARE.
;
; Except as contained in this notice, the name of Maxim Integrated
; Products, Inc. shall not be used except as stated in the Maxim Integrated
; Products, Inc. Branding Policy.
;
; The mere transfer of this software does not imply any licenses
; of trade secrets, proprietary technology, copyrights, patents,
; trademarks, maskwork rights, or any other form of intellectual
; property whatsoever. Maxim Integrated Products, Inc. retains all
; ownership rights.
;*******************************************************************************

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __initial_sp
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
				IMPORT  g_board_info
__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     DAPLINK_BUILD_KEY         ; Reserved
                DCD     DAPLINK_HIC_ID            ; Reserved
                DCD     DAPLINK_VERSION           ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     g_board_info              ; Ptr to Board info, family info other target details
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; Maxim 32620 Externals interrupts
                DCD CLKMAN_IRQHandler         /* 16:01 CLKMAN */
                DCD PWRMAN_IRQHandler         /* 17:02 PWRMAN */
                DCD FLC_IRQHandler            /* 18:03 Flash Controller */
                DCD RTC0_IRQHandler           /* 19:04 RTC INT0 */
                DCD RTC1_IRQHandler           /* 20:05 RTC INT1 */
                DCD RTC2_IRQHandler           /* 21:06 RTC INT2 */
                DCD RTC3_IRQHandler           /* 22:07 RTC INT3 */
                DCD PMU_IRQHandler            /* 23:08 PMU */
                DCD USB_IRQHandler            /* 24:09 USB */
                DCD AES_IRQHandler            /* 25:10 AES */
                DCD MAA_IRQHandler            /* 26:11 MAA */
                DCD WDT0_IRQHandler           /* 27:12 WATCHDOG0 */
                DCD WDT0_P_IRQHandler         /* 28:13 WATCHDOG0 PRE-WINDOW */
                DCD WDT1_IRQHandler           /* 29:14 WATCHDOG1 */
                DCD WDT1_P_IRQHandler         /* 30:15 WATCHDOG1 PRE-WINDOW */
                DCD GPIO_P0_IRQHandler        /* 31:16 GPIO Port 0  */
                DCD GPIO_P1_IRQHandler        /* 32:17 GPIO Port 1  */
                DCD GPIO_P2_IRQHandler        /* 33:18 GPIO Port 2  */
                DCD GPIO_P3_IRQHandler        /* 34:19 GPIO Port 3  */
                DCD GPIO_P4_IRQHandler        /* 35:20 GPIO Port 4  */
                DCD GPIO_P5_IRQHandler        /* 36:21 GPIO Port 5  */
                DCD GPIO_P6_IRQHandler        /* 37:22 GPIO Port 6  */
                DCD TMR0_IRQHandler           /* 38:23 Timer32-0 */
                DCD TMR16_0_IRQHandler        /* 39:24 Timer16-s0 */
                DCD TMR1_IRQHandler           /* 40:25 Timer32-1 */
                DCD TMR16_1_IRQHandler        /* 41:26 Timer16-s1 */
                DCD TMR2_IRQHandler           /* 42:27 Timer32-2 */
                DCD TMR16_2_IRQHandler        /* 43:28 Timer16-s2 */
                DCD TMR3_IRQHandler           /* 44:29 Timer32-3 */
                DCD TMR16_3_IRQHandler        /* 45:30 Timer16-s3 */
                DCD TMR4_IRQHandler           /* 46:31 Timer32-4 */
                DCD TMR16_4_IRQHandler        /* 47:32 Timer16-s4 */
                DCD TMR5_IRQHandler           /* 48:33 Timer32-5 */
                DCD TMR16_5_IRQHandler        /* 49:34 Timer16-s5 */
                DCD UART0_IRQHandler          /* 50:35 UART0 */
                DCD UART1_IRQHandler          /* 51:36 UART1 */
                DCD UART2_IRQHandler          /* 52:37 UART0 */
                DCD UART3_IRQHandler          /* 53:38 UART1 */
                DCD PT_IRQHandler             /* 54:39 PT */
                DCD I2CM0_IRQHandler          /* 55:40 I2C Master 0 */
                DCD I2CM1_IRQHandler          /* 56:41 I2C Master 1 */
                DCD I2CM2_IRQHandler          /* 57:42 I2C Master 2 */
                DCD I2CS_IRQHandler           /* 58:43 I2C Slave */
                DCD SPI0_IRQHandler           /* 59:44 SPI0 */
                DCD SPI1_IRQHandler           /* 60:45 SPI1 */
                DCD SPI2_IRQHandler           /* 61:46 SPI2 */
                DCD SPIB_IRQHandler           /* 62:47 SPI Bridge */
                DCD OWM_IRQHandler            /* 63:48 1-Wire Master */
                DCD AFE_IRQHandler            /* 64:49 AFE */

__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

Reset_Handler   PROC
                EXPORT Reset_Handler                    [WEAK]
                IMPORT PreInit
                IMPORT SystemInit
                IMPORT __main
                LDR     R0, =PreInit
                BLX     R0
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler             PROC
                        EXPORT NMI_Handler                     [WEAK]
                        B       .
                        ENDP

HardFault_Handler       PROC
                        EXPORT HardFault_Handler              [WEAK]
                        B       .
                        ENDP

SVC_Handler             PROC
                        EXPORT SVC_Handler                    [WEAK]
                        B       .
                        ENDP

DebugMon_Handler        PROC
                        EXPORT DebugMon_Handler               [WEAK]
                        B       .
                        ENDP

SysTick_Handler         PROC
                        EXPORT SysTick_Handler                [WEAK]
                        B       .
                        ENDP

PendSV_Handler          PROC
                        EXPORT PendSV_Handler                 [WEAK]
                        B       .
                        ENDP

MemManage_Handler       PROC
                        EXPORT MemManage_Handler              [WEAK]
                        B       .
                        ENDP

BusFault_Handler        PROC
                        EXPORT BusFault_Handler               [WEAK]
                        B       .
                        ENDP

UsageFault_Handler      PROC
                        EXPORT UsageFault_Handler             [WEAK]
                        B       .
                        ENDP
DefaultIRQ_Handler PROC
        EXPORT CLKMAN_IRQHandler                        [WEAK]
        EXPORT PWRMAN_IRQHandler                        [WEAK]
        EXPORT FLC_IRQHandler                           [WEAK]
        EXPORT RTC0_IRQHandler                          [WEAK]
        EXPORT RTC1_IRQHandler                          [WEAK]
        EXPORT RTC2_IRQHandler                          [WEAK]
        EXPORT RTC3_IRQHandler                          [WEAK]
        EXPORT PMU_IRQHandler                           [WEAK]
        EXPORT USB_IRQHandler                           [WEAK]
        EXPORT AES_IRQHandler                           [WEAK]
        EXPORT MAA_IRQHandler                           [WEAK]
        EXPORT WDT0_IRQHandler                          [WEAK]
        EXPORT WDT0_P_IRQHandler                        [WEAK]
        EXPORT WDT1_IRQHandler                          [WEAK]
        EXPORT WDT1_P_IRQHandler                        [WEAK]
        EXPORT GPIO_P0_IRQHandler                       [WEAK]
        EXPORT GPIO_P1_IRQHandler                       [WEAK]
        EXPORT GPIO_P2_IRQHandler                       [WEAK]
        EXPORT GPIO_P3_IRQHandler                       [WEAK]
        EXPORT GPIO_P4_IRQHandler                       [WEAK]
        EXPORT GPIO_P5_IRQHandler                       [WEAK]
        EXPORT GPIO_P6_IRQHandler                       [WEAK]
        EXPORT TMR0_IRQHandler                          [WEAK]
        EXPORT TMR16_0_IRQHandler                       [WEAK]
        EXPORT TMR1_IRQHandler                          [WEAK]
        EXPORT TMR16_1_IRQHandler                       [WEAK]
        EXPORT TMR2_IRQHandler                          [WEAK]
        EXPORT TMR16_2_IRQHandler                       [WEAK]
        EXPORT TMR3_IRQHandler                          [WEAK]
        EXPORT TMR16_3_IRQHandler                       [WEAK]
        EXPORT TMR4_IRQHandler                          [WEAK]
        EXPORT TMR16_4_IRQHandler                       [WEAK]
        EXPORT TMR5_IRQHandler                          [WEAK]
        EXPORT TMR16_5_IRQHandler                       [WEAK]
        EXPORT UART0_IRQHandler                         [WEAK]
        EXPORT UART1_IRQHandler                         [WEAK]
        EXPORT UART2_IRQHandler                         [WEAK]
        EXPORT UART3_IRQHandler                         [WEAK]
        EXPORT PT_IRQHandler                            [WEAK]
        EXPORT I2CM0_IRQHandler                         [WEAK]
        EXPORT I2CM1_IRQHandler                         [WEAK]
        EXPORT I2CM2_IRQHandler                         [WEAK]
        EXPORT I2CS_IRQHandler                          [WEAK]
        EXPORT SPI0_IRQHandler                          [WEAK]
        EXPORT SPI1_IRQHandler                          [WEAK]
        EXPORT SPI2_IRQHandler                          [WEAK]
        EXPORT SPIB_IRQHandler                          [WEAK]
        EXPORT OWM_IRQHandler                           [WEAK]
        EXPORT AFE_IRQHandler                           [WEAK]

CLKMAN_IRQHandler
PWRMAN_IRQHandler
FLC_IRQHandler
RTC0_IRQHandler
RTC1_IRQHandler
RTC2_IRQHandler
RTC3_IRQHandler
PMU_IRQHandler
USB_IRQHandler
AES_IRQHandler
MAA_IRQHandler
WDT0_IRQHandler
WDT0_P_IRQHandler
WDT1_IRQHandler
WDT1_P_IRQHandler
GPIO_P0_IRQHandler
GPIO_P1_IRQHandler
GPIO_P2_IRQHandler
GPIO_P3_IRQHandler
GPIO_P4_IRQHandler
GPIO_P5_IRQHandler
GPIO_P6_IRQHandler
TMR0_IRQHandler
TMR16_0_IRQHandler
TMR1_IRQHandler
TMR16_1_IRQHandler
TMR2_IRQHandler
TMR16_2_IRQHandler
TMR3_IRQHandler
TMR16_3_IRQHandler
TMR4_IRQHandler
TMR16_4_IRQHandler
TMR5_IRQHandler
TMR16_5_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
UART3_IRQHandler
PT_IRQHandler
I2CM0_IRQHandler
I2CM1_IRQHandler
I2CM2_IRQHandler
I2CS_IRQHandler
SPI0_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
SPIB_IRQHandler
OWM_IRQHandler
AFE_IRQHandler

        B .
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
