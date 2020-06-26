/**
 * @file    i2c_gpio.c
 * @brief   I2C GPIO control for musca PCA9537
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

#include "string.h"
#include "stdio.h"
#include "stdint.h"

#include "gpio.h"
#include "utils.h"
#include "i2c_gpio.h"

// Clock registers
#define I2CGPIO_FREQ    6            // Sets OSC Clock SCL frequency
#define I2CGPIO_WR      0            // Write command
#define I2CGPIO_RD      1            // Read command
#define I2CGPIO_ADDR    0x49         // Default slave address for PCA9537

/*----------------------------------------------------------------------------
  I2C Address + Wr + A
 *----------------------------------------------------------------------------*/
void i2c_gpio_addr(unsigned int addr, unsigned int read)
{
    unsigned int loop, data;
    
    // Repeated Start condition (if required after Command)
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);
    LPC_GPIO->DIR[PIN_I2C_SDA_PORT] &= ~PIN_I2C_SDA;
    delay_us(I2CGPIO_FREQ);
    
    // Start condition 'S' (DATA > CLK)
    LPC_GPIO->DIR[PIN_I2C_SDA_PORT] |= PIN_I2C_SDA;
    delay_us(I2CGPIO_FREQ);
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);
    
    // Addr is 7 bits so add Read
    data = (addr << 1) & 0xFE;
    if (read)
        data |= 0x01;
    
    // Clock out the 8 bits
    for (loop = 0; loop < 8; loop++)
    {
        if (data & (0x80 >> loop))
        {
            LPC_GPIO->DIR[PIN_I2C_SDA_PORT] &= ~PIN_I2C_SDA;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
        }
        else
        {
            LPC_GPIO->DIR[PIN_I2C_SDA_PORT] |= PIN_I2C_SDA;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
        }
    }

    // Set data low
    delay_us(I2CGPIO_FREQ);
    LPC_GPIO->DIR[PIN_I2C_SDA_PORT] |= PIN_I2C_SDA;
    delay_us(I2CGPIO_FREQ);
    
    // Transmission clock 'A'
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);
}

/*----------------------------------------------------------------------------
  I2C Command + A
 *----------------------------------------------------------------------------*/
void i2c_gpio_cmd(unsigned int cmd)
{
    unsigned int loop;
    
    // Clock out the 8 bits
    for (loop = 0; loop < 8; loop++)
    {
        if (cmd & (0x80 >> loop))
        {
            LPC_GPIO->DIR[PIN_I2C_SDA_PORT] &= ~PIN_I2C_SDA;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
        }
        else
        {
            LPC_GPIO->DIR[PIN_I2C_SDA_PORT] |= PIN_I2C_SDA;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
        }
    }

    // Set data low
    delay_us(I2CGPIO_FREQ);
    LPC_GPIO->DIR[PIN_I2C_SDA_PORT] |= PIN_I2C_SDA;
    delay_us(I2CGPIO_FREQ);
    
    // Transmission clock 'A'
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);
}

/*----------------------------------------------------------------------------
  I2C Write Data + A + P
 *----------------------------------------------------------------------------*/
void i2c_gpio_write(unsigned int data)
{
    unsigned int loop;
    
    // Clock out the 8 bits
    for (loop = 0; loop < 8; loop++)
    {
        if (data & (0x80 >> loop))
        {
            LPC_GPIO->DIR[PIN_I2C_SDA_PORT] &= ~PIN_I2C_SDA;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
        }
        else
        {
            LPC_GPIO->DIR[PIN_I2C_SDA_PORT] |= PIN_I2C_SDA;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
            LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
            delay_us(I2CGPIO_FREQ);
        }
    }

    // Set data low
    delay_us(I2CGPIO_FREQ);
    LPC_GPIO->DIR[PIN_I2C_SDA_PORT] |= PIN_I2C_SDA;
    delay_us(I2CGPIO_FREQ);
    
    // Transmission clock 'A'
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);
    
    // Stop condition 'P'
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);
    LPC_GPIO->DIR[PIN_I2C_SDA_PORT] &= ~PIN_I2C_SDA;
    delay_us(I2CGPIO_FREQ);
}

/*----------------------------------------------------------------------------
  I2C Read Data + A + P
 *----------------------------------------------------------------------------*/
void i2c_gpio_read(unsigned int *data, unsigned int ack)
{
    unsigned int loop;
    
    // Set SDA high (O/D) and allow PCA9537 to drive SDA
    LPC_GPIO->DIR[PIN_I2C_SDA_PORT] &= ~PIN_I2C_SDA;
    delay_us(I2CGPIO_FREQ);
    
    // Clock in the 8 bits
    *data = 0;
    for (loop = 0; loop < 8; loop++)
    {
        LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
        delay_us(I2CGPIO_FREQ);
        if (LPC_GPIO->DIR[PIN_I2C_SDA_PORT] & PIN_I2C_SDA)
            *data &= ~(0x80 >> loop);
        LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
        delay_us(I2CGPIO_FREQ);
    }

    // Set data for acknowledge
    delay_us(I2CGPIO_FREQ);
    if (ack)
        LPC_GPIO->DIR[PIN_I2C_SDA_PORT] |= PIN_I2C_SDA;
    else
        LPC_GPIO->DIR[PIN_I2C_SDA_PORT] &= ~PIN_I2C_SDA;
    delay_us(I2CGPIO_FREQ);
        
    // Transmission clock 'A'
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] |= PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);

    // End of acknowledge
    LPC_GPIO->DIR[PIN_I2C_SDA_PORT] |= PIN_I2C_SDA;
    delay_us(I2CGPIO_FREQ);
    
    // Stop condition 'P'
    LPC_GPIO->DIR[PIN_I2C_SCL_PORT] &= ~PIN_I2C_SCL;
    delay_us(I2CGPIO_FREQ);
    LPC_GPIO->DIR[PIN_I2C_SDA_PORT] &= ~PIN_I2C_SDA;
    delay_us(I2CGPIO_FREQ);
}

/*----------------------------------------------------------------------------
  I2C Write byte
 *----------------------------------------------------------------------------*/
void i2c_gpio_wbyte(unsigned int cmd, unsigned int data)
{
    // Set slave address write
    i2c_gpio_addr(I2CGPIO_ADDR, I2CGPIO_WR);
    // Set command
    i2c_gpio_cmd(cmd);
    // Write the data
    i2c_gpio_write(data);
}

/*----------------------------------------------------------------------------
  I2C Read byte
 *----------------------------------------------------------------------------*/
void i2c_gpio_rbyte(unsigned int cmd, unsigned int *data)
{
    // Set slave address write
    i2c_gpio_addr(I2CGPIO_ADDR, I2CGPIO_WR);
    // Set command
    i2c_gpio_cmd(cmd);
    
    // Set slave address write
    i2c_gpio_addr(I2CGPIO_ADDR, I2CGPIO_RD);
    // Read the data
    i2c_gpio_read(data, 0);
}

// end of i2c_gpio.c
