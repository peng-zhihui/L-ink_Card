/**
 * @file    read_uid.c
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
 
#include "stm32f1xx.h"
#include "read_uid.h"

void read_unique_id(uint32_t *id)
{
    uint32_t Device_Serial0, Device_Serial1, Device_Serial2;    
    
    Device_Serial0 = *(uint32_t*)(0x1FFFF7E8); 
    Device_Serial1 = *(uint32_t*)(0x1FFFF7EC); 
    Device_Serial2 = *(uint32_t*)(0x1FFFF7F0);    
  
    id[0] = Device_Serial0;
    id[1] = Device_Serial1;
    id[2] = Device_Serial2;
    id[3] = 0xA5A5A5A5;
}
