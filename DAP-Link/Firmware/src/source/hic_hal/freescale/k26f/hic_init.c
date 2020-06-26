/**
 * @file    hic_init.c
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
 * Copyright (c) 2016-2017 NXP
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

#include "hic_init.h"
#include "gpio.h"
#include "fsl_clock.h"
#include "usb_phy.h"
#include "util.h"

static void busy_wait(uint32_t cycles)
{
    volatile uint32_t i;
    i = cycles;

    while (i > 0) {
        i--;
    }
}

static void fll_delay(void)
{
    // ~2.5ms at 16MHz core clock
    busy_wait(10000);
}

// This IRQ handler will be invoked if VDD falls below the trip point.
void LVD_LVW_IRQHandler(void)
{
    if (PMC->LVDSC1 & PMC_LVDSC1_LVDF_MASK)
    {
        util_assert(false && "low voltage detect tripped");
        PMC->LVDSC1 |= PMC_LVDSC1_LVDACK_MASK;
    }
    if (PMC->LVDSC2 & PMC_LVDSC2_LVWF_MASK)
    {
        util_assert(false && "low voltage warning tripped");
        PMC->LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
    }
}

//! - MPU is disabled and gated.
//! - 8kB cache is enabled. SRAM is not cached, so no flushing is required for normal operation.
//! - Enable low voltage warning interrupt.
//! - Disable USB current limiter so the voltage doesn't drop as we enable high speed clocks.
void sdk_init(void)
{
    CLOCK_SetXtal0Freq(16000000U); // 16 MHz crystal
    CLOCK_SetXtal32Freq(0);

    // Disable the MPU if it's enabled.
    if (SIM->SCGC7 & SIM_SCGC7_MPU_MASK)
    {
        SYSMPU->CESR = 0;
        SIM->SCGC7 &= ~SIM_SCGC7_MPU_MASK;
    }

    // Invalidate and enable code cache.
    LMEM->PCCCR = LMEM_PCCCR_GO_MASK | LMEM_PCCCR_INVW1_MASK | LMEM_PCCCR_INVW0_MASK | LMEM_PCCCR_ENCACHE_MASK;

    // Enable LVD/LVW IRQ.
    PMC->LVDSC1 |= PMC_LVDSC1_LVDACK_MASK;
    PMC->LVDSC1 = PMC_LVDSC1_LVDIE_MASK | PMC_LVDSC1_LVDV(0); // low trip point
    PMC->LVDSC2 |= PMC_LVDSC2_LVWACK_MASK;
    PMC->LVDSC2 = PMC_LVDSC2_LVWIE_MASK | PMC_LVDSC2_LVWV(0); // low trip point
//     NVIC_EnableIRQ(LVD_LVW_IRQn);

    // Disable USB inrush current limiter.
    SIM->USBPHYCTL |= SIM_USBPHYCTL_USBDISILIM_MASK;
}

//! - Turn on 16MHz crystal oscillator.
//! - Turn on 32kHz IRC.
//! - Switch core clock to System PLL at 120 MHz, bus clock at 60 MHz, flash clock at 24 MHz.
//! - Enable the 480MHz USB PHY PLL.
//! - Ungate USBPHY and USBHS.
//! - Configure the USB PHY.
void hic_enable_usb_clocks(void)
{
    // Enable external oscillator and 32kHz IRC.
    MCG->C1 |= MCG_C1_IRCLKEN_MASK; // Select 32k IR.
    // Delay at least 100µs for 32kHz IRQ to stabilize.
    fll_delay();
    // Configure OSC for very high freq, low power mode.
    MCG->C2 = (MCG->C2 & ~(MCG_C2_RANGE_MASK | MCG_C2_HGO_MASK)) | MCG_C2_RANGE(2);
    OSC0->CR |= OSC_CR_ERCLKEN_MASK; // Enable OSC.
    MCG->C2 |= MCG_C2_EREFS_MASK; // Select OSC as ext ref.

    // Wait for the oscillator to stabilize.
    while (!(MCG->S & MCG_S_OSCINIT0_MASK))
    {
    }

    // Divide 16MHz xtal by 512 = 31.25kHz
    CLOCK_SetFbeMode(4, kMCG_Dmx32Default, kMCG_DrsMid, fll_delay);

    // Set dividers before switching to SYSPLL.
    SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0)       // System/core  /1 = 120MHz
                    | SIM_CLKDIV1_OUTDIV2(1)    // Bus          /2 = 60Mhz
                    | SIM_CLKDIV1_OUTDIV3(4)    // FlexBus      /5 = 24Mhz
                    | SIM_CLKDIV1_OUTDIV4(4);   // Flash        /5 = 24MHz

    // 120MHz SYSPLL
    mcg_pll_config_t pllConfig;
    pllConfig.enableMode = 0;
    pllConfig.prdiv = 2 - 1;
    pllConfig.vdiv = 30 - 16;
    CLOCK_SetPbeMode(kMCG_PllClkSelPll0, &pllConfig);
    CLOCK_SetPeeMode();

    // Enable USB clock source and init phy. This turns on the 480MHz PLL.
    CLOCK_EnableUsbhs0Clock(kCLOCK_UsbSrcPll0, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
    USB_EhciPhyInit(0, CPU_XTAL_CLK_HZ);

    SystemCoreClockUpdate();
}

void hic_power_target(void)
{
    // Keep powered off in bootloader mode
    // to prevent the target from effecting the state
    // of the reset line / reset button
    if (!daplink_is_bootloader()) {
        // configure pin as GPIO
        PIN_POWER_EN_PORT->PCR[PIN_POWER_EN_BIT] = PORT_PCR_MUX(1);
        // force always on logic 1
        PIN_POWER_EN_GPIO->PSOR = 1UL << PIN_POWER_EN_BIT;
        PIN_POWER_EN_GPIO->PDDR |= 1UL << PIN_POWER_EN_BIT;

        // Let the voltage rails stabilize.  This is especailly important
        // during software resets, since the target's 3.3v rail can take
        // 20-50ms to drain.  During this time the target could be driving
        // the reset pin low, causing the bootloader to think the reset
        // button is pressed.
        // Note: With optimization set to -O2 the value 5115 delays for ~1ms @ 20.9Mhz core
        busy_wait(5115 * 50);
    }
}
