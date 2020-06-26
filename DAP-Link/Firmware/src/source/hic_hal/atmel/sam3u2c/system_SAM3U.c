/**************************************************************************//**
 * @file     system_SAM3U.c
 * @brief    CMSIS Cortex-M3 Device Peripheral Access Layer Source File
 *           for the Atmel SAM3U Device Series
 * @version  V1.10
 * @date     16. April 2013
 *
 * @note
 * Copyright (C) 2019-2013 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#include "sam3u.h"

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/

/*--------------------- Embedded Flash Controllers Configuration -------------
//
// <e0> Embedded Flash Controller 0 (EEFC0)
//   <o1.8..11>  FWS: Flash Wait State <1-16><#-1>
//   <o1.24>     FAM: Flash Access Mode
//                 <0=> 128-bit in read mode (enhance speed)
//                 <1=> 64-bit in read mode (enhance power consumption)
// </e0>
//
// <e2> Embedded Flash Controller 1 (EEFC1)
//   <o3.8..11>  FWS: Flash Wait State <1-16><#-1>
//   <o3.24>     FAM: Flash Access Mode
//                 <0=> 128-bit in read mode (enhance speed)
//                 <1=> 64-bit in read mode (enhance power consumption)
// </e2>
*/
#define EEFC0_SETUP     1               // Reset values:
#define EEFC0_FMR_Val   0x00000300      // 0x00000000
#define EEFC1_SETUP     1               // Reset values:
#define EEFC1_FMR_Val   0x00000300      // 0x00000000


/*--------------------- Power Management Controller Configuration ------------
//
// <e> Power Management Controller (PMC)
//   <h> System Clock Enable Register (PMC_SCER)
//     <o1.8>      PCK0: Programmable Clock 0 Output Enable
//     <o1.9>      PCK1: Programmable Clock 1 Output Enable
//     <o1.10>     PCK2: Programmable Clock 2 Output Enable
//   </h>
//
//   <h> Peripheral Clock Enable Register (PMC_PCER)
//     <o2.2>      PID2:  Real Time Clock Clock Enable
//     <o2.3>      PID3:  Real Time Timer Clock Enable
//     <o2.4>      PID4:  Watchdog Timer Clock Enable
//     <o2.5>      PID5:  Power Management Controller Clock Enable
//     <o2.6>      PID6:  Enhanced Embedded Flash Controller 0 Clock Enable
//     <o2.7>      PID7:  Enhanced Embedded Flash Controller 1 Clock Enable
//     <o2.8>      PID8:  UART Clock Enable
//     <o2.9>      PID9:  Static Memory Controller Clock Enable
//     <o2.10>     PID10: Parallel I/O Controller A Clock Enable
//     <o2.11>     PID11: Parallel I/O Controller B Clock Enable
//     <o2.12>     PID12: Parallel I/O Controller C Clock Enable
//     <o2.13>     PID13: USART 0 Clock Enable
//     <o2.14>     PID14: USART 1 Clock Enable
//     <o2.15>     PID15: USART 2 Clock Enable
//     <o2.16>     PID16: USART 3 Clock Enable
//     <o2.17>     PID17: High Speed Multimedia Card Interface Clock Enable
//     <o2.18>     PID18: Two-wire Interface 0 Clock Enable
//     <o2.19>     PID19: Two-wire Interface 1 Clock Enable
//     <o2.20>     PID20: Synchronous Peripheral Interface Clock Enable
//     <o2.21>     PID21: Synchronous Serial Controller Clock Enable
//     <o2.22>     PID22: Timer Counter 0 Clock Enable
//     <o2.23>     PID23: Timer Counter 1 Clock Enable
//     <o2.24>     PID24: Timer Counter 2 Clock Enable
//     <o2.25>     PID25: Pulse Width Modulation Controller Clock Enable
//     <o2.26>     PID26: 12-bit ADC Controller Clock Enable
//     <o2.27>     PID27: 10-bit ADC Controller Clock Enable
//     <o2.28>     PID28: DMA Controller Clock Enable
//     <o2.29>     PID29: USB Device High Speed Clock Enable
//   </h>
//
//   <h> UTMI Clock Configuration Register (CKGR_UCKR)
//     <o3.16>     UPLLEN: UTMI PLL Enable
//     <o3.20..23> UPLLCOUNT: UTMI PLL Startup Time <0-15>
//   </h>
//
//   <h> Main Oscillator Register (CKGR_MOR)
//     <o4.0>      MOSCXTEN: Main Crystal Oscillator Enable
//     <o4.1>      MOSCXTBY: Main Crystal Oscillator Bypass
//     <o4.2>      WAITMODE: Wait Mode Command
//     <o4.3>      MOSCRCEN: Main On-chip RC Oscillator Enable
//     <o4.4..6>   MOSCRCF: Main On-chip RC Oscillator Frequency Selection
//                   <0=> 4MHz <1=> 8 MHz <2=> 12 MHz <3=> Reserved
//     <o4.8..15>  MOSCXTST: Main Crystal Oscillator Startup Time <0-255>
//     <o4.24>     MOSCSEL: Main Oscillator Selection
//                   <0=> Main On-chip RC Oscillator <1=> Main Crystal Oscillator
//     <o4.25>     CFDEN: Clock Failure Detector Enable
//   </h>
//
//   <h> Clock Generator Phase Locked Loop A Register (CKGR_PLLAR)
//                   <i> PLL A Freq = (Main CLOCK Freq / DIVA) * (MULA + 1)
//                   <i> Example: XTAL = 12 MHz, DIVA = 1, MULA = 14  =>  PLLA =  168 MHz
//     <o5.0..7>   DIVA: PLL Divider A <0-255>
//                   <i> 0        - Divider output is 0
//                   <i> 1        - Divider is bypassed
//                   <i> 2 .. 255 - Divider output is the Main Clock divided by DIVA
//     <o5.8..13>  PLLACOUNT: PLL A Counter <0-63>
//                   <i> Number of Slow Clocks before the LOCKA bit is set in
//                   <i> PMC_SR after CKGR_PLLAR is written
//     <o5.14..15> STMODE: Start Mode
//                   <0=> Fast Startup <1=> Reserved <2=> Normal Startup <3=> Reserved
//                   <i> Must be set to "Normal Startup" when PLL A is Off
//     <o5.16..26> MULA: PLL A Multiplier <0-2047>
//                   <i> 0         - The PLL A is deactivated
//                   <i> 1 .. 2047 - The PLL A Clock frequency is the PLL a input
//                   <i>             frequency multiplied by MULA + 1
//   </h>
//
//   <h> Master Clock Register (CKGR_MCKR)
//     <o6.0..1>   CSS: Master Clock Selection
//                   <0=> Slow Clock
//                   <1=> Main Clock
//                   <2=> PLL A Clock
//                   <3=> UPLL Clock
//     <o6.4..6>   PRES: Master Clock Prescaler
//                   <0=> Clock        <1=> Clock / 2
//                   <2=> Clock / 4    <3=> Clock / 8
//                   <4=> Clock / 16   <5=> Clock / 32
//                   <6=> Clock / 64   <7=> Clock / 6
//   </h>
//
//   <h> Programmable Clock Register 0 (PMC_PCK0)
//     <o7.0..2>   CSS: Master Clock Selection
//                   <0=> Slow Clock
//                   <1=> Main Clock
//                   <2=> PLL A Clock
//                   <3=> UPLL Clock
//                   <4=> Master Clock
//                   <5=> Master Clock
//                   <6=> Master Clock
//                   <7=> Master Clock
//     <o7.4..6>   PRES: Programmable Clock Prescaler
//                   <0=> Clock        <1=> Clock / 2
//                   <2=> Clock / 4    <3=> Clock / 8
//                   <4=> Clock / 16   <5=> Clock / 32
//                   <6=> Clock / 64   <7=> Reserved
//   </h>
//
//   <h> Programmable Clock Register 1 (PMC_PCK1)
//     <o8.0..2>   CSS: Master Clock Selection
//                   <0=> Slow Clock
//                   <1=> Main Clock
//                   <2=> PLL A Clock
//                   <3=> UPLL Clock
//                   <4=> Master Clock
//                   <5=> Master Clock
//                   <6=> Master Clock
//                   <7=> Master Clock
//     <o8.4..6>   PRES: Programmable Clock Prescaler
//                   <0=> None         <1=> Clock / 2
//                   <2=> Clock / 4    <3=> Clock / 8
//                   <4=> Clock / 16   <5=> Clock / 32
//                   <6=> Clock / 64   <7=> Reserved
//   </h>
//
//   <h> Programmable Clock Register 2 (PMC_PCK2)
//     <o9.0..2>   CSS: Master Clock Selection
//                   <0=> Slow Clock
//                   <1=> Main Clock
//                   <2=> PLL A Clock
//                   <3=> UPLL Clock
//                   <4=> Master Clock
//                   <5=> Master Clock
//                   <6=> Master Clock
//                   <7=> Master Clock
//     <o9.4..6>   PRES: Programmable Clock Prescaler
//                   <0=> None         <1=> Clock / 2
//                   <2=> Clock / 4    <3=> Clock / 8
//                   <4=> Clock / 16   <5=> Clock / 32
//                   <6=> Clock / 64   <7=> Reserved
//   </h>
// </e>
*/
#define PMC_SETUP       1               // Reset values:
#define PMC_SCER_Val    0x00000000      // WO register (0x00000001)
#define PMC_PCER_Val    0x00001C00      // WO register (0x00000000)
#define CKGR_UCKR_Val   0x10200000      // 0x10200800
#define CKGR_MOR_Val    0x01370101      // 0x00000001
#define CKGR_PLLAR_Val  0x200F8601 //0x200D8601      // 0x00003F00
#define PMC_MCKR_Val    0x00000012      // 0x00000001
#define PMC_PCK0_Val    0x00000000      // 0x00000000
#define PMC_PCK1_Val    0x00000000      // 0x00000000
#define PMC_PCK2_Val    0x00000000      // 0x00000000


/*--------------------- Watchdog Configuration -------------------------------
//
// <e> Watchdog Disable
// </e>
*/
#define WDT_SETUP       1               // Reset values:


/*
//-------- <<< end of configuration section >>> ------------------------------
*/

/*----------------------------------------------------------------------------
  Check the register settings
 *----------------------------------------------------------------------------*/
#define CHECK_RANGE(val, min, max)                ((val < min) || (val > max))
#define CHECK_RSVD(val, mask)                     (val & mask)

/* Embedded Flash Controllers Configuration ----------------------------------*/
#if (CHECK_RSVD((EEFC0_FMR_Val),  ~0x01000F01))
   #error "EEFC0_FMR: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((EEFC1_FMR_Val),  ~0x01000F01))
   #error "EEFC1_FMR: Invalid values of reserved bits!"
#endif

/* Power Management Controller Configuration ---------------------------------*/
#if (CHECK_RSVD((PMC_SCER_Val),   ~0x00000700))
   #error "PMC_SCER: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PMC_PCER_Val),   ~0xFFFFFFFC))
   #error "PMC_PCER: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((CKGR_UCKR_Val),  ~0x10F10000))
   #error "CKGR_UCKR: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((CKGR_MOR_Val),   ~0x03FFFF7F))
   #error "CKGR_MOR: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((CKGR_PLLAR_Val), ~0x27FFFFFF))
   #error "CKGR_PLLAR: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PMC_MCKR_Val),   ~0x00000073))
   #error "PMC_MCKR: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PMC_PCK0_Val),   ~0x00000077))
   #error "PMC_PCK0: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PMC_PCK1_Val),   ~0x00000077))
   #error "PMC_PCK1: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PMC_PCK2_Val),   ~0x00000077))
   #error "PMC_PCK2: Invalid values of reserved bits!"
#endif


/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define XTAL        (12000000UL)        /* Crystal frequency                  */
#define XTAL32      (   32768UL)        /* 32k crystal frequency              */
#define OSC_CLK     (      XTAL)        /* Main oscillator frequency          */
#define OSC32_CLK   (    XTAL32)        /* 32k oscillator frequency           */
#define ERC_OSC     (   32000UL)        /* Embedded RC oscillator freqquency  */
#define EFRC_OSC    ( 4000000UL)        /* Embedded fast RC oscillator freq   */



#if (PMC_SETUP)
  /* Determine clock frequency according to clock register values             */
  #if  ((PMC_MCKR_Val & 3) == 0)         /* Slow Clock is selected             */
      #if (1 /* PMC_SR & (1 << 7) */)   /* MUST be checked with correct register */
        #define __CORE_CLK_PRE     (OSC32_CLK)
      #else
        #define __CORE_CLK_PRE     (ERC_OSC)
      #endif
  #elif ((PMC_MCKR_Val & 3) == 1)        /* Main Clock is selected             */
      #if (CKGR_MOR_Val & (1 << 24))
        #if ((CKGR_MOR_Val & (1<< 0)) == 0)
          #error "CKGR_MOR: Main Crystal Oscillator selected but not enabled!"
        #endif
        #define __CORE_CLK_PRE     (OSC_CLK)
      #else
        #if ((CKGR_MOR_Val & (1<< 3)) == 0)
          #error "CKGR_MOR: Main On-Chip RC Oscillator selected but not enabled!"
        #endif
        #if   (((CKGR_MOR_Val >> 4) & 3) == 0)
          #define __CORE_CLK_PRE     (EFRC_OSC)
        #elif (((CKGR_MOR_Val >> 4) & 3) == 1)
          #define __CORE_CLK_PRE     (EFRC_OSC * 2)
        #elif (((CKGR_MOR_Val >> 4) & 3) == 2)
          #define __CORE_CLK_PRE     (EFRC_OSC * 3)
        #else
          #define __CORE_CLK_PRE     (EFRC_OSC)
        #endif
      #endif
  #elif ((PMC_MCKR_Val & 3) == 2)        /* PLLA Clock is selected             */
      #if (CKGR_MOR_Val & (1 << 24))
        #if ((CKGR_MOR_Val & (1<< 0)) == 0)
          #error "CKGR_MOR: Main Crystal Oscillator selected but not enabled!"
        #endif
        #define __PLLA_CLK       (OSC_CLK)
      #else
        #if ((CKGR_MOR_Val & (1<< 3)) == 0)
          #error "CKGR_MOR: Main On-Chip RC Oscillator selected but not enabled!"
        #endif
        #if   (((CKGR_MOR_Val >> 4) & 3) == 0)
          #define __PLLA_CLK      (EFRC_OSC)
        #elif (((CKGR_MOR_Val >> 4) & 3) == 1)
          #define __PLLA_CLK      (EFRC_OSC * 2)
        #elif (((CKGR_MOR_Val >> 4) & 3) == 2)
          #define __PLLA_CLK      (EFRC_OSC * 3)
        #else
          #define __PLLA_CLK      (EFRC_OSC)
        #endif
      #endif

      #define __PLLA_MUL       ((((CKGR_PLLAR_Val) >> 16) & 0x7FF) + 1)
      #define __PLLA_DIV       ((((CKGR_PLLAR_Val) >>  0) & 0x0FF))
      #define __CORE_CLK_PRE  (__PLLA_CLK * __PLLA_MUL / __PLLA_DIV)
  #else                                  /* UPLL Clock is selected             */
      #define __CORE_CLK_PRE  (OSC_CLK * 40)
  #endif

  #if (((PMC_MCKR_Val >> 4) & 7) == 7)
    #define __CORE_CLK   (__CORE_CLK_PRE / 6)
  #else
    #define __CORE_CLK    (__CORE_CLK_PRE >> ((PMC_MCKR_Val >> 4) & 7))
  #endif

#else
    #define __CORE_CLK   (EFRC_OSC)
#endif

#if (__CORE_CLK > 96000000UL)
  #error "Core Clock > 96MHz!"
#endif

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = __CORE_CLK;/*!< System Clock Frequency (Core Clock)*/


/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
  /* Determine clock frequency according to clock register values             */
  switch (PMC->PMC_MCKR & 3) {
    case 0:                             /* Slow clock                         */
      if (SUPC->SUPC_SR & (1 << 7))
        SystemCoreClock = OSC32_CLK;
      else
        SystemCoreClock = ERC_OSC;
      break;
    case 1:                             /* Main clock                         */
      if (PMC->CKGR_MOR & (1 << 24))
        SystemCoreClock = OSC_CLK;
      else {
        SystemCoreClock = EFRC_OSC;
        switch ((PMC->CKGR_MOR >> 4) & 3) {
          case 0:
            break;
          case 1:
            SystemCoreClock *= 2;
            break;
          case 2:
            SystemCoreClock *= 3;
            break;
          case 3:
            break;
        }
      }
      break;
    case 2:                             /* PLLA clock                         */
      if (PMC->CKGR_MOR & (1 << 24))
        SystemCoreClock = OSC_CLK;
      else {
        SystemCoreClock = EFRC_OSC;
        switch ((PMC->CKGR_MOR >> 4) & 3) {
          case 0:
            break;
          case 1:
            SystemCoreClock *= 2;
            break;
          case 2:
            SystemCoreClock *= 3;
            break;
          case 3:
            break;
        }
      }
      SystemCoreClock *= ((((PMC->CKGR_PLLAR) >> 16) & 0x7FF) + 1);
      SystemCoreClock /= ((((PMC->CKGR_PLLAR) >>  0) & 0x0FF));
      break;
    case 3:                             /* UPLL clock                         */
      SystemCoreClock = OSC_CLK * 40;
      break;
  }

  if (((PMC->PMC_MCKR >> 4) & 7) == 7)
    SystemCoreClock /= 6;
  else
    SystemCoreClock >>= ((PMC->PMC_MCKR >> 4) & 7);

}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemFrequency variable.
 */
void SystemInit (void)
{
#if (EEFC0_SETUP == 1)                  /* Embedded Flash Controller 0 Setup  */
  EFC0->EEFC_FMR = EEFC0_FMR_Val;
#endif

#if (EEFC1_SETUP == 1)                  /* Embedded Flash Controller 1 Setup  */
  #ifdef EFC1                           /* some SAM3U devices do not have EFC1*/
  EFC1->EEFC_FMR = EEFC1_FMR_Val;
  #endif
#endif

#if (PMC_SETUP == 1)                    /* Power Management Controller Setup  */

PMC->PMC_WPMR = 0x504D4300;             /* Disable write protect              */

#if (CKGR_UCKR_Val & (1 << 16))         /* If UPLL Enabled                    */
  PMC->CKGR_UCKR = CKGR_UCKR_Val;
  while (!(PMC->PMC_SR & (1 << 6)));    /* Wait for LOCKU                     */
#endif

#if (CKGR_MOR_Val & ((1<<3)|(1<<0)))    /* If MOSCRCEN or MOSCXTEN set        */
  PMC->CKGR_MOR  =  (PMC->CKGR_MOR &  (1<<24)) | /* Keep the current MOSCSEL  */
                    (CKGR_MOR_Val  & ~(1<<24)) | /* Set value except MOSCSEL  */
                    ((1<<3)|(1<<0));             /* and enable bothe OSC      */
#if (CKGR_MOR_Val & ((1 << 3)))
  while (!(PMC->PMC_SR & (1 << 17)));   /* Wait for MOSCRCS                   */
#endif
#if (CKGR_MOR_Val & ((1 << 0)))
  while (!(PMC->PMC_SR & (1 << 0)));    /* Wait for MOSCXTS                   */
#endif
  PMC->CKGR_MOR  =  CKGR_MOR_Val;       /* set the desired selection          */
  while (!(PMC->PMC_SR & (1 << 16)));   /* Wait for MOSCSELS                  */
#endif

#if (CKGR_PLLAR_Val & ((0x7FF<<16)))    /* If MULA != 0                       */
  PMC->CKGR_PLLAR = CKGR_PLLAR_Val;
  while (!(PMC->PMC_SR & (1 << 1)));    /* Wait for LOCKA                     */
#endif

  if ((PMC_MCKR_Val & 0x03) >= 2) {
    /* Write PRES field only                                                  */
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~0x70) | (PMC_MCKR_Val & 0x70);
  } else {
    /* Write CSS field only                                                   */
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~0x03) | (PMC_MCKR_Val & 0x03);
  }
  while (!(PMC->PMC_SR & (1 << 3)));    /* Wait for MCKRDY                    */
  PMC->PMC_MCKR   =  PMC_MCKR_Val;      /* Write all MCKR                     */
  while (!(PMC->PMC_SR & (1 << 3)));    /* Wait for MCKRDY                    */

#if (PMC_PCK0_Val)
  PMC->PMC_PCKR[0] = PMC_PCK0_Val;      /* Write PCK0                         */
  while (!(PMC->PMC_SR & (1 << 8)));    /* Wait for PCKRDY0                   */
#endif
#if (PMC_PCK1_Val)
  PMC->PMC_PCKR[1] = PMC_PCK1_Val;      /* Write PCK1                         */
  while (!(PMC->PMC_SR & (1 << 9)));    /* Wait for PCKRDY1                   */
#endif
#if (PMC_PCK2_Val)
  PMC->PMC_PCKR[2] = PMC_PCK2_Val;      /* Write PCK2                         */
  while (!(PMC->PMC_SR & (1 << 10)));   /* Wait for PCKRDY2                   */
#endif

  PMC->PMC_SCER  = PMC_SCER_Val;
  PMC->PMC_PCER0 = PMC_PCER_Val;

PMC->PMC_WPMR = 0x504D4301;             /* Enable write protect               */
#endif

#if (WDT_SETUP == 1)                    /* Watchdog Setup                     */
  WDT->WDT_MR = WDT_MR_WDDIS;
#endif
}
