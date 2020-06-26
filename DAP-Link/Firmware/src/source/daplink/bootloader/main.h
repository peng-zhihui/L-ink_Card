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

/**
 *  @enum main_usb_busy_t
 *  @brief Defines the filtered activity of the USB connection
 */
typedef enum {
    MAIN_USB_IDLE,   /*!< MAIN_USB_IDLE value means no bus activity */
    MAIN_USB_ACTIVE  /*!< MAIN_USB_ACTIVE value means MSC transfers are occuring */
} main_usb_busy_t;

/**
 *  @enum main_usb_connect_t
 *  @brief State the USB connection can be in
 */
typedef enum {
    MAIN_USB_DISCONNECTED,       /*!< MAIN_USB_DISCONNECTED state for a USB */
    MAIN_USB_CONNECTING,         /*!< MAIN_USB_CONNECTING state for a USB */
    MAIN_USB_CONNECTED,          /*!< MAIN_USB_CONNECTED state for a USB */
    MAIN_USB_CHECK_CONNECTED,    /*!< MAIN_USB_CHECK_CONNECTED state for a USB */
    MAIN_USB_DISCONNECTING       /*!< MAIN_USB_DISCONNECTING state for a USB */
} main_usb_connect_t;

/**
 *  @enum main_led_state_t
 *  @brief Statest the USB connection can be in
 */
typedef enum {
    MAIN_LED_DEF = 0,
    MAIN_LED_FLASH,
    MAIN_LED_FLASH_PERMANENT
} main_led_state_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Allows other parts of the program to request the device to eject physical media
 *  @param  none
 *  @return none
 */
void main_msc_disconnect_event(void);

/**
 *  Delay the disconnect if there is one in progress
 *  @param  none
 *  @return none
 */
void main_msc_delay_disconnect_event(void);

/**
 *  Allows other parts of the program to request the device to immediately eject physical media
 *  @param  none
 *  @return none
 */
void main_force_msc_disconnect_event(void);

/**
 *  Allows other parts of the program to request a LED to toggle state
 *  @param  permanent decides if the led should remain on or flash
 *  @return none
 */
void main_blink_msc_led(main_led_state_t permanent);

#ifdef __cplusplus
}
#endif

#endif
