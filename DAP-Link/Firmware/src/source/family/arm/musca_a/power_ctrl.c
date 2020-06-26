/**
 * @file    power_ctrl.c
 * @brief   power control sequence logic for Musca A
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
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

#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "syscon.h"
#include "gpio.h"
#include "utils.h"
#include "uart.h"

// Configure SCC
void configure_syscon(unsigned int pc)
{
    unsigned int din;

    // Dummy read
    syscon_readreg(0x004, &din);

    // CPU0 boot from QSPI or SRAM
    syscon_writereg(0x020, pc);

    // CPU1 boot from QSPI or SRAM
    syscon_writereg(0x028, pc);

    // Set IO drive strength and slew rate
    syscon_writereg(0x068, 0x03F00000);
    syscon_writereg(0x06C, 0x000FFFFF);
    syscon_writereg(0x078, 0xFFFFFFFF);
}

// Brown Out Detect
void enable_BOD(int enable)
{
    if (enable)
    {
        // Set BOD interrupt to 2.80-2.90V
        LPC_SYSCON->BODCTRL |= 0x1C;
        NVIC_EnableIRQ(BOD_IRQn);           /* Enable BOD interrupt */
    }
    else
    {
        LPC_SYSCON->BODCTRL = 0x00;
        NVIC_DisableIRQ(BOD_IRQn);
        NVIC_ClearPendingIRQ(BOD_IRQn);
    }
}

//Power off / shutdown sequence
void power_off_sequence()
{
    // Disable Brown Out Detection
    enable_BOD(0);

    // Apply CS_nSRST
    LPC_GPIO->CLR[PIN_nRESET_PORT] = PIN_nRESET;

    // Wait 10ms
    delay(10);

    // Apply CB_nRST
    LPC_GPIO->CLR[PIN_CB_nRST_PORT] = PIN_CB_nRST;
    // Wait 10ms
    delay(10);

    // Apply CFG_nRST
    LPC_GPIO->CLR[PIN_CFG_nRST_PORT] = PIN_CFG_nRST;

    // Wait 10ms
    delay(10);
}

//Power on sequence
void power_on_sequence()
{
    // Drive SCC signals
    LPC_GPIO->DIR[PIN_SCC_CLK_PORT]     |= PIN_SCC_CLK;
    LPC_GPIO->DIR[PIN_SCC_DATAIN_PORT]  |= PIN_SCC_DATAIN;
    LPC_GPIO->DIR[PIN_SCC_DATAOUT_PORT] &= ~PIN_SCC_DATAOUT;
    LPC_GPIO->DIR[PIN_SCC_WNR_PORT]     |= PIN_SCC_WNR;
    LPC_GPIO->DIR[PIN_SCC_LOAD_PORT]    |= PIN_SCC_LOAD;

    // Wait 10ms
    delay(10);

    // Release CFG_nRST to allow SCC config
    LPC_GPIO->SET[PIN_CFG_nRST_PORT] = PIN_CFG_nRST;

    // Wait 10ms
    delay(10);

    // Configure SCC
    configure_syscon(0x10200000);

    // Wait 10ms
    delay(10);

    // Release SCC signals
    LPC_GPIO->DIR[PIN_SCC_CLK_PORT]     &= ~PIN_SCC_CLK;
    LPC_GPIO->DIR[PIN_SCC_DATAIN_PORT]  &= ~PIN_SCC_DATAIN;
    LPC_GPIO->DIR[PIN_SCC_DATAOUT_PORT] &= ~PIN_SCC_DATAOUT;
    LPC_GPIO->DIR[PIN_SCC_WNR_PORT]     &= ~PIN_SCC_WNR;
    LPC_GPIO->DIR[PIN_SCC_LOAD_PORT]    &= ~PIN_SCC_LOAD;

    // Wait 10ms
    delay(10);

    // Release CB_nRST
    LPC_GPIO->SET[PIN_CB_nRST_PORT] = PIN_CB_nRST;

    // Wait 10ms
    delay(10);

    // Release CS_nSRST
    LPC_GPIO->SET[PIN_nRESET_PORT] = PIN_nRESET;

    // Wait 10ms
    delay(10);

    // Enable Brown Out Detection
    enable_BOD(1);
}

// BOD Interrupt Service Routine
void BOD_IRQHandler(void)
{
    NVIC_DisableIRQ(BOD_IRQn);
    gpio_set_cdc_led(GPIO_LED_OFF);  // ON GREEN

    // go into controlled shutdown
    power_off_sequence();

    while(1)
    {
        delay(100);
        gpio_set_hid_led(GPIO_LED_ON);
        delay(100);
        gpio_set_hid_led(GPIO_LED_OFF);
    }
}

// Function to wait till PBON button is pressed and released
void wait_for_pbon(void)
{
    // Standby - wait for PBON
    while (!gpio_get_pbon_btn())
    {
        // Do something with leds?
        gpio_set_cdc_led(GPIO_LED_ON);
        delay(100);
        gpio_set_cdc_led(GPIO_LED_OFF);
        delay(100);
    }
    // Wait for PBON to go low then high
    while (gpio_get_pbon_btn())
    {
        // Do something with leds?
        gpio_set_hid_led(GPIO_LED_ON);
        delay(100);
        gpio_set_hid_led(GPIO_LED_OFF);
        delay(100);
    }
    // Wait 10ms
    delay(10);

}
