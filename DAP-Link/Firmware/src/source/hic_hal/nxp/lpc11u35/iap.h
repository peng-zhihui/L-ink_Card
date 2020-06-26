/**
 * @file    iap.h
 * @brief   
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2017-2017, ARM Limited, All Rights Reserved
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


#ifndef IAP_H
#define IAP_H

#include<stdint.h>

#define CMD_SUCCESS 0

typedef struct {
    uint32_t cmd;
    uint32_t par[4];
    uint32_t stat;
    uint32_t res[4];
} iap_operation_t;

extern iap_operation_t iap_op;

void iap_lock(void);
void iap_unlock(void);
void iap_call(iap_operation_t* operation);
void iap_reinvoke(void);

#endif
