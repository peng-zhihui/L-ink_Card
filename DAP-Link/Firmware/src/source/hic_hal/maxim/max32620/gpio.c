/* CMSIS-DAP Interface Firmware
 * Copyright (c) 2009-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "max32620.h"
#include "clkman_regs.h"
#include "gpio_regs.h"
#include "IO_Config.h"
#include "gpio.h"

/******************************************************************************/
void gpio_init(void)
{
    int i;
    uint32_t out_mode;

    // Ensure that the GPIO clock is enabled
    if (MXC_CLKMAN->sys_clk_ctrl_6_gpio == MXC_S_CLKMAN_CLK_SCALE_DISABLED) {
        MXC_CLKMAN->sys_clk_ctrl_6_gpio = MXC_S_CLKMAN_CLK_SCALE_DIV_1;
    }

    // All inputs readable
    for (i = 0; i < MXC_GPIO_NUM_PORTS; i++) {
        MXC_GPIO->in_mode[i] = 0x00000000;
    }

    // LED initial state off
    MXC_GPIO->out_val[PIN_DAP_LED_PORT] |= (1 << PIN_DAP_LED_PIN);
    MXC_GPIO->out_val[PIN_MSD_LED_PORT] |= (1 << PIN_MSD_LED_PIN);
    MXC_GPIO->out_val[PIN_CDC_LED_PORT] |= (1 << PIN_CDC_LED_PIN);

    // LED outputs
    out_mode = MXC_GPIO->out_mode[PIN_DAP_LED_PORT];
    out_mode &= ~(0xFU << (4 * PIN_DAP_LED_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_OPEN_DRAIN << (4 * PIN_DAP_LED_PIN));
    MXC_GPIO->out_mode[PIN_DAP_LED_PORT] = out_mode;

    out_mode = MXC_GPIO->out_mode[PIN_MSD_LED_PORT];
    out_mode &= ~(0xFU << (4 * PIN_MSD_LED_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_OPEN_DRAIN << (4 * PIN_MSD_LED_PIN));
    MXC_GPIO->out_mode[PIN_MSD_LED_PORT] = out_mode;

    out_mode = MXC_GPIO->out_mode[PIN_CDC_LED_PORT];
    out_mode &= ~(0xFU << (4 * PIN_CDC_LED_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_OPEN_DRAIN << (4 * PIN_CDC_LED_PIN));
    MXC_GPIO->out_mode[PIN_CDC_LED_PORT] = out_mode;

    // Button input
    out_mode = MXC_GPIO->out_mode[PIN_RESET_IN_NO_FWRD_PORT];
    out_mode &= ~(0xFU << (4 * PIN_RESET_IN_NO_FWRD_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_OPEN_DRAIN_WEAK_PULLUP << (4 * PIN_RESET_IN_NO_FWRD_PIN));
    MXC_GPIO->out_mode[PIN_RESET_IN_NO_FWRD_PORT] = out_mode;
    MXC_GPIO->out_val[PIN_RESET_IN_NO_FWRD_PORT] |= (0x1 << PIN_RESET_IN_NO_FWRD_PIN);
}

/******************************************************************************/
void gpio_set_hid_led(gpio_led_state_t state)
{
    if (state == GPIO_LED_ON) {
        MXC_CLRBIT(&MXC_GPIO->out_val[PIN_DAP_LED_PORT], PIN_DAP_LED_PIN);
    } else {
        MXC_SETBIT(&MXC_GPIO->out_val[PIN_DAP_LED_PORT], PIN_DAP_LED_PIN);
    }
}

/******************************************************************************/
void gpio_set_msc_led(gpio_led_state_t state)
{
    if (state == GPIO_LED_ON) {
        MXC_CLRBIT(&MXC_GPIO->out_val[PIN_MSD_LED_PORT], PIN_MSD_LED_PIN);
    } else {
        MXC_SETBIT(&MXC_GPIO->out_val[PIN_MSD_LED_PORT], PIN_MSD_LED_PIN);
    }
}

/******************************************************************************/
void gpio_set_cdc_led(gpio_led_state_t state)
{
    if (state == GPIO_LED_ON) {
        MXC_CLRBIT(&MXC_GPIO->out_val[PIN_CDC_LED_PORT], PIN_CDC_LED_PIN);
    } else {
        MXC_SETBIT(&MXC_GPIO->out_val[PIN_CDC_LED_PORT], PIN_CDC_LED_PIN);
    }
}

/******************************************************************************/
uint8_t gpio_get_reset_btn_no_fwrd(void)
{
    return !MXC_GETBIT(&MXC_GPIO->in_val[PIN_RESET_IN_NO_FWRD_PORT], PIN_RESET_IN_NO_FWRD_PIN);
}

/******************************************************************************/
uint8_t gpio_get_reset_btn_fwrd(void)
{
    return 0;
}

/******************************************************************************/
void gpio_set_board_power(bool powerEnabled)
{
}
