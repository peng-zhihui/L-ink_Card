/**********************************************************************
* $Id$        system_lpc43xx.c            2012-05-21
*//**
* @file        system_lpc43xx.c
* @brief    Cortex-M3 Device System Source File for NXP lpc43xx Series.
* @version    1.0
* @date        21. May. 2011
* @author    NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/

#include "LPC43xx.h"
#include "fpu_enable.h"

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __IRC            (12000000UL)    /* IRC Oscillator frequency          */

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = 96000000;    /* System Clock Frequency (Core Clock)*/

extern uint32_t __Vectors;

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit(void)
{
    // Set up Cortex_M3 or M4 VTOR register to point to vector table
    SCB->VTOR = (unsigned int)&__Vectors;

    fpuEnable();

    // In case we are running from internal flash, we configure the flash
    // accelerator.
#define FLASH_ACCELERATOR_SPEED 6
    {
        uint32_t *MAM, t;
        // Set up flash controller for both banks
        // Bank A
        MAM = (uint32_t *)(LPC_CREG_BASE + 0x120);
        t = *MAM;
        t &= ~(0xF << 12);
        *MAM = t | (FLASH_ACCELERATOR_SPEED << 12);
        // Bank B
        MAM = (uint32_t *)(LPC_CREG_BASE + 0x124);
        t = *MAM;
        t &= ~(0xF << 12);
        *MAM = t | (FLASH_ACCELERATOR_SPEED << 12);
    }
}

void SystemReset(void)
{
    /* Ensure all outstanding memory accesses included buffered write are completed before reset */
    __DSB();

    LPC_WWDT->MOD |= (1 << 1);
    LPC_WWDT->MOD |= (1 << 0);
    LPC_WWDT->FEED = 0xAA;
    LPC_WWDT->FEED = 0x55;

    /* Ensure completion of memory access */
    __DSB();

    /* wait until reset */
    while(1);
}
