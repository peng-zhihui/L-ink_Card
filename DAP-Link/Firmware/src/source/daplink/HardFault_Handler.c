/**
 * @file    main.c
 * @brief   Entry point for interface program logic
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2018, ARM Limited, All Rights Reserved
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


#include "settings.h"
#include "util.h"
#include "cortex_m.h"

register unsigned int _psp __asm("psp");
register unsigned int _msp __asm("msp");
register unsigned int _lr __asm("lr");
register unsigned int _control __asm("control");
void HardFault_Handler()
{
//hexdump logic on hardfault
    uint32_t stk_ptr;
    uint32_t * stack = (uint32_t *)_msp;
    
    if ((_lr & 0xF) == 0xD) { //process stack
        stack = (uint32_t *)_psp;
    }

    //calculate stack ptr before fault
    stk_ptr = (uint32_t)stack + 0x20;
    if ((stack[7] & 0x200) != 0) { //xpsr bit 9 align
        stk_ptr += 0x4;
    }
    if ((_lr & 0x10) == 0) { //fp
        stk_ptr += 0x48;
    }
    
    config_ram_add_hexdump(_lr);  //EXC_RETURN
    config_ram_add_hexdump(_psp);
    config_ram_add_hexdump(_msp);
    config_ram_add_hexdump(_control);
    config_ram_add_hexdump(stk_ptr); //SP
    config_ram_add_hexdump(stack[5]);  //LR
    config_ram_add_hexdump(stack[6]);  //PC
    config_ram_add_hexdump(stack[7]);  //xPSR 

#ifndef __CORTEX_M
#error __CORTEX_M not defined!!
#else

#if (__CORTEX_M > 0x00)
    config_ram_add_hexdump(SCB->HFSR);
    config_ram_add_hexdump(SCB->CFSR);
    config_ram_add_hexdump(SCB->DFSR);
    config_ram_add_hexdump(SCB->AFSR);
    config_ram_add_hexdump(SCB->MMFAR);
    config_ram_add_hexdump(SCB->BFAR);
#endif  

#endif //#ifndef __CORTEX_M

    util_assert(0);
    SystemReset();

    while (1); // Wait for reset
}
