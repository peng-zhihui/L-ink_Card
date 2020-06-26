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

#ifndef I2C_GPIO_H
#define I2C_GPIO_H

#define FL_ON           0x01        // FLASH power control
#define CR_ON           0x02        // Core power control
#define IO_ON           0x04        // I/O power control
#define FLASH_PDM       0x08        // FLASH PDM power control

void i2c_gpio_wbyte(unsigned int cmd, unsigned int data);
void i2c_gpio_rbyte(unsigned int cmd, unsigned int *data);

#endif
