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

#include "LPC43xx.h"
#include "lpc43xx_scu.h"
#include "gpio.h"
#include "compiler.h"
#include "DAP_config.h" // For the nRESET and RESET_TXE port/pin info
#include "IO_Config.h"

BOOL gpio_reset_pin_is_input  = __TRUE;

// Connected LED            P1_1: GPIO0[8]
#define LED_CONNECTED_PORT  0
#define LED_CONNECTED_BIT   8

// LPC43xx peripheral register bit masks (used by macros)
#define CCU_CLK_CFG_RUN     (1UL << 0)
#define CCU_CLK_CFG_AUTO    (1UL << 1)
#define CCU_CLK_STAT_RUN    (1UL << 0)

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
    /* Enable clock and init GPIO outputs */
    LPC_CCU1->CLK_M4_GPIO_CFG = CCU_CLK_CFG_AUTO | CCU_CLK_CFG_RUN;

    while (!(LPC_CCU1->CLK_M4_GPIO_STAT & CCU_CLK_STAT_RUN));

    /* Configure I/O pins: function number, input buffer enabled,  */
    /*                     no pull-up/down                         */
    scu_pinmux(1, 1, GPIO_NOPULL, FUNC0);   /* LED:      GPIO0[8]  */
    scu_pinmux(2, 11, GPIO_NOPULL, FUNC0);  /* ISPCTRL:  GPIO1[11]  */
    scu_pinmux(2, 5, GPIO_PUP,    FUNC4);   /* nRESET:    GPIO5[5] */
    scu_pinmux(2, 6, GPIO_NOPULL, FUNC4);   /* nRESET_OE: GPIO5[6] */
    /* Configure: LED as output (turned off) */
    LPC_GPIO_PORT->CLR[LED_CONNECTED_PORT]  = (1 << LED_CONNECTED_BIT);
    LPC_GPIO_PORT->DIR[LED_CONNECTED_PORT] |= (1 << LED_CONNECTED_BIT);
    /* Configure: ISPCTRL as output and high */
    LPC_GPIO_PORT->SET[ISPCTRL_PORT]  = (1 << ISPCTRL_BIT);
    LPC_GPIO_PORT->DIR[ISPCTRL_PORT] |= (1 << ISPCTRL_BIT);
    /* configure Reset Button as input, Reset Output Enable as output LOW */
    LPC_GPIO_PORT->DIR[PORT_nRESET]    &= ~(1 << PIN_nRESET_IN_BIT);
    LPC_GPIO_PORT->CLR[PORT_RESET_TXE]  = (1 << PIN_RESET_TXE_IN_BIT);
    LPC_GPIO_PORT->DIR[PORT_RESET_TXE] |= (1 << PIN_RESET_TXE_IN_BIT);
    /* Use Pin Interrupt 0 */
    LPC_SCU->PINTSEL0 &= ~0xff;
    LPC_SCU->PINTSEL0 |= (PORT_nRESET << 5) | (PIN_nRESET_IN_BIT);

    busy_wait(10000);
}

void gpio_set_hid_led(gpio_led_state_t state)
{
    if (state) {
        LPC_GPIO_PORT->SET[LED_CONNECTED_PORT] = (1 << LED_CONNECTED_BIT);
    } else {
        LPC_GPIO_PORT->CLR[LED_CONNECTED_PORT] = (1 << LED_CONNECTED_BIT);
    }
}

void gpio_set_cdc_led(gpio_led_state_t state)
{
    if (state) {
        LPC_GPIO_PORT->SET[LED_CONNECTED_PORT] = (1 << LED_CONNECTED_BIT);
    } else {
        LPC_GPIO_PORT->CLR[LED_CONNECTED_PORT] = (1 << LED_CONNECTED_BIT);
    }
}

void gpio_set_msc_led(gpio_led_state_t state)
{
    if (state) {
        LPC_GPIO_PORT->SET[LED_CONNECTED_PORT] = (1 << LED_CONNECTED_BIT);
    } else {
        LPC_GPIO_PORT->CLR[LED_CONNECTED_PORT] = (1 << LED_CONNECTED_BIT);
    }
}

void gpio_set_isp_pin(uint8_t state)
{
    if (state) {
        LPC_GPIO_PORT->SET[ISPCTRL_PORT] = (1 << ISPCTRL_BIT);
    } else {
        LPC_GPIO_PORT->CLR[ISPCTRL_PORT] = (1 << ISPCTRL_BIT);
    }
}

uint8_t gpio_get_reset_btn_no_fwrd(void)
{
    return LPC_GPIO_PORT->W[PORT_nRESET * 32 + PIN_nRESET_IN_BIT] ? 0 : 1;
}

uint8_t gpio_get_reset_btn_fwrd(void)
{
    return 0;
}

void gpio_set_board_power(bool powerEnabled)
{
}
