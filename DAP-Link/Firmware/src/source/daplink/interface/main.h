/**
 * @file    main.h
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
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

#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stdbool.h>

// LED state
typedef enum main_led_state {
    MAIN_LED_DEF = 0,
    MAIN_LED_FLASH,
    MAIN_LED_FLASH_PERMANENT
} main_led_state_t;

typedef enum main_usb_connect {
    USB_DISCONNECTED,
    USB_CONNECTING,
    USB_CONNECTED,
    USB_CHECK_CONNECTED,
    USB_CONFIGURED,
    USB_DISCONNECTING
} main_usb_connect_t;

#ifdef __cplusplus
extern "C" {
#endif

void main_reset_target(uint8_t send_unique_id);
void main_usb_set_test_mode(bool enabled);
void main_powerdown_event(void);
void main_disable_debug_event(void);
void main_cdc_send_event(void);
void main_msc_disconnect_event(void);
void main_msc_delay_disconnect_event(void);
void main_force_msc_disconnect_event(void);
void main_blink_hid_led(main_led_state_t state);
void main_blink_msc_led(main_led_state_t state);
void main_blink_cdc_led(main_led_state_t state);

#ifdef __cplusplus
}
#endif

#endif
