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

#include "read_uid.h"
#include "iap.h"

void read_unique_id(uint32_t *id)
{
    iap_lock();

    // readUID IAP call
    iap_op.cmd = 58;
    iap_call(&iap_op);
    *id = iap_op.res[0] ^ iap_op.res[1] ^ iap_op.res[2] ^ iap_op.res[3];

    iap_unlock();
}
