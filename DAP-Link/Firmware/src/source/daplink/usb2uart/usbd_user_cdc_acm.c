/**
 * @file    usbd_user_cdc_acm.c
 * @brief   implementation of USB CDC ACM middleware
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

#include "cmsis_os2.h"
#include "rl_usb.h"
#include "main.h"
#include "uart.h"
#ifdef DRAG_N_DROP_SUPPORT
#include "flash_intf.h"
#endif
#include "target_family.h"

UART_Configuration UART_Config;

/** @brief  Vitual COM Port initialization
 *
 *  The function inititalizes the hardware resources of the port used as
 *  the Virtual COM Port.
 *
 *  @return 0 Function failed.
 *  @return 1 Function succeeded.
 */
int32_t USBD_CDC_ACM_PortInitialize(void)
{
    uart_initialize();
    main_cdc_send_event();
    return 1;
}

/** @brief  Vitual COM Port uninitialization
 *
 *  The function uninititalizes/releases the hardware resources of the port used
 *  as the Virtual COM Port.
 *
 *  @return 0 Function failed.
 *  @return 1 Function succeeded.
 */
int32_t USBD_CDC_ACM_PortUninitialize(void)
{
    uart_uninitialize();
    return 1;
}

/** @brief  Vitual COM Port reset
 *
 *  The function resets the internal states of the port used
 *  as the Virtual COM Port.
 *
 *  @return 0 Function failed.
 *  @return 1 Function succeeded.
 */
int32_t USBD_CDC_ACM_PortReset(void)
{
    uart_reset();
    return 1;
}

/** @brief  Virtual COM Port change communication settings
 *
 *  The function changes communication settings of the port used as the
 *  Virtual COM Port.
 *
 *  @param [in] line_coding Pointer to the loaded CDC_LINE_CODING structure.
 *  @return 0 Function failed.
 *  @return 1 Function succeeded.
 */
int32_t USBD_CDC_ACM_PortSetLineCoding(CDC_LINE_CODING *line_coding)
{
    UART_Config.Baudrate    = line_coding->dwDTERate;
    UART_Config.DataBits    = (UART_DataBits) line_coding->bDataBits;
    UART_Config.Parity      = (UART_Parity)   line_coding->bParityType;
    UART_Config.StopBits    = (UART_StopBits) line_coding->bCharFormat;
    UART_Config.FlowControl = UART_FLOW_CONTROL_NONE;
    return uart_set_configuration(&UART_Config);
}

/** @brief  Vitual COM Port retrieve communication settings
 *
 * The function retrieves communication settings of the port used as the
 *  Virtual COM Port.
 *
 *  @param [in] line_coding Pointer to the CDC_LINE_CODING structure.
 *  @return 0 Function failed.
 *  @return 1 Function succeeded.
 */
int32_t USBD_CDC_ACM_PortGetLineCoding(CDC_LINE_CODING *line_coding)
{
    line_coding->dwDTERate   = UART_Config.Baudrate;
    line_coding->bDataBits   = UART_Config.DataBits;
    line_coding->bParityType = UART_Config.Parity;
    line_coding->bCharFormat = UART_Config.StopBits;
    return (1);
}

static U32 start_break_time = 0;
int32_t USBD_CDC_ACM_SendBreak(uint16_t dur)
{
    uint32_t end_break_time;
#ifdef DRAG_N_DROP_SUPPORT
    if (!flash_intf_target->flash_busy())
#endif
    { //added checking if flashing on target is in progress
        // reset and send the unique id over CDC
        if (dur != 0) {
            start_break_time = osKernelGetSysTimerCount();
            target_set_state(RESET_HOLD);
        } else {
            end_break_time = osKernelGetSysTimerCount();

            // long reset -> send uID over serial (300 -> break > 3s)
            if ((end_break_time - start_break_time) >= (300)) {
                main_reset_target(1);
            } else {
                main_reset_target(0);
            }
        }
    }
    return (1);
}

/** @brief  Virtual COM Port set control line state
 *
 *  The function sets control line state on the port used as the
 *  Virtual COM Port.
 *
 *  @param [in] ctrl_bmp Control line settings
 *      bitmap (0. bit - DTR state, 1. bit - RTS state).
 *  @return 0 Function failed.
 *  @return 1 Function succeeded.
 */
int32_t USBD_CDC_ACM_PortSetControlLineState(uint16_t ctrl_bmp)
{
    return (1);
}

void cdc_process_event()
{
    int32_t len_data = 0;
    uint8_t data[64];

    len_data = USBD_CDC_ACM_DataFree();

    if (len_data > sizeof(data)) {
        len_data = sizeof(data);
    }

    if (len_data) {
        len_data = uart_read_data(data, len_data);
    }

    if (len_data) {
        if (USBD_CDC_ACM_DataSend(data , len_data)) {
            main_blink_cdc_led(MAIN_LED_FLASH);
        }
    }

    len_data = uart_write_free();

    if (len_data > sizeof(data)) {
        len_data = sizeof(data);
    }

    if (len_data) {
        len_data = USBD_CDC_ACM_DataRead(data, len_data);
    }

    if (len_data) {
        if (uart_write_data(data, len_data)) {
            main_blink_cdc_led(MAIN_LED_FLASH);
        }
    }

    // Always process events
    main_cdc_send_event();
}
