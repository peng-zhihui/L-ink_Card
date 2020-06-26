/**
 * @file    uart.c
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

#include "string.h"
#include "uart.h"
#include "gpio.h"
#include "util.h"
#include "circ_buf.h"
#include "NuMicro.h"

#define RX_OVRF_MSG         "<DAPLink:Overflow>\n"
#define RX_OVRF_MSG_SIZE    (sizeof(RX_OVRF_MSG) - 1)
#define BUFFER_SIZE         (512)

#define TX_FIFO_SIZE        16 /* TX Hardware FIFO size */

circ_buf_t write_buffer;
uint8_t write_buffer_data[BUFFER_SIZE];
circ_buf_t read_buffer;
uint8_t read_buffer_data[BUFFER_SIZE];

static UART_Configuration configuration = {
    .Baudrate = 9600,
    .DataBits = UART_DATA_BITS_8,
    .Parity = UART_PARITY_NONE,
    .StopBits = UART_STOP_BITS_1,
    .FlowControl = UART_FLOW_CONTROL_NONE,
};

static void clear_buffers(void)
{
    circ_buf_init(&write_buffer, write_buffer_data, sizeof(write_buffer_data));
    circ_buf_init(&read_buffer, read_buffer_data, sizeof(read_buffer_data));
}

int32_t uart_initialize(void)
{
    clear_buffers();
    UART_Open(UART0, 115200);
    UART_ENABLE_INT(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_RXTOIEN_Msk));
    NVIC_EnableIRQ(UART0_IRQn);
    return 1;
}

int32_t uart_uninitialize(void)
{
    NVIC_DisableIRQ(UART0_IRQn);
    UART_Close(UART0);
    clear_buffers();
    return 1;
}

int32_t uart_reset(void)
{
    UART_Configuration backup_configuration = configuration;
    uart_set_configuration(&backup_configuration);
    return 1;
}

int32_t uart_set_configuration(UART_Configuration *config)
{
    uint32_t u32Reg;
    uint32_t u32Baud_Div;
    NVIC_DisableIRQ(UART0_IRQn);
    /* Reset hardware fifo */
    UART0->FIFO |= (UART_FIFO_TXRST_Msk | UART_FIFO_RXRST_Msk);
    /* Reset software fifo */
    clear_buffers();
    /* Set parity */
    configuration.Parity = config->Parity;

    if (config->Parity == UART_PARITY_ODD) {
        u32Reg = 0x08;
    } else if (config->Parity == UART_PARITY_EVEN) {
        u32Reg = 0x18;
    } else if (config->Parity == UART_PARITY_NONE) {
        u32Reg = 0;
    } else {
        u32Reg = 0;
    }

    /* Stop bit */
    configuration.StopBits = config->StopBits;

    if (config->StopBits == UART_STOP_BITS_2) {
        u32Reg |= 0x4;
    } else if (config->StopBits == UART_STOP_BITS_1_5) {
        u32Reg |= 0x4;
    } else if (config->StopBits == UART_STOP_BITS_1)
        ;

    /* Bit width */
    configuration.DataBits = config->DataBits;

    if (config->DataBits == UART_DATA_BITS_5) {
        u32Reg |= 0;
    } else if (config->DataBits == UART_DATA_BITS_6) {
        u32Reg |= 1;
    } else if (config->DataBits == UART_DATA_BITS_7) {
        u32Reg |= 2;
    } else if (config->DataBits == UART_DATA_BITS_8) {
        u32Reg |= 3;
    }

    configuration.FlowControl = UART_FLOW_CONTROL_NONE;
    /* Set baudrate */
    configuration.Baudrate = config->Baudrate;
    u32Baud_Div = UART_BAUD_MODE2_DIVIDER(__HXT, configuration.Baudrate);

    if (u32Baud_Div > 0xFFFF) {
        UART0->BAUD = (UART_BAUD_MODE0 | UART_BAUD_MODE0_DIVIDER(__HXT, configuration.Baudrate));
    } else {
        UART0->BAUD = (UART_BAUD_MODE2 | u32Baud_Div);
    }

    UART0->LINE = u32Reg;
    NVIC_EnableIRQ(UART0_IRQn);
    return 1;
}

int32_t uart_get_configuration(UART_Configuration *config)
{
    config->Baudrate = configuration.Baudrate;
    config->DataBits = configuration.DataBits;
    config->Parity   = configuration.Parity;
    config->StopBits = configuration.StopBits;
    config->FlowControl = UART_FLOW_CONTROL_NONE;
    return 1;
}

int32_t uart_write_free(void)
{
    return circ_buf_count_free(&write_buffer);
}

int32_t uart_write_data(uint8_t *data, uint16_t size)
{
    uint8_t bInChar;
    uint32_t u32Size = circ_buf_write(&write_buffer, data, size);

    if (circ_buf_count_used(&write_buffer) > 0) {
        if ((UART0->INTEN & UART_INTEN_THREIEN_Msk) == 0) {
            bInChar = circ_buf_pop(&write_buffer);
            /* Send one bytes out */
            UART_WRITE(UART0, bInChar);
            /* Enable Tx Empty Interrupt. (Trigger first one) */
            UART0->INTEN |= UART_INTEN_THREIEN_Msk;
        }
    }

    return u32Size;
}

int32_t uart_read_data(uint8_t *data, uint16_t size)
{
    return circ_buf_read(&read_buffer, data, size);
}

void UART0_IRQHandler(void)
{
    uint8_t bInChar;
    int32_t u32Size;
    uint32_t u32IntStatus;
    u32IntStatus = UART0->INTSTS;

    if ((u32IntStatus & UART_INTSTS_RDAINT_Msk) || (u32IntStatus & UART_INTSTS_RXTOINT_Msk)) {
        /* Receiver FIFO threshold level is reached or Rx time out */
        /* Get all the input characters */
        while ((!UART_GET_RX_EMPTY(UART0))) {
            /* Get the character from UART Buffer */
            bInChar = UART_READ(UART0); /* Rx trigger level is 1 byte*/
            /* Check if buffer full */
            uint32_t u32Free = circ_buf_count_free(&read_buffer);

            if (u32Free > RX_OVRF_MSG_SIZE) {
                circ_buf_push(&read_buffer, bInChar);
            } else if (RX_OVRF_MSG_SIZE == u32Free) {
                circ_buf_write(&read_buffer, (uint8_t *)RX_OVRF_MSG, RX_OVRF_MSG_SIZE);
            } else {
                // Drop character
            }
        }
    }

    if (u32IntStatus & UART_INTSTS_THREINT_Msk) {
        if (circ_buf_count_used(&write_buffer) > 0) {
            /* Fill the Tx FIFO */
            u32Size = circ_buf_count_used(&write_buffer);

            if (u32Size >= TX_FIFO_SIZE) {
                u32Size = TX_FIFO_SIZE;
            }

            while (u32Size) {
                bInChar = circ_buf_pop(&write_buffer);
                UART_WRITE(UART0, bInChar);
                u32Size--;
            }
        } else {
            /* No more data, just stop Tx (Stop work) */
            UART0->INTEN &= ~UART_INTEN_THREIEN_Msk;
        }
    }
}
