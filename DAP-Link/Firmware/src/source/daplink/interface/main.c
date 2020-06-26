/**
 * @file    main.c
 * @brief   Entry point for interface program logic
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

#include <string.h>
#include <stdio.h>

#include "cmsis_os2.h"
#include "rl_usb.h"
#include "main.h"
#include "gpio.h"
#include "uart.h"
#include "tasks.h"
#include "swd_host.h"
#include "info.h"
#include "settings.h"
#include "daplink.h"
#include "util.h"
#include "DAP.h"
#include "bootloader.h"
#include "cortex_m.h"
#include "sdk.h"
#include "target_family.h"
#include "target_board.h"

#ifdef DRAG_N_DROP_SUPPORT
#include "vfs_manager.h"
#include "flash_intf.h"
#include "flash_manager.h"
#endif

// Event flags for main task
// Timers events
#define FLAGS_MAIN_90MS         (1 << 0)
#define FLAGS_MAIN_30MS         (1 << 1)
// Reset events
#define FLAGS_MAIN_RESET        (1 << 2)
// Other Events
#define FLAGS_MAIN_POWERDOWN    (1 << 4)
#define FLAGS_MAIN_DISABLEDEBUG (1 << 5)
#define FLAGS_MAIN_PROC_USB     (1 << 9)
// Used by cdc when an event occurs
#define FLAGS_MAIN_CDC_EVENT    (1 << 11)
// Used by msd when flashing a new binary
#define FLAGS_LED_BLINK_30MS    (1 << 6)

// Timing constants (in 90mS ticks)
// USB busy time (~3 sec)
#define USB_BUSY_TIME           (33)
// Delay before a USB device connect may occur (~1 sec)
#define USB_CONNECT_DELAY       (11)
// Timeout for USB being configured (~2 sec)
#define USB_CONFIGURE_TIMEOUT   (22)
// Delay before target may be taken out of reset or reprogrammed after startup
#define STARTUP_DELAY           (1)

// Decrement to zero
#define DECZERO(x)              (x ? --x : 0)

//default hid led settings
#ifndef HID_LED_DEF
#define HID_LED_DEF GPIO_LED_OFF
#endif

//default cdc led settings
#ifndef CDC_LED_DEF
#define CDC_LED_DEF GPIO_LED_OFF
#endif

//default msc led settings
#ifndef MSC_LED_DEF
#define MSC_LED_DEF GPIO_LED_OFF
#endif

// Reference to our main task
osThreadId_t main_task_id;

// USB busy LED state; when TRUE the LED will flash once using 30mS clock tick
static uint8_t hid_led_usb_activity = 0;
static uint8_t cdc_led_usb_activity = 0;
static uint8_t msc_led_usb_activity = 0;
static main_led_state_t hid_led_state = MAIN_LED_FLASH;
static main_led_state_t cdc_led_state = MAIN_LED_FLASH;
static main_led_state_t msc_led_state = MAIN_LED_FLASH;

// Global state of usb
main_usb_connect_t usb_state;
static bool usb_test_mode = false;

// Timer task, set flags every 30mS and 90mS
void timer_task_30mS(void * arg)
{
    static uint32_t i = 0;
    osThreadFlagsSet(main_task_id, FLAGS_MAIN_30MS);
    if (!(i++ % 3)) {
        osThreadFlagsSet(main_task_id, FLAGS_MAIN_90MS);
    }
}

// Functions called from other tasks to trigger events in the main task
// parameter should be reset type??
void main_reset_target(uint8_t send_unique_id)
{
    osThreadFlagsSet(main_task_id, FLAGS_MAIN_RESET);
    return;
}

// Flash HID LED using 30mS tick
void main_blink_hid_led(main_led_state_t state)
{
    hid_led_usb_activity = 1;
    hid_led_state = state;
    return;
}

// Flash CDC LED using 30mS tick
void main_blink_cdc_led(main_led_state_t state)
{
    cdc_led_usb_activity = 1;
    cdc_led_state = state;
    return;
}

// Flash MSC LED using 30mS tick
void main_blink_msc_led(main_led_state_t state)
{
    msc_led_usb_activity = 1;
    msc_led_state = state;
    return;
}

// Power down the interface
void main_powerdown_event(void)
{
    osThreadFlagsSet(main_task_id, FLAGS_MAIN_POWERDOWN);
    return;
}

// Disable debug on target
void main_disable_debug_event(void)
{
    osThreadFlagsSet(main_task_id, FLAGS_MAIN_DISABLEDEBUG);
    return;
}

// Start CDC processing
void main_cdc_send_event(void)
{
    osThreadFlagsSet(main_task_id, FLAGS_MAIN_CDC_EVENT);
    return;
}

void main_usb_set_test_mode(bool enabled)
{
    usb_test_mode = enabled;
}

void USBD_SignalHandler()
{
    osThreadFlagsSet(main_task_id, FLAGS_MAIN_PROC_USB);
}

extern void cdc_process_event(void);

void main_task(void * arg)
{
    // State processing
    uint16_t flags = 0;
    // LED
    gpio_led_state_t hid_led_value = HID_LED_DEF;
    gpio_led_state_t cdc_led_value = CDC_LED_DEF;
    gpio_led_state_t msc_led_value = MSC_LED_DEF;
    // USB
    uint32_t usb_state_count = USB_BUSY_TIME;
    uint32_t usb_no_config_count = USB_CONFIGURE_TIMEOUT;
    // button state
    uint8_t reset_pressed = 0;
#ifdef PBON_BUTTON
    uint8_t power_on = 1;
#endif

    // Initialize settings - required for asserts to work
    config_init();
    // Update bootloader if it is out of date
    bootloader_check_and_update();
    // Get a reference to this task
    main_task_id = osThreadGetId();
    // leds
    gpio_init();
    // Turn to LED default settings
    gpio_set_hid_led(hid_led_value);
    gpio_set_cdc_led(cdc_led_value);
    gpio_set_msc_led(msc_led_value);
    // Initialize the DAP
    DAP_Setup();

    // make sure we have a valid board info structure.
    util_assert(g_board_info.info_version == kBoardInfoVersion);

    // do some init with the target before USB and files are configured
    if (g_board_info.prerun_board_config) {
        g_board_info.prerun_board_config();
    }

    //initialize the family
    init_family();

    if (g_target_family && g_target_family->prerun_target_config) {
        g_target_family->prerun_target_config();
    }

    //setup some flags
    if (g_board_info.flags & kEnableUnderResetConnect) {
        swd_set_reset_connect(CONNECT_UNDER_RESET);
    }
    if (g_board_info.flags & kEnablePageErase) {
#ifdef DRAG_N_DROP_SUPPORT
        flash_manager_set_page_erase(true);
#endif
    }

    // Update versions and IDs
    info_init();
    // USB
    usbd_init();
#ifdef DRAG_N_DROP_SUPPORT
    vfs_mngr_fs_enable((config_ram_get_disable_msd()==0));
#endif
    usbd_connect(0);
    usb_state = USB_CONNECTING;
    usb_state_count = USB_CONNECT_DELAY;

    // Start timer tasks
    osTimerId_t tmr_id = osTimerNew(timer_task_30mS, osTimerPeriodic, NULL, NULL);
    osTimerStart(tmr_id, 3);
    while (1) {
        flags = osThreadFlagsWait(FLAGS_MAIN_RESET             // Put target in reset state
                       | FLAGS_MAIN_90MS            // 90mS tick
                       | FLAGS_MAIN_30MS            // 30mS tick
                       | FLAGS_MAIN_POWERDOWN       // Power down interface
                       | FLAGS_MAIN_DISABLEDEBUG    // Disable target debug
                       | FLAGS_MAIN_PROC_USB        // process usb events
                       | FLAGS_MAIN_CDC_EVENT       // cdc event
                       , osFlagsWaitAny
                       , osWaitForever);

        if (flags & FLAGS_MAIN_PROC_USB) {
            if (usb_test_mode) {
                // When in USB test mode Insert a delay to
                // simulate worst-case behavior.
                osDelay(1);
            }
            USBD_Handler();
        }

        if (flags & FLAGS_MAIN_RESET) {
            target_set_state(RESET_RUN);
        }

        if (flags & FLAGS_MAIN_POWERDOWN) {
            // Disable debug
            target_set_state(NO_DEBUG);
            // Disable board power before USB is disconnected.
            gpio_set_board_power(false);
            // Disconnect USB
            usbd_connect(0);
            // Turn off LED
            gpio_set_hid_led(GPIO_LED_OFF);
            gpio_set_cdc_led(GPIO_LED_OFF);
            gpio_set_msc_led(GPIO_LED_OFF);

            // TODO: put the interface chip in sleep mode
            while (1);
        }

        if (flags & FLAGS_MAIN_DISABLEDEBUG) {
            // Disable debug
            target_set_state(NO_DEBUG);
        }

        if (flags & FLAGS_MAIN_CDC_EVENT) {
            cdc_process_event();
        }

        if (flags & FLAGS_MAIN_90MS) {
            // Update USB busy status
#ifdef DRAG_N_DROP_SUPPORT
            vfs_mngr_periodic(90); // FLAGS_MAIN_90MS
#endif
            // Update USB connect status
            switch (usb_state) {
                case USB_DISCONNECTING:
                    usb_state = USB_DISCONNECTED;
                    // Disable board power before USB is disconnected.
                    gpio_set_board_power(false);
                    usbd_connect(0);
                    break;

                case USB_CONNECTING:
                    // Wait before connecting
                    if (DECZERO(usb_state_count) == 0) {
                        usbd_connect(1);
                        usb_state = USB_CHECK_CONNECTED;
                        // Reset connect timeout
                        usb_no_config_count = USB_CONFIGURE_TIMEOUT;
                    }

                    break;

                case USB_CHECK_CONNECTED:
                    if (usbd_configured()) {
                        // Let the HIC enable power to the target now that high power has been negotiated.
                        gpio_set_board_power(true);

                        usb_state = USB_CONNECTED;
                    }
                    else if (DECZERO(usb_no_config_count) == 0) {
                        // USB configuration timed out, which most likely indicates that the HIC is
                        // powered by a USB wall wart or similar power source. Go ahead and enable
                        // board power.
                        gpio_set_board_power(true);
                    }

                    break;

                case USB_CONNECTED:
                case USB_DISCONNECTED:
                default:
                    break;
            }
        }

        // 30mS tick used for flashing LED when USB is busy
        if (flags & FLAGS_MAIN_30MS) {

            // handle reset button without eventing
            if (!reset_pressed && gpio_get_reset_btn_fwrd()) {
#ifdef DRAG_N_DROP_SUPPORT
               if (!flash_intf_target->flash_busy()) //added checking if flashing on target is in progress
#endif
                {
                    // Reset button pressed
                    target_set_state(RESET_HOLD);
                    reset_pressed = 1;
                }
            } else if (reset_pressed && !gpio_get_reset_btn_fwrd()) {
                // Reset button released
                target_set_state(RESET_RUN);
                reset_pressed = 0;
            }

#ifdef PBON_BUTTON
            // handle PBON pressed
            if(gpio_get_pbon_btn())
            {
                if(power_on)
                {
                    // Loop till PBON is pressed
                    while (gpio_get_pbon_btn()) {;}
                    // Power button released when target was running
                    target_set_state(SHUTDOWN);
                    power_on = 0;
                }
                else
                {
                    // Loop till PBON is pressed
                    while (gpio_get_pbon_btn()) {;}
                    // Power button released when target was already powered off
                    target_set_state(POWER_ON);
                    power_on = 1;
                }
            }
#endif

            // DAP LED
            if (hid_led_usb_activity) {

                if ((hid_led_state == MAIN_LED_FLASH) || (hid_led_state == MAIN_LED_FLASH_PERMANENT)) {
                    // Toggle LED value
                    hid_led_value = GPIO_LED_ON == hid_led_value ? GPIO_LED_OFF : GPIO_LED_ON;

                    // If in flash mode stop after one cycle
                    if ((HID_LED_DEF == hid_led_value) && (MAIN_LED_FLASH == hid_led_state)) {
                        hid_led_usb_activity = 0;
                        hid_led_state = MAIN_LED_DEF;
                    }
                } else {
                    //LED next state is MAIN_LED_DEF
                    hid_led_value = HID_LED_DEF;
                    hid_led_usb_activity = 0;
                }

                // Update hardware
                gpio_set_hid_led(hid_led_value);
            }

            // MSD LED
            if (msc_led_usb_activity) {

                if ((msc_led_state == MAIN_LED_FLASH) || (msc_led_state == MAIN_LED_FLASH_PERMANENT)) {
                    // Toggle LED value
                    msc_led_value = GPIO_LED_ON == msc_led_value ? GPIO_LED_OFF : GPIO_LED_ON;

                    // If in flash mode stop after one cycle
                    if ((MSC_LED_DEF == msc_led_value) && (MAIN_LED_FLASH == msc_led_state)) {
                        msc_led_usb_activity = 0;
                        msc_led_state = MAIN_LED_DEF;
                    }
                } else {
                    //LED next state is MAIN_LED_DEF
                    msc_led_value = MSC_LED_DEF;
                    msc_led_usb_activity = 0;
                }

                // Update hardware
                gpio_set_msc_led(msc_led_value);
            }

            // CDC LED
            if (cdc_led_usb_activity) {

                if ((cdc_led_state == MAIN_LED_FLASH) || (cdc_led_state == MAIN_LED_FLASH_PERMANENT)){
                    // Toggle LED value
                    cdc_led_value = GPIO_LED_ON == cdc_led_value ? GPIO_LED_OFF : GPIO_LED_ON;

                    // If in flash mode stop after one cycle
                    if ((CDC_LED_DEF == cdc_led_value) && (MAIN_LED_FLASH == cdc_led_state)) {
                        cdc_led_usb_activity = 0;
                        cdc_led_state = MAIN_LED_DEF;
                    }
                }else{
                    //LED next state is MAIN_LED_DEF
                    cdc_led_value = CDC_LED_DEF;
                    cdc_led_usb_activity = 0;
                }

                // Update hardware
                gpio_set_cdc_led(cdc_led_value);
            }
        }
    }
}

int main(void)
{
    // Explicitly set the vector table since the bootloader might not set
    // it to what we expect.
#if DAPLINK_ROM_BL_SIZE > 0
    SCB->VTOR = SCB_VTOR_TBLOFF_Msk & DAPLINK_ROM_IF_START;
#endif
    // initialize vendor sdk
    sdk_init();

    osKernelInitialize();                 // Initialize CMSIS-RTOS
    osThreadNew(main_task, NULL, NULL);    // Create application main thread
    osKernelStart();                      // Start thread execution
    for (;;) {}
}
