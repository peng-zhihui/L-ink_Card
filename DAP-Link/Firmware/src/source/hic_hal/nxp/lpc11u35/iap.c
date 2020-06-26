/**
 * @file    iap.c
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2017-2017, ARM Limited, All Rights Reserved
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

#include "iap.h"
#include "cortex_m.h"
#include "LPC11Uxx.h"

iap_operation_t iap_op;
static cortex_int_state_t state;
static uint32_t lock_count;

// taken code from the Nxp App Note AN11305
/* This data must be global so it is not read from the stack */
typedef void (*IAP)(uint32_t [], uint32_t []);
static const IAP iap_entry = (IAP)0x1fff1ff1;
#define init_msdstate() *((uint32_t *)(0x10000054)) = 0x0

/* This function resets some microcontroller peripherals to reset
 * hardware configuration to ensure that the USB In-System Programming module
 * will work properly. It is normally called from reset and assumes some reset
 * configuration settings for the MCU.
 * Some of the peripheral configurations may be redundant in your specific
 * project.
 */
void iap_reinvoke(void)
{
    /* make sure USB clock is turned on before calling ISP */
    LPC_SYSCON->SYSAHBCLKCTRL |= 0x04000;
    /* make sure 32-bit Timer 1 is turned on before calling ISP */
    LPC_SYSCON->SYSAHBCLKCTRL |= 0x00400;
    /* make sure GPIO clock is turned on before calling ISP */
    LPC_SYSCON->SYSAHBCLKCTRL |= 0x00040;
    /* make sure IO configuration clock is turned on before calling ISP */
    LPC_SYSCON->SYSAHBCLKCTRL |= 0x10000;
    /* make sure AHB clock divider is 1:1 */
    LPC_SYSCON->SYSAHBCLKDIV = 1;
    /* Send Reinvoke ISP command to ISP entry point*/
    iap_op.cmd = 57;
    init_msdstate();					 /* Initialize Storage state machine */
    /* Set stack pointer to ROM value (reset default) This must be the last
     * piece of code executed before calling ISP, because most C expressions
     * and function returns will fail after the stack pointer is changed.
     * In addition ensure the CONTROL register is set to 0 so the MSP is
     * used rather than the PSP.
     */
    __set_MSP(*((volatile uint32_t *)0x00000000));
    __set_CONTROL(0);
    /* Enter ISP. We call "iap_entry" to enter ISP because the ISP entry is done
     * through the same command interface as IAP.
     */
    iap_entry(&iap_op.cmd, &iap_op.stat);
    // Not supposed to come back!
}

void iap_call(iap_operation_t *operation)
{
    iap_entry(&operation->cmd, &operation->stat);
}

void iap_lock()
{
    cortex_int_state_t local_state;
    local_state = cortex_int_get_and_disable();
    if (lock_count == 0) {
        state = local_state;
    }
    lock_count++;
}

void iap_unlock()
{
    lock_count--;
    if (lock_count == 0) {
        cortex_int_restore(state);
    }
    
}
