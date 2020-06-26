/**
 * @file    power_ctrl.c
 * @brief   power control sequence logic for Musca B
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
#include "i2c_gpio.h"
#include "daplink_debug.h"
#include "settings.h"
#include "region_defs.h"

// I2CIO expander init (PCA9537)
void i2cio_init(void)
{
    unsigned int data;

    // Default I2C state
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
    // Wait 1ms
    delay(1);
    LPC_GPIO->DIR[PIN_I2C_SDA_PORT] &= ~PIN_I2C_SDA;
    // Wait 1ms
    delay(1);

    // Dummy read
    i2c_gpio_rbyte(0, &data);
    // Wait 1ms
    delay(1);

    // Set default values
    // Output register (all PSUs OFF)
    i2c_gpio_wbyte(1, 0x00);
    // Wait 1ms
    delay(1);
    // Inversion register
    i2c_gpio_wbyte(2, 0x00);
    // Wait 1ms
    delay(1);
    // Configuration register
    i2c_gpio_wbyte(3, 0xF0);
    // Wait 100ms
    delay(100);
}

// I2CIO expander power OFF
// Sequence: ON > FLASH_PDM + /PWR_OK > /VDD_IO > /VDD_CORE > + /VDD_FLASH + /FLASH_PDM > OFF
void i2cio_power_off(void)
{
    i2c_gpio_wbyte(1, FL_ON | FLASH_PDM | CR_ON | IO_ON);
    LPC_GPIO->CLR[PIN_PWROK_PORT] = PIN_PWROK;

    i2c_gpio_wbyte(1, FL_ON | FLASH_PDM | CR_ON);

    i2c_gpio_wbyte(1, FL_ON | FLASH_PDM);

    i2c_gpio_wbyte(1, 0);
}

// I2CIO expander power ON
// Sequence: OFF > VDD_FLASH + FLASH_PDM > VDD_CORE > /FLASH_PDM + PWR_OK > VDD_IO > ON
void i2cio_power_on(void)
{
    i2c_gpio_wbyte(1, FL_ON | FLASH_PDM);
    // Wait 100ms
    delay(100);

    i2c_gpio_wbyte(1, FL_ON | FLASH_PDM | CR_ON);
    // Wait 100ms
    delay(100);

    i2c_gpio_wbyte(1, FL_ON | CR_ON);
    LPC_GPIO->SET[PIN_PWROK_PORT] = PIN_PWROK;
    // Wait 100ms
    delay(100);

    i2c_gpio_wbyte(1, FL_ON | CR_ON | IO_ON);
    // Wait 100ms
    delay(100);
}

// Configure SCC
void configure_syscon(unsigned int pc)
{
    unsigned int din, dout;

    // Dummy read
    syscon_readreg(0x004, &din);

    // CPU0 boot from QSPI/eFlash0/eFlash1/SRAM
    // pc = 0x10000000/0x1A000000/0x1A200000/0x1A400000;
    syscon_writereg(0x058, pc);
    
    // CPU1 boot from QSPI/eFlash0/eFlash1/SRAM
    // pc = 0x10000000/0x1A000000/0x1A200000/0x1A400000;
    syscon_writereg(0x060, pc);

    // QSPI divider for 20MHz
    dout = 0x00000001;
    syscon_writereg(0x010, dout);

    // Set PA[37:0] IO drive strength (4/8/4mA) and slew rate (Slow)
    syscon_writereg(0x0E8, 0xFFF00000);
    syscon_writereg(0x0EC, 0xFFFFFFC0);
    syscon_writereg(0x0F0, 0x000FFFFF);
    syscon_writereg(0x0F4, 0xFFFFFFFF);
    syscon_writereg(0x108, 0xFFFFFFFF);
    syscon_writereg(0x10C, 0x0000003F);
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
#if (defined MUSCA_B_BOOT_CODE_SRAM)
    configure_syscon(MUSCA_B_S_CODE_SRAM_START);
#elif (defined MUSCA_B_BOOT_EFLASH1)
    configure_syscon(MUSCA_B_S_EFLASH1_START);
#elif (defined MUSCA_B_BOOT_EFLASH0)
    configure_syscon(MUSCA_B_S_EFLASH0_START);
#elif (defined MUSCA_B_BOOT_QSPI_FLASH)
    configure_syscon(MUSCA_B_S_FLASH_START);
#endif

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

    // Turn OFF power
    i2cio_power_off();

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
