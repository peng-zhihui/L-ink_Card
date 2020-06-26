/**
 * @file    syscon.h
 * @brief   System Controller serial interface
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2017-2019, ARM Limited, All Rights Reserved
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

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __SYSCON_H
#define __SYSCON_H

/* SCC registers */
#define SCC_RESET_CTRL     0x00                     // RESET Control register
#define SCC_CLK_CTRL       0x04                     // CLOCK Control register
#define SCC_PWR_CTRL       0x08                     // Power Control register
#define SCC_PLL_CTRL       0x0C                     // System Clock divider register
#define SCC_DBG_CTRL       0x10                     // Debug Security override
#define SCC_SRAM_CTRL      0x14                     // SRAM Bank power gate enable
#define SCC_INTR_CTRL      0x18                     // Combined interrupt mask
#define SCC_CORDIO_CTRL    0x1C                     // REF to LLC Control and multiplex
#define SCC_CPU0_VTOR_SRAM 0x20                     // CPU0 SRAM Boot vector
#define SCC_SPARE0         0x24                     // CPU0 Spare
#define SCC_CPU1_VTOR_SRAM 0x28                     // CPU1 SRAM Boot vector
#define SCC_SPARE1         0x2C                     // CPU1 Spare

void syscon_readreg(unsigned int addr, unsigned int *din);
void syscon_writereg(unsigned int addr, unsigned int dout);

#endif
