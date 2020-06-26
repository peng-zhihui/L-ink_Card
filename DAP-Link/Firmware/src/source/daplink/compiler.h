/**
 * @file    compiler.h
 * @brief   Special asserts and macros
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

#ifndef COMPILER_H
#define COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

#define COMPILER_CONCAT_(a, b) a##b
#define COMPILER_CONCAT(a, b) COMPILER_CONCAT_(a, b)

// Divide by zero if the the expression is false.  This
// causes an error at compile time.
//
// The special value '__COUNTER__' is used to create a unique value to
// append to 'compiler_assert_' to create a unique token.  This prevents
// conflicts resulting from the same enum being declared multiple times.
#define COMPILER_ASSERT(e) enum { COMPILER_CONCAT(compiler_assert_, __COUNTER__) = 1/((e) ? 1 : 0) }

#define __at(_addr) __attribute__ ((at(_addr)))

#ifdef __cplusplus
}
#endif

#endif
