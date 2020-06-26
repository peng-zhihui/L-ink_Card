/**
 * @file    power_ctrl.h
 * @brief   power control sequence logic for Musca A
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2018-2019, ARM Limited, All Rights Reserved
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

void BOD_IRQHandler(void);

void wait_for_pbon(void);

void configure_syscon(unsigned int pc);

void enable_BOD(int enable);

void power_off_sequence(void);

void power_on_sequence(void);

void i2cio_power_off(void);

void i2cio_power_on(void);

void i2cio_init(void);
