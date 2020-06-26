/**
 * @file    FlashPrg.h
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

#ifndef FLASHPRG_H
#define FLASHPRG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize programming functions
 *  @param adr device base address
 *  @param clk clock frequency (Hz)
 *  @param fnc function code (1 - Erase, 2 - Program, 3 - Verify)
 *  @return 0 on success, an error code otherwise
 */
uint32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc);

/** De-Initialize programming functions [optional]
 *  @param fnc function code (1 - Erase, 2 - Program, 3 - Verify)
 *  @return 0 on success, an error code otherwise
 */
uint32_t UnInit(uint32_t fnc);

/** Check region for erased memory
 *  @param adr address to start from
 *  @param sz the amount of memory to check
 *  @param pat the pattern of erased memory (usually 0xff)
 *  @return 0 on success, an error code otherwise
 */
uint32_t BlankCheck(uint32_t adr, uint32_t sz, uint8_t pat);

/** Perform a full chip erase
 *  @return 0 on success, an error code otherwise
 */
uint32_t EraseChip(void);

/** Erase a single sector of memory
 *  @param adr address of a sector to erase
 *  @return 0 on success, an error code otherwise
 */
uint32_t EraseSector(uint32_t adr);

/** Program data into memory
 *  @param adr address to start programming from
 *  @param sz the amount of data to program
 *  @param buf memory contents to be programmed
 *  @return 0 on success, an error code otherwise
 */
uint32_t ProgramPage(uint32_t adr, uint32_t sz, uint32_t *buf);

/** Verify contents in memory
 *  @param adr start address of the verification
 *  @param sz the amount of data to be verified
 *  @param buf memory contents to be compared against
 *  @return 0 on success, an error code otherwise
 */
uint32_t Verify(uint32_t adr, uint32_t sz, uint32_t *buf);

#ifdef __cplusplus
}
#endif

#endif
