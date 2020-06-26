/**
 * @file    debug_ca.h
 * @brief   Access to ARM DAP (Cortex-A) using CMSIS-DAP protocol
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

#ifndef DEBUG_CA_H
#define DEBUG_CA_H

#include "debug_cm.h"

// Debug registers
#define DEBUG_REGSITER_BASE (0x80030000)

#define DBGDIDR         (DEBUG_REGSITER_BASE +  (0    * 4))   // Debug ID
#define DBGWFAR         (DEBUG_REGSITER_BASE +  (6    * 4))   // Watchpoint Fault Address
#define DBGVCR          (DEBUG_REGSITER_BASE +  (7    * 4))   // Vector Catch
#define DBGECR          (DEBUG_REGSITER_BASE +  (9    * 4))   // Event Catch
#define DBGDTRRX        (DEBUG_REGSITER_BASE +  (32   * 4))   // Host to Target Data Transfer
#define DBGITR          (DEBUG_REGSITER_BASE +  (33   * 4))   // WO Instruction Transfer
#define DBGDSCR         (DEBUG_REGSITER_BASE +  (34   * 4))   // Debug Status and Control
#define DBGDTRTX        (DEBUG_REGSITER_BASE +  (35   * 4))   // Target to Host Data Transfer
#define DBGDRCR         (DEBUG_REGSITER_BASE +  (36   * 4))   // Debug Run Control
#define DBGEACR         (DEBUG_REGSITER_BASE +  (37   * 4))   // External Auxiliary Control
#define DBGPCSR         (DEBUG_REGSITER_BASE +  (40   * 4))   // Program Counter Sampling
#define DBGCIDSR        (DEBUG_REGSITER_BASE +  (41   * 4))   // Context ID Sampling
#define DBGVIDSR        (DEBUG_REGSITER_BASE +  (42   * 4))   // Virtualization ID Sampling
#define DBGBVR0         (DEBUG_REGSITER_BASE +  (64   * 4))   // Breakpoint Value
#define DBGBVR1         (DEBUG_REGSITER_BASE +  (65   * 4))   // Breakpoint Value
#define DBGBVR2         (DEBUG_REGSITER_BASE +  (66   * 4))   // Breakpoint Value
#define DBGBVR3         (DEBUG_REGSITER_BASE +  (67   * 4))   // Breakpoint Value
#define DBGBVR4         (DEBUG_REGSITER_BASE +  (68   * 4))   // Breakpoint Value
#define DBGBVR5         (DEBUG_REGSITER_BASE +  (69   * 4))   // Breakpoint Value
#define DBGBVR6         (DEBUG_REGSITER_BASE +  (70   * 4))   // Breakpoint Value
#define DBGBVR7         (DEBUG_REGSITER_BASE +  (71   * 4))   // Breakpoint Value
#define DBGBVR8         (DEBUG_REGSITER_BASE +  (72   * 4))   // Breakpoint Value
#define DBGBVR9         (DEBUG_REGSITER_BASE +  (73   * 4))   // Breakpoint Value
#define DBGBVR10        (DEBUG_REGSITER_BASE +  (74   * 4))   // Breakpoint Value
#define DBGBVR11        (DEBUG_REGSITER_BASE +  (75   * 4))   // Breakpoint Value
#define DBGBVR12        (DEBUG_REGSITER_BASE +  (76   * 4))   // Breakpoint Value
#define DBGBVR13        (DEBUG_REGSITER_BASE +  (77   * 4))   // Breakpoint Value
#define DBGBVR14        (DEBUG_REGSITER_BASE +  (78   * 4))   // Breakpoint Value
#define DBGBVR15        (DEBUG_REGSITER_BASE +  (79   * 4))   // Breakpoint Value
#define DBGBCR0         (DEBUG_REGSITER_BASE +  (80   * 4))   // Breakpoint Control
#define DBGBCR1         (DEBUG_REGSITER_BASE +  (81   * 4))   // Breakpoint Control
#define DBGBCR2         (DEBUG_REGSITER_BASE +  (82   * 4))   // Breakpoint Control
#define DBGBCR3         (DEBUG_REGSITER_BASE +  (83   * 4))   // Breakpoint Control
#define DBGBCR4         (DEBUG_REGSITER_BASE +  (84   * 4))   // Breakpoint Control
#define DBGBCR5         (DEBUG_REGSITER_BASE +  (85   * 4))   // Breakpoint Control
#define DBGBCR6         (DEBUG_REGSITER_BASE +  (86   * 4))   // Breakpoint Control
#define DBGBCR7         (DEBUG_REGSITER_BASE +  (87   * 4))   // Breakpoint Control
#define DBGBCR8         (DEBUG_REGSITER_BASE +  (88   * 4))   // Breakpoint Control
#define DBGBCR9         (DEBUG_REGSITER_BASE +  (89   * 4))   // Breakpoint Control
#define DBGBCR10        (DEBUG_REGSITER_BASE +  (90   * 4))   // Breakpoint Control
#define DBGBCR11        (DEBUG_REGSITER_BASE +  (91   * 4))   // Breakpoint Control
#define DBGBCR12        (DEBUG_REGSITER_BASE +  (92   * 4))   // Breakpoint Control
#define DBGBCR13        (DEBUG_REGSITER_BASE +  (93   * 4))   // Breakpoint Control
#define DBGBCR14        (DEBUG_REGSITER_BASE +  (94   * 4))   // Breakpoint Control
#define DBGBCR15        (DEBUG_REGSITER_BASE +  (95   * 4))   // Breakpoint Control
#define DBGWVR0         (DEBUG_REGSITER_BASE +  (96   * 4))   // Watchpoint Value
#define DBGWVR1         (DEBUG_REGSITER_BASE +  (97   * 4))   // Watchpoint Value
#define DBGWVR2         (DEBUG_REGSITER_BASE +  (98   * 4))   // Watchpoint Value
#define DBGWVR3         (DEBUG_REGSITER_BASE +  (99   * 4))   // Watchpoint Value
#define DBGWVR4         (DEBUG_REGSITER_BASE +  (100  * 4))   // Watchpoint Value
#define DBGWVR5         (DEBUG_REGSITER_BASE +  (101  * 4))   // Watchpoint Value
#define DBGWVR6         (DEBUG_REGSITER_BASE +  (102  * 4))   // Watchpoint Value
#define DBGWVR7         (DEBUG_REGSITER_BASE +  (103  * 4))   // Watchpoint Value
#define DBGWVR8         (DEBUG_REGSITER_BASE +  (104  * 4))   // Watchpoint Value
#define DBGWVR9         (DEBUG_REGSITER_BASE +  (105  * 4))   // Watchpoint Value
#define DBGWVR10        (DEBUG_REGSITER_BASE +  (106  * 4))   // Watchpoint Value
#define DBGWVR11        (DEBUG_REGSITER_BASE +  (107  * 4))   // Watchpoint Value
#define DBGWVR12        (DEBUG_REGSITER_BASE +  (108  * 4))   // Watchpoint Value
#define DBGWVR13        (DEBUG_REGSITER_BASE +  (109  * 4))   // Watchpoint Value
#define DBGWVR14        (DEBUG_REGSITER_BASE +  (110  * 4))   // Watchpoint Value
#define DBGWVR15        (DEBUG_REGSITER_BASE +  (111  * 4))   // Watchpoint Value
#define DBGWCR0         (DEBUG_REGSITER_BASE +  (112  * 4))   // Watchpoint Control
#define DBGWCR1         (DEBUG_REGSITER_BASE +  (113  * 4))   // Watchpoint Control
#define DBGWCR2         (DEBUG_REGSITER_BASE +  (114  * 4))   // Watchpoint Control
#define DBGWCR3         (DEBUG_REGSITER_BASE +  (115  * 4))   // Watchpoint Control
#define DBGWCR4         (DEBUG_REGSITER_BASE +  (116  * 4))   // Watchpoint Control
#define DBGWCR5         (DEBUG_REGSITER_BASE +  (117  * 4))   // Watchpoint Control
#define DBGWCR6         (DEBUG_REGSITER_BASE +  (118  * 4))   // Watchpoint Control
#define DBGWCR7         (DEBUG_REGSITER_BASE +  (119  * 4))   // Watchpoint Control
#define DBGWCR8         (DEBUG_REGSITER_BASE +  (120  * 4))   // Watchpoint Control
#define DBGWCR9         (DEBUG_REGSITER_BASE +  (121  * 4))   // Watchpoint Control
#define DBGWCR10        (DEBUG_REGSITER_BASE +  (122  * 4))   // Watchpoint Control
#define DBGWCR11        (DEBUG_REGSITER_BASE +  (123  * 4))   // Watchpoint Control
#define DBGWCR12        (DEBUG_REGSITER_BASE +  (124  * 4))   // Watchpoint Control
#define DBGWCR13        (DEBUG_REGSITER_BASE +  (125  * 4))   // Watchpoint Control
#define DBGWCR14        (DEBUG_REGSITER_BASE +  (126  * 4))   // Watchpoint Control
#define DBGWCR15        (DEBUG_REGSITER_BASE +  (127  * 4))   // Watchpoint Control
#define DBGBXVR0        (DEBUG_REGSITER_BASE +  (144  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR1        (DEBUG_REGSITER_BASE +  (145  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR2        (DEBUG_REGSITER_BASE +  (146  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR3        (DEBUG_REGSITER_BASE +  (147  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR4        (DEBUG_REGSITER_BASE +  (148  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR5        (DEBUG_REGSITER_BASE +  (149  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR6        (DEBUG_REGSITER_BASE +  (150  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR7        (DEBUG_REGSITER_BASE +  (151  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR8        (DEBUG_REGSITER_BASE +  (152  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR9        (DEBUG_REGSITER_BASE +  (153  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR10       (DEBUG_REGSITER_BASE +  (154  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR11       (DEBUG_REGSITER_BASE +  (155  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR12       (DEBUG_REGSITER_BASE +  (156  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR13       (DEBUG_REGSITER_BASE +  (157  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR14       (DEBUG_REGSITER_BASE +  (158  * 4))   // Breakpoint Extended Valueb
#define DBGBXVR15       (DEBUG_REGSITER_BASE +  (159  * 4))   // Breakpoint Extended Valueb
#define DBGOSLAR        (DEBUG_REGSITER_BASE +  (192  * 4))   // OS Lock Access
#define DBGOSLSR        (DEBUG_REGSITER_BASE +  (193  * 4))   // OS Lock Status
#define DBGPRCR         (DEBUG_REGSITER_BASE +  (196  * 4))   // Powerdown and Reset Control
#define DBGPRSR         (DEBUG_REGSITER_BASE +  (197  * 4))   // Powerdown and Reset Status
#define DBGITCTRL       (DEBUG_REGSITER_BASE +  (960  * 4))   // Integration Mode Control
#define DBGCLAIMSET     (DEBUG_REGSITER_BASE +  (1000 * 4))   // Claim Tag Set
#define DBGCLAIMCLR     (DEBUG_REGSITER_BASE +  (1001 * 4))   // Claim Tag Clear
#define DBGLAR          (DEBUG_REGSITER_BASE +  (1004 * 4))   // Lock Access
#define DBGLSR          (DEBUG_REGSITER_BASE +  (1005 * 4))   // Lock Status
#define DBGAUTHSTATUS   (DEBUG_REGSITER_BASE +  (1006 * 4))   // Authentication Status
#define DBGDEVID2       (DEBUG_REGSITER_BASE +  (1008 * 4))   // Debug Device ID 2
#define DBGDEVID1       (DEBUG_REGSITER_BASE +  (1009 * 4))   // Debug Device ID 1
#define DBGDEVID        (DEBUG_REGSITER_BASE +  (1010 * 4))   // Debug Device ID
#define DBGDEVTYPE      (DEBUG_REGSITER_BASE +  (1011 * 4))   // Device Type
#define DBGPID0         (DEBUG_REGSITER_BASE +  (1012 * 4))   // Debug Peripheral ID
#define DBGPID1         (DEBUG_REGSITER_BASE +  (1013 * 4))   // Debug Peripheral ID
#define DBGPID2         (DEBUG_REGSITER_BASE +  (1014 * 4))   // Debug Peripheral ID
#define DBGPID3         (DEBUG_REGSITER_BASE +  (1015 * 4))   // Debug Peripheral ID
#define DBGPID4         (DEBUG_REGSITER_BASE +  (1016 * 4))   // Debug Peripheral ID
#define DBGCID0         (DEBUG_REGSITER_BASE +  (1020 * 4))   // Debug Component ID
#define DBGCID1         (DEBUG_REGSITER_BASE +  (1021 * 4))   // Debug Component ID
#define DBGCID2         (DEBUG_REGSITER_BASE +  (1022 * 4))   // Debug Component ID
#define DBGCID3         (DEBUG_REGSITER_BASE +  (1023 * 4))   // Debug Component ID

#endif
