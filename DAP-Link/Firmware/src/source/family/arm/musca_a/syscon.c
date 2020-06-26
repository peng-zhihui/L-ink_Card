/**
 * @file    syscon.c
 * @brief   System Controller serial interface
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2008-2019, ARM Limited, All Rights Reserved
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

#include <ctype.h>                                // character functions
#include <string.h>                               // string and memory functions

#include "IO_Config_Override.h"                   // I/O pin definitions

#include "syscon.h"                               // SCC interface

// SYSCON timing
#define TSUH            2                         // Clock setup and hold
#define TCLK            2                         // Clock high time

// Misc defines
#define GPIOIMSK        0x003F                    // GPIOI SCC bit mask

/*----------------------------------------------------------------------------
  System Controller serial interface
 *----------------------------------------------------------------------------*/
static void Sleepns(uint32_t cycles)
{
    volatile uint32_t i = cycles;        // fudge factor to give approximate 1 ns

    while (i > 0) {
        i--;
    }
}
/*----------------------------------------------------------------------------
  System Controller 32bit register read (16uS)
 *----------------------------------------------------------------------------*/
void syscon_readreg(unsigned int addr, unsigned int *din)
{
    volatile unsigned int loop, data;

    // Write the 12bit address value
    for(loop = 0; loop < 12; loop++)
    {
        if (addr & 0x800)
        {
            LPC_GPIO->SET[PIN_SCC_DATAIN_PORT] = PIN_SCC_DATAIN;
        }
        else
        {
            LPC_GPIO->CLR[PIN_SCC_DATAIN_PORT] = PIN_SCC_DATAIN;
        }
        Sleepns(TCLK);
        LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK);
        LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        // Set next address bit
        addr = (addr << 1) & 0xFFF;
    }

    LPC_GPIO->CLR[PIN_SCC_DATAIN_PORT] = PIN_SCC_DATAIN;
    Sleepns(TCLK);

    // Config load
    LPC_GPIO->SET[PIN_SCC_LOAD_PORT] = PIN_SCC_LOAD;
    Sleepns(TCLK * 2);
    LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
    Sleepns(TCLK * 3);
    LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
    LPC_GPIO->CLR[PIN_SCC_LOAD_PORT] = PIN_SCC_LOAD;
    Sleepns(TCLK * 2);

    // Read the 32bit data value
    data = 0;

    for (loop = 0; loop < 4; loop++)
    {
        data = (data >> 8) & 0x00FFFFFF;
        Sleepns(TCLK);
        LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK * 2);
        LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK);
        data |= ((LPC_GPIO->PIN[PIN_SCC_DATAOUT_PORT] & PIN_SCC_DATAOUT) << (24 - PIN_SCC_DATAOUT_BIT));
        Sleepns(TCLK);
        LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK * 2);
        LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK);
        data |= ((LPC_GPIO->PIN[PIN_SCC_DATAOUT_PORT] & PIN_SCC_DATAOUT) << (25 - PIN_SCC_DATAOUT_BIT));
        Sleepns(TCLK);
        LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK * 2);
        LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK);
        data |= ((LPC_GPIO->PIN[PIN_SCC_DATAOUT_PORT] & PIN_SCC_DATAOUT) << (26 - PIN_SCC_DATAOUT_BIT));
        Sleepns(TCLK);
        LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK * 2);
        LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK);
        data |= ((LPC_GPIO->PIN[PIN_SCC_DATAOUT_PORT] & PIN_SCC_DATAOUT) << (27 - PIN_SCC_DATAOUT_BIT));
        Sleepns(TCLK);
        LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK * 2);
        LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK);
        data |= ((LPC_GPIO->PIN[PIN_SCC_DATAOUT_PORT] & PIN_SCC_DATAOUT) << (28 - PIN_SCC_DATAOUT_BIT));
        Sleepns(TCLK);
        LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK * 2);
        LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK);
        data |= ((LPC_GPIO->PIN[PIN_SCC_DATAOUT_PORT] & PIN_SCC_DATAOUT) << (29 - PIN_SCC_DATAOUT_BIT));
        Sleepns(TCLK);
        LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK * 2);
        LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK);
        data |= ((LPC_GPIO->PIN[PIN_SCC_DATAOUT_PORT] & PIN_SCC_DATAOUT) << (30 - PIN_SCC_DATAOUT_BIT));
        Sleepns(TCLK);
        LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK * 2);
        LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK);
        data |= ((LPC_GPIO->PIN[PIN_SCC_DATAOUT_PORT] & PIN_SCC_DATAOUT) << (31 - PIN_SCC_DATAOUT_BIT));
        Sleepns(TCLK);
    }

    // Return the 32bit data value
    *din = data;
}

/*----------------------------------------------------------------------------
  System Controller 32bit register write (20uS)
 *----------------------------------------------------------------------------*/
void syscon_writereg(unsigned int addr, unsigned int dout)
{
    volatile unsigned int loop, data;

    // Set write enable
    LPC_GPIO->SET[PIN_SCC_WNR_PORT] = PIN_SCC_WNR;

    // Write the 12bit address value
    for(loop = 0; loop < 12; loop++)
    {
        if (addr & 0x800)
        {
            LPC_GPIO->SET[PIN_SCC_DATAIN_PORT] = PIN_SCC_DATAIN;
        }
        else
        {
            LPC_GPIO->CLR[PIN_SCC_DATAIN_PORT] = PIN_SCC_DATAIN;
        }
        Sleepns(TCLK);
        LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK);
        LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        // Set next address bit
        addr = (addr << 1) & 0xFFF;
    }

    LPC_GPIO->CLR[PIN_SCC_DATAIN_PORT] = PIN_SCC_DATAIN;
    Sleepns(TCLK);

    // Write the 32bit data value
    data  = dout;
    for (loop = 0; loop < 32; loop++)
    {
        if (data & 0x80000000)
        {
            LPC_GPIO->SET[PIN_SCC_DATAIN_PORT] = PIN_SCC_DATAIN;
        }
        else
        {
            LPC_GPIO->CLR[PIN_SCC_DATAIN_PORT] = PIN_SCC_DATAIN;
        }
        Sleepns(TCLK);
        LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        Sleepns(TCLK);
        LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
        // Set next address bit
        data = (data << 1);
        Sleepns(TCLK);
    }

    LPC_GPIO->CLR[PIN_SCC_DATAIN_PORT] = PIN_SCC_DATAIN;
    Sleepns(TCLK);

    // Config load
    LPC_GPIO->SET[PIN_SCC_LOAD_PORT] = PIN_SCC_LOAD;
    Sleepns(TCLK * 2);
    LPC_GPIO->SET[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
    Sleepns(TCLK * 3);
    LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
    LPC_GPIO->CLR[PIN_SCC_LOAD_PORT] = PIN_SCC_LOAD;
    Sleepns(TCLK * 2);

    // Set to read
    LPC_GPIO->CLR[PIN_SCC_WNR_PORT] = PIN_SCC_WNR;
}

// end of syscon.c
