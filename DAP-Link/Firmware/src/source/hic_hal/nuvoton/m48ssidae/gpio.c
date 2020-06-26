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

#include "gpio.h"

void gpio_init(void)
{
    GPIO_SetMode(OFF_BTN_GRP, (1 << OFF_BTN_BIT), GPIO_MODE_INPUT);
}

void gpio_set_hid_led(gpio_led_state_t state)
{
}

void gpio_set_cdc_led(gpio_led_state_t state)
{
}

void gpio_set_msc_led(gpio_led_state_t state)
{
}

uint8_t gpio_get_reset_btn_no_fwrd(void)
{
    return (OFF_BTN_IO != 0) ? 0 : 1;
}

uint8_t gpio_get_reset_btn_fwrd(void)
{
    return 0;
}

void gpio_set_board_power(bool powerEnabled)
{
}
