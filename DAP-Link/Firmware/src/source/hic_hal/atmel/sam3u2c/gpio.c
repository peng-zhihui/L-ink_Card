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

#include "sam3u.h"
#include "gpio.h"
#include "uart.h"
#include "compiler.h"
#include "daplink.h"

// This GPIO configuration is only valid for the SAM3U2C
COMPILER_ASSERT(DAPLINK_HIC_ID == DAPLINK_HIC_ID_SAM3U2C);

void gpio_init(void)
{
    volatile int Cnt;
    //
    // Initially enable clock for GPIO and initialize LED ports as output with LED == off
    //
    PMC->PMC_PCER0 = (1 << 10) | (1 << 11) | (1 << 12);  // Enable clock for all PIOs

    // DAP LED
    PIN_DAP_LED_PORT->PIO_PER = PIN_DAP_LED;
    PIN_DAP_LED_PORT->PIO_SODR = PIN_DAP_LED;
    PIN_DAP_LED_PORT->PIO_OER = PIN_DAP_LED;

    // MSD LED
    PIN_MSD_LED_PORT->PIO_PER = PIN_MSD_LED;
    PIN_MSD_LED_PORT->PIO_SODR = PIN_MSD_LED;
    PIN_MSD_LED_PORT->PIO_OER = PIN_MSD_LED;

    // CDC LED
    PIN_CDC_LED_PORT->PIO_PER = PIN_CDC_LED;
    PIN_CDC_LED_PORT->PIO_SODR = PIN_CDC_LED;
    PIN_CDC_LED_PORT->PIO_OER = PIN_CDC_LED;

    // Forwarded reset
    PIN_RESET_IN_FWRD_PORT->PIO_PER  = PIN_RESET_IN_FWRD; // Pin == GPIO control
    PIN_RESET_IN_FWRD_PORT->PIO_ODR  = PIN_RESET_IN_FWRD; // Disable output
    PIN_RESET_IN_FWRD_PORT->PIO_PUER = PIN_RESET_IN_FWRD; // Enable pull-up
    Cnt = 1000000;
    do {} while (--Cnt);    // Give pull-up some time to become active

    // Enable port A interrupts in the NVIC
    NVIC_EnableIRQ(PIOA_IRQn);
}

void gpio_set_hid_led(gpio_led_state_t state)
{
    if (GPIO_LED_ON == state) {
        PIN_DAP_LED_PORT->PIO_CODR = PIN_DAP_LED; // LED == on
    } else {
        PIN_DAP_LED_PORT->PIO_SODR = PIN_DAP_LED; // LED == off
    }
}

void gpio_set_cdc_led(gpio_led_state_t state)
{
    if (GPIO_LED_ON == state) {
        PIN_CDC_LED_PORT->PIO_CODR = PIN_CDC_LED; //  LED == on
    } else {
        PIN_CDC_LED_PORT->PIO_SODR = PIN_CDC_LED; //  LED == off
    }
}

void gpio_set_msc_led(gpio_led_state_t state)
{
    if (GPIO_LED_ON == state) {
        PIN_MSD_LED_PORT->PIO_CODR = PIN_MSD_LED; //  LED == on
    } else {
        PIN_MSD_LED_PORT->PIO_SODR = PIN_MSD_LED; //  LED == off
    }
}

void PIOA_IRQHandler(void)
{
    //
    // ISR is called when flow control is de-asserted
    //
    uint32_t interrupts = PIOA->PIO_ISR;

    if ((interrupts >> 9) & 1) { //CTS
        uart_software_flow_control();
    }
}

uint8_t gpio_get_reset_btn_no_fwrd(void)
{
    return 0;
}

uint8_t gpio_get_reset_btn_fwrd()
{
    return (PIN_RESET_IN_FWRD_PORT->PIO_PDSR & PIN_RESET_IN_FWRD) == 0;
}

void gpio_set_board_power(bool powerEnabled)
{
}
