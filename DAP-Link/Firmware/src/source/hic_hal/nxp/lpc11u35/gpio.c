/**
 * @file    gpio.c
 * @brief
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

#include "LPC11Uxx.h"
#include "gpio.h"
#include "compiler.h"
#include "IO_Config.h"
#include "settings.h"
#include "iap.h"

static void busy_wait(uint32_t cycles)
{
    volatile uint32_t i;
    i = cycles;

    while (i > 0) {
        i--;
    }
}

void gpio_init(void)
{
    // enable clock for GPIO port 0
    LPC_SYSCON->SYSAHBCLKCTRL |= (1UL << 6);
#if defined(TARGET_POWER_HOLD)
    // Target PowerHOLD port
    PIN_PWH_IOCON = PIN_PWH_IOCON_INIT;
    LPC_GPIO->CLR[PIN_PWH_PORT] = PIN_PWH;
    LPC_GPIO->DIR[PIN_PWH_PORT] |= PIN_PWH;
#endif
    // configure GPIO-LED as output
#if defined(CONTROLLED_POWER_LED)
    // Power led (red)
    PIN_POW_LED_IOCON = PIN_POW_LED_IOCON_INIT;
    LPC_GPIO->CLR[PIN_POW_LED_PORT] = PIN_POW_LED;
    LPC_GPIO->DIR[PIN_POW_LED_PORT] |= PIN_POW_LED;
#endif
    // DAP led (green)
    PIN_DAP_LED_IOCON = PIN_DAP_LED_IOCON_INIT;
    LPC_GPIO->SET[PIN_DAP_LED_PORT] = PIN_DAP_LED;
    LPC_GPIO->DIR[PIN_DAP_LED_PORT] |= PIN_DAP_LED;
    // MSD led (red)
    PIN_MSD_LED_IOCON = PIN_MSD_LED_IOCON_INIT;
    LPC_GPIO->SET[PIN_MSD_LED_PORT] = PIN_MSD_LED;
    LPC_GPIO->DIR[PIN_MSD_LED_PORT] |= PIN_MSD_LED;
    // Serial LED (blue)
    PIN_CDC_LED_IOCON = PIN_CDC_LED_IOCON_INIT;
    LPC_GPIO->SET[PIN_CDC_LED_PORT] = PIN_CDC_LED;
    LPC_GPIO->DIR[PIN_CDC_LED_PORT] |= PIN_CDC_LED;
    // configure Button(s) as input
    PIN_RESET_IN_IOCON = PIN_RESET_IN_IOCON_INIT;
    LPC_GPIO->DIR[PIN_RESET_IN_PORT] &= ~PIN_RESET_IN;
    PIN_RESET_IN_FWRD_IOCON = PIN_RESET_IN_FWRD_IOCON_INIT;
    LPC_GPIO->DIR[PIN_RESET_IN_FWRD_PORT] &= ~PIN_RESET_IN_FWRD;
#if !defined(PIN_nRESET_FET_DRIVE)
    // open drain logic for reset button
    PIN_nRESET_IOCON = PIN_nRESET_IOCON_INIT;
    LPC_GPIO->CLR[PIN_nRESET_PORT] = PIN_nRESET;
    LPC_GPIO->DIR[PIN_nRESET_PORT] &= ~PIN_nRESET;
#else
    // FET drive logic for reset button
    PIN_nRESET_IOCON = PIN_nRESET_IOCON_INIT;
    LPC_GPIO->CLR[PIN_nRESET_PORT] = PIN_nRESET;
    LPC_GPIO->DIR[PIN_nRESET_PORT] |= PIN_nRESET;
#endif
    /* Enable AHB clock to the FlexInt, GroupedInt domain. */
    LPC_SYSCON->SYSAHBCLKCTRL |= ((1 << 19) | (1 << 23) | (1 << 24));
    // Give the cap on the reset button time to charge
    busy_wait(10000);

    if (gpio_get_reset_btn() || config_ram_get_initial_hold_in_bl()) {
        IRQn_Type irq;
        // Disable SYSTICK timer and interrupt before calling into ISP
        SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);

        // Disable all nvic interrupts
        for (irq = (IRQn_Type)0; irq < (IRQn_Type)32; irq++) {
            NVIC_DisableIRQ(irq);
            NVIC_ClearPendingIRQ(irq);
        }

        // If switching to "bootloader" mode then setup the watchdog
        // so it will exit CRP mode after ~30 seconds
        if (config_ram_get_initial_hold_in_bl()) {
            LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 15); // Enable watchdog module
            LPC_SYSCON->PDRUNCFG &= ~(1 << 6);      // Enable watchdog clock (WDOSC)
            LPC_SYSCON->WDTOSCCTRL = (0xF << 5);    // Set max frequency - 2.3MHz
            LPC_WWDT->CLKSEL = (1 << 0);            // Select watchdog clock
            LPC_WWDT->TC = 0x00FFFFFF;              // Set time to reset to ~29s
            LPC_WWDT->MOD = (1 << 0) | (1 << 1);    // Enable watchdog and set reset
            LPC_WWDT->FEED = 0xAA;                  // Enable watchdog
            LPC_WWDT->FEED = 0x55;
        }

        iap_reinvoke();
    }
}

void gpio_set_hid_led(gpio_led_state_t state)
{
    if (state) {
        LPC_GPIO->CLR[PIN_DAP_LED_PORT] = PIN_DAP_LED;
    } else {
        LPC_GPIO->SET[PIN_DAP_LED_PORT] = PIN_DAP_LED;
    }
}

void gpio_set_cdc_led(gpio_led_state_t state)
{
    if (state) {
        LPC_GPIO->CLR[PIN_CDC_LED_PORT] = PIN_CDC_LED;
    } else {
        LPC_GPIO->SET[PIN_CDC_LED_PORT] = PIN_CDC_LED;
    }
}

void gpio_set_msc_led(gpio_led_state_t state)
{
    if (state) {
        LPC_GPIO->CLR[PIN_MSD_LED_PORT] = PIN_MSD_LED;
    } else {
        LPC_GPIO->SET[PIN_MSD_LED_PORT] = PIN_MSD_LED;
    }
}

uint8_t gpio_get_reset_btn_no_fwrd()
{
    return LPC_GPIO->PIN[PIN_RESET_IN_PORT] & PIN_RESET_IN ? 0 : 1;
}

uint8_t gpio_get_reset_btn_fwrd()
{
    return LPC_GPIO->PIN[PIN_RESET_IN_FWRD_PORT] & PIN_RESET_IN_FWRD ? 0 : 1;
}

void gpio_set_board_power(bool powerEnabled)
{
}
