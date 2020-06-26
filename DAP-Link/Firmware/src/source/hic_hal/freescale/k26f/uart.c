/**
 * @file    uart.c
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
 * Copyright (c) 2016-2017 NXP
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
#include "fsl_device_registers.h"
#include "uart.h"
#include "util.h"
#include "cortex_m.h"
#include "circ_buf.h"
#include "settings.h" // for config_get_overflow_detect

#define UART_INSTANCE (UART0)
#define UART_IRQ (UART0_RX_TX_IRQn)

extern uint32_t SystemCoreClock;

static void clear_buffers(void);

#define RX_OVRF_MSG         "<DAPLink:Overflow>\n"
#define RX_OVRF_MSG_SIZE    (sizeof(RX_OVRF_MSG) - 1)
#define BUFFER_SIZE         (512)


circ_buf_t write_buffer;
uint8_t write_buffer_data[BUFFER_SIZE];
circ_buf_t read_buffer;
uint8_t read_buffer_data[BUFFER_SIZE];

void clear_buffers(void)
{
    util_assert(!(UART_INSTANCE->C2 & UART_C2_TIE_MASK));
    circ_buf_init(&write_buffer, write_buffer_data, sizeof(write_buffer_data));
    circ_buf_init(&read_buffer, read_buffer_data, sizeof(read_buffer_data));
}

int32_t uart_initialize(void)
{
    NVIC_DisableIRQ(UART_IRQ);
    // enable clk PORTC
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    // enable clk uart
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;

    // disable interrupt
    NVIC_DisableIRQ(UART_IRQ);
    // transmitter and receiver disabled
    UART_INSTANCE->C2 &= ~(UART_C2_RE_MASK | UART_C2_TE_MASK);
    // disable interrupt
    UART_INSTANCE->C2 &= ~(UART_C2_RIE_MASK | UART_C2_TIE_MASK);

    clear_buffers();

    // Enable receiver and transmitter
    UART_INSTANCE->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK;

    // alternate 3: UART0
    PORTB->PCR[16] = PORT_PCR_MUX(3);
    PORTB->PCR[17] = PORT_PCR_MUX(3);

    // Enable receive interrupt
    UART_INSTANCE->C2 |= UART_C2_RIE_MASK;
    NVIC_ClearPendingIRQ(UART_IRQ);
    NVIC_EnableIRQ(UART_IRQ);

    return 1;
}

int32_t uart_uninitialize(void)
{
    // transmitter and receiver disabled
    UART_INSTANCE->C2 &= ~(UART_C2_RE_MASK | UART_C2_TE_MASK);
    // disable interrupt
    UART_INSTANCE->C2 &= ~(UART_C2_RIE_MASK | UART_C2_TIE_MASK);
    clear_buffers();
    return 1;
}

int32_t uart_reset(void)
{
    // disable interrupt
    NVIC_DisableIRQ(UART_IRQ);
    // disable TIE interrupt
    UART_INSTANCE->C2 &= ~(UART_C2_TIE_MASK);
    clear_buffers();
    // enable interrupt
    NVIC_EnableIRQ(UART_IRQ);
    return 1;
}

int32_t uart_set_configuration(UART_Configuration *config)
{
    uint8_t data_bits = 8;
    uint8_t parity_enable = 0;
    uint8_t parity_type = 0;
    uint32_t dll;
    // disable interrupt
    NVIC_DisableIRQ(UART_IRQ);
    UART_INSTANCE->C2 &= ~(UART_C2_RIE_MASK | UART_C2_TIE_MASK);
    // Disable receiver and transmitter while updating
    UART_INSTANCE->C2 &= ~(UART_C2_RE_MASK | UART_C2_TE_MASK);
    clear_buffers();

    // set data bits, stop bits, parity
    if ((config->DataBits < 8) || (config->DataBits > 9)) {
        data_bits = 8;
    }

    data_bits -= 8;

    if (config->Parity == 1) {
        parity_enable = 1;
        parity_type = 1;
        data_bits++;
    } else if (config->Parity == 2) {
        parity_enable = 1;
        parity_type = 0;
        data_bits++;
    }

    // does not support 10 bit data comm
    if (data_bits == 2) {
        data_bits = 0;
        parity_enable = 0;
        parity_type = 0;
    }

    // data bits, parity and parity mode
    UART_INSTANCE->C1 = data_bits << UART_C1_M_SHIFT
                | parity_enable << UART_C1_PE_SHIFT
                | parity_type << UART_C1_PT_SHIFT;
    dll =  SystemCoreClock / (16 * config->Baudrate);
    // set baudrate
    UART_INSTANCE->BDH = (UART_INSTANCE->BDH & ~(UART_BDH_SBR_MASK)) | ((dll >> 8) & UART_BDH_SBR_MASK);
    UART_INSTANCE->BDL = (UART_INSTANCE->BDL & ~(UART_BDL_SBR_MASK)) | (dll & UART_BDL_SBR_MASK);
    // Enable transmitter and receiver
    UART_INSTANCE->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK;
    // Enable UART interrupt
    NVIC_ClearPendingIRQ(UART_IRQ);
    NVIC_EnableIRQ(UART_IRQ);
    UART_INSTANCE->C2 |= UART_C2_RIE_MASK;
    return 1;
}

int32_t uart_get_configuration(UART_Configuration *config)
{
    return 1;
}

int32_t uart_write_free(void)
{
    return circ_buf_count_free(&write_buffer);
}

int32_t uart_write_data(uint8_t *data, uint16_t size)
{
    cortex_int_state_t state;
    uint32_t cnt;

    cnt = circ_buf_write(&write_buffer, data, size);

    // Atomically enable TX
    state = cortex_int_get_and_disable();
    if (circ_buf_count_used(&write_buffer)) {
        UART_INSTANCE->C2 |= UART_C2_TIE_MASK;
    }
    cortex_int_restore(state);

    return cnt;
}

int32_t uart_read_data(uint8_t *data, uint16_t size)
{
    return circ_buf_read(&read_buffer, data, size);
}

void UART0_RX_TX_IRQHandler(void)
{
    uint32_t s1;
    volatile uint8_t errorData;
    // read interrupt status
    s1 = UART_INSTANCE->S1;
    // mask off interrupts that are not enabled
    if (!(UART_INSTANCE->C2 & UART_C2_RIE_MASK)) {
        s1 &= ~UART_S1_RDRF_MASK;
    }
    if (!(UART_INSTANCE->C2 & UART_C2_TIE_MASK)) {
        s1 &= ~UART_S1_TDRE_MASK;
    }

    // handle character to transmit
    if (s1 & UART_S1_TDRE_MASK) {
        // Assert that there is data in the buffer
        util_assert(circ_buf_count_used(&write_buffer) > 0);

        // Send out data
        UART_INSTANCE->D = circ_buf_pop(&write_buffer);
        // Turn off the transmitter if that was the last byte
        if (circ_buf_count_used(&write_buffer) == 0) {
            // disable TIE interrupt
            UART_INSTANCE->C2 &= ~(UART_C2_TIE_MASK);
        }
    }

    // handle received character
    if (s1 & UART_S1_RDRF_MASK) {
        if ((s1 & UART_S1_NF_MASK) || (s1 & UART_S1_FE_MASK)) {
            errorData = UART_INSTANCE->D;
        } else {
            uint32_t free;
            uint8_t data;

            data = UART_INSTANCE->D;
            free = circ_buf_count_free(&read_buffer);
            if (free > RX_OVRF_MSG_SIZE) {
                circ_buf_push(&read_buffer, data);
            } else if ((RX_OVRF_MSG_SIZE == free) && config_get_overflow_detect()) {
                circ_buf_write(&read_buffer, (uint8_t*)RX_OVRF_MSG, RX_OVRF_MSG_SIZE);
            } else {
                // Drop character
            }
        }
    }
}
