/* CMSIS-DAP Interface Firmware
 * Copyright (c) 2009-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "read_uid.h"

#define UID_BASE 0x40001018

void read_unique_id(uint32_t * id) {
    uint32_t *uid_ptr = (uint32_t*)UID_BASE;
    *id = uid_ptr[0] ^ uid_ptr[1] ^ uid_ptr[2] ^ uid_ptr[3];
}
