/**
 * @file    tasks.h
 * @brief   Macros for configuring the run time tasks
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

#ifndef TASK_H
#define TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#define NO_TIMEOUT  (0xffff)

#define LOWEST_PRIORITY             (1)     /* Priority 0 is reserved for the RTX idle task */
#define HIGHEST_PRIORITY            (254)   /* Priority 255 is reserved by RTX */

#define MAIN_TASK_PRIORITY          (10)
#define TIMER_TASK_PRIORITY         (11)
#define TIMER_TASK_30_PRIORITY      (TIMER_TASK_PRIORITY)

// trouble here is that reset for different targets is implemented differently so all targets
//  have to use the largest stack or these have to be defined in multiple places... Not ideal
//  may want to move away from threads for some of these behaviours to optimize mempory usage (RAM)
#define TIMER_TASK_30_STACK (136)
#define MAIN_TASK_STACK     (800)

#ifdef __cplusplus
}
#endif

#endif
