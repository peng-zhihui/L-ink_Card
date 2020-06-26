/**
 * @file    validation.h
 * @brief   Helper functions to determine if a hex or binary file is valid
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

#ifndef VALIDATION_H
#define VALIDATION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t validate_bin_nvic(const uint8_t *buf);
uint8_t validate_hexfile(const uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif
