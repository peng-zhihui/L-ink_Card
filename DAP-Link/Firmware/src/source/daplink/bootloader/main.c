/**
 * @file    main.c
 * @brief   DAPLink Bootloader application entry point
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

#include "main.h"
#include "gpio.h"
#include "validation.h"
#include "vfs_manager.h"
#include "cmsis_os2.h"
#include "rl_usb.h"
#include "settings.h"
#include "info.h"
#include "target_config.h"
#include "util.h"
#include "cortex_m.h"
#include "sdk.h"
#include "target_board.h"

//default msc led settings
#ifndef MSC_LED_DEF
#define MSC_LED_DEF GPIO_LED_ON
#endif

__asm void modify_stack_pointer_and_start_app(uint32_t r0_sp, uint32_t r1_pc)
{
    MOV SP, R0
    BX R1
}

// Event flags for main task
// Timers events
#define FLAGS_MAIN_90MS         (1 << 0)
#define FLAGS_MAIN_30MS         (1 << 1)
// USB Events
#define FLAGS_MAIN_PROC_USB     (1 << 9)
// Used by msc when flashing a new binary
#define FLAGS_LED_BLINK_30MS    (1 << 6)

// Timing constants (in 90mS ticks)
// USB busy time
#define USB_BUSY_TIME           (33)
// Delay before a USB device connect may occur
#define USB_CONNECT_DELAY       (11)
// Decrement to zero
#define DECZERO(x)              (x ? --x : 0)
#define NO_TIMEOUT              (0xffff)

// Global state of usb used in
main_usb_connect_t usb_state;

// Reference to our main task
osThreadId_t main_task_id;

static uint8_t msc_led_usb_activity = 0;
static main_led_state_t msc_led_state = MAIN_LED_FLASH;

static main_usb_busy_t usb_busy;
static uint32_t usb_busy_count;

// Timer task, set flags every 30mS and 90mS
void timer_task_30mS(void * arg)
{
    static uint32_t i = 0;
    osThreadFlagsSet(main_task_id, FLAGS_MAIN_30MS);
    if (!(i++ % 3)) {
        osThreadFlagsSet(main_task_id, FLAGS_MAIN_90MS);
    }
}

// Flash MSC LED using 30mS tick
void main_blink_msc_led(main_led_state_t state)
{
    msc_led_usb_activity = 1;
    msc_led_state = state;
    return;
}

void USBD_SignalHandler()
{
    osThreadFlagsSet(main_task_id, FLAGS_MAIN_PROC_USB);
}

void main_task(void * arg)
{
    // State processing
    uint16_t flags;
    // LED
    gpio_led_state_t msc_led_value = MSC_LED_DEF;
    // USB
    uint32_t usb_state_count;

    if (config_ram_get_initial_hold_in_bl()) {
        // Delay for 1 second for VMs
        osDelay(100);
    }

    // Get a reference to this task
    main_task_id = osThreadGetId();
    // Set LED defaults
    gpio_set_hid_led(GPIO_LED_OFF);
    gpio_set_cdc_led(GPIO_LED_OFF);
    gpio_set_msc_led(msc_led_value);
    // Update version information file
    info_init();
    // USB
    usbd_init();
    vfs_mngr_init(true);
    usbd_connect(0);
    usb_busy = MAIN_USB_IDLE;
    usb_busy_count = 0;
    usb_state = MAIN_USB_CONNECTING;
    usb_state_count = USB_CONNECT_DELAY;
    // Start timer tasks
    osTimerId_t tmr_id = osTimerNew(timer_task_30mS, osTimerPeriodic, NULL, NULL);
    osTimerStart(tmr_id, 3);

    while (1) {
        // need to create a new event for programming failure
        flags = osThreadFlagsWait(FLAGS_MAIN_90MS     // 90mS tick
                        | FLAGS_MAIN_30MS            // 30mS tick
                        | FLAGS_MAIN_PROC_USB       // process usb events
                        , osFlagsWaitAny, 
                        osWaitForever);

        if (flags & FLAGS_MAIN_PROC_USB) {
            USBD_Handler();
        }

        if (flags & FLAGS_MAIN_90MS) {
            vfs_mngr_periodic(90); // FLAGS_MAIN_90MS

            // Update USB busy status
            switch (usb_busy) {
                case MAIN_USB_ACTIVE:
                    if (DECZERO(usb_busy_count) == 0) {
                        usb_busy = MAIN_USB_IDLE;
                    }

                    break;

                case MAIN_USB_IDLE:
                default:
                    break;
            }

            // Update USB connect status
            switch (usb_state) {
                case MAIN_USB_DISCONNECTING:

                    // Wait until USB is idle before disconnecting
                    if (usb_busy == MAIN_USB_IDLE && (DECZERO(usb_state_count) == 0)) {
                        usbd_connect(0);
                        usb_state = MAIN_USB_DISCONNECTED;
                    }

                    break;

                case MAIN_USB_CONNECTING:

                    // Wait before connecting
                    if (DECZERO(usb_state_count) == 0) {
                        usbd_connect(1);
                        usb_state = MAIN_USB_CHECK_CONNECTED;
                    }

                    break;

                case MAIN_USB_CHECK_CONNECTED:
                    if (usbd_configured()) {
                        usb_state = MAIN_USB_CONNECTED;
                    }

                    break;

                case MAIN_USB_DISCONNECTED:
                    SystemReset();
                    break;

                case MAIN_USB_CONNECTED:
                default:
                    break;
            }
        }

        // 30mS tick used for flashing LED when USB is busy
        if (flags & FLAGS_MAIN_30MS) {
            if (msc_led_usb_activity) {
                
                if ((msc_led_state == MAIN_LED_FLASH) || (msc_led_state == MAIN_LED_FLASH_PERMANENT)) {
                    // Toggle LED value
                    msc_led_value = (GPIO_LED_ON == msc_led_value) ? GPIO_LED_OFF : GPIO_LED_ON;
                    // If in flash mode stop after one cycle but in bootloader LED stays on
                    if ((MSC_LED_DEF == msc_led_value) && (MAIN_LED_FLASH == msc_led_state)) {    
                        msc_led_usb_activity = 0;
                        msc_led_state = MAIN_LED_DEF;
                    }

                }else{
                    //LED next state is MAIN_LED_DEF
                    msc_led_value = MSC_LED_DEF;
                    msc_led_usb_activity = 0;
                }

                // Update hardware
                gpio_set_msc_led(msc_led_value);
            }
        }
    }
}

int main(void)
{
    // initialize vendor sdk
    sdk_init();
    // init leds and button
    gpio_init();
    // init settings
    config_init();

    // check for invalid app image or rst button press. Should be checksum or CRC but NVIC validation is better than nothing.
    // If the interface has set the hold in bootloader setting don't jump to app
    if (!gpio_get_reset_btn() && g_board_info.target_cfg && validate_bin_nvic((uint8_t *)g_board_info.target_cfg->flash_regions[0].start) && !config_ram_get_initial_hold_in_bl()) {
        // change to the new vector table
        SCB->VTOR = g_board_info.target_cfg->flash_regions[0].start; //bootloaders should only have one flash region for interface
        // modify stack pointer and start app
        modify_stack_pointer_and_start_app((*(uint32_t *)(g_board_info.target_cfg->flash_regions[0].start)), (*(uint32_t *)(g_board_info.target_cfg->flash_regions[0].start + 4)));
    }

    // config the usb interface descriptor and web auth token before USB connects
    //unique_string_auth_config();
    // either the rst pin was pressed or we have an empty app region
    osKernelInitialize();                 // Initialize CMSIS-RTOS
    osThreadNew(main_task, NULL, NULL);    // Create application main thread
    osKernelStart();                      // Start thread execution
    for (;;) {}
}
