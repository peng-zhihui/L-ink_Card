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
#include "util.h"
#include "cortex_m.h"
#include "IO_Config.h"
#include "circ_buf.h"
#include "settings.h" // for config_get_overflow_detect

#define RX_OVRF_MSG         "<DAPLink:Overflow>\n"
#define RX_OVRF_MSG_SIZE    (sizeof(RX_OVRF_MSG) - 1)
#define BUFFER_SIZE         (512)

circ_buf_t write_buffer;
uint8_t write_buffer_data[BUFFER_SIZE];
circ_buf_t read_buffer;
uint8_t read_buffer_data[BUFFER_SIZE];

void clear_buffers(void)
{
    util_assert(!(UART->C2 & UART_C2_TIE_MASK));
    circ_buf_init(&write_buffer, write_buffer_data, sizeof(write_buffer_data));
    circ_buf_init(&read_buffer, read_buffer_data, sizeof(read_buffer_data));
}

int32_t uart_initialize(void)
{
    NVIC_DisableIRQ(UART_RX_TX_IRQn);

    // enable clk port
    if (UART_PORT == PORTA) {
        SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    }

    if (UART_PORT == PORTC) {
        SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    }

    if (UART_PORT == PORTD) {
        SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    }

    if (UART_PORT == PORTE) {
        SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    }

    // enable clk uart
    if (1 == UART_NUM) {
        SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
    }

    if (2 == UART_NUM) {
        SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    }

    // transmitter and receiver disabled
    UART->C2 &= ~(UART_C2_RE_MASK | UART_C2_TE_MASK);
    // disable interrupt
    UART->C2 &= ~(UART_C2_RIE_MASK | UART_C2_TIE_MASK);

    clear_buffers();

    // alternate setting
    UART_PORT->PCR[PIN_UART_RX_BIT] = PORT_PCR_MUX(PIN_UART_RX_MUX_ALT) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    UART_PORT->PCR[PIN_UART_TX_BIT] = PORT_PCR_MUX(PIN_UART_TX_MUX_ALT);
    // transmitter and receiver enabled
    UART->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK;
    // Enable receive interrupt
    UART->C2 |= UART_C2_RIE_MASK;
    NVIC_ClearPendingIRQ(UART_RX_TX_IRQn);
    NVIC_EnableIRQ(UART_RX_TX_IRQn);
    return 1;
}

int32_t uart_uninitialize(void)
{
    // transmitter and receiver disabled
    UART->C2 &= ~(UART_C2_RE_MASK | UART_C2_TE_MASK);
    // disable interrupt
    UART->C2 &= ~(UART_C2_RIE_MASK | UART_C2_TIE_MASK);
    clear_buffers();
    return 1;
}

int32_t uart_reset(void)
{
    // disable interrupt
    NVIC_DisableIRQ(UART_RX_TX_IRQn);
    // disable TIE interrupt
    UART->C2 &= ~(UART_C2_TIE_MASK);
    clear_buffers();
    // enable interrupt
    NVIC_EnableIRQ(UART_RX_TX_IRQn);
    return 1;
}

int32_t uart_set_configuration(UART_Configuration *config)
{
    uint8_t data_bits = 8;
    uint8_t parity_enable = 0;
    uint8_t parity_type = 0;
    uint32_t dll;
    // disable interrupt
    NVIC_DisableIRQ(UART_RX_TX_IRQn);
    UART->C2 &= ~(UART_C2_RIE_MASK | UART_C2_TIE_MASK);
    // Disable receiver and transmitter while updating
    UART->C2 &= ~(UART_C2_RE_MASK | UART_C2_TE_MASK);
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
    UART->C1 = data_bits << UART_C1_M_SHIFT |
               parity_enable << UART_C1_PE_SHIFT |
               parity_type << UART_C1_PT_SHIFT;
    dll =  SystemCoreClock / (16 * config->Baudrate);

    if (1 == UART_NUM || 2 == UART_NUM) {
        dll /= 2; //TODO <<= 1
    }

    // set baudrate
    UART->BDH = (UART->BDH & ~(UART_BDH_SBR_MASK)) | ((dll >> 8) & UART_BDH_SBR_MASK);
    UART->BDL = (UART->BDL & ~(UART_BDL_SBR_MASK)) | (dll & UART_BDL_SBR_MASK);
    // Enable transmitter and receiver
    UART->C2 |= UART_C2_RE_MASK | UART_C2_TE_MASK;
    // Enable UART interrupt
    NVIC_ClearPendingIRQ(UART_RX_TX_IRQn);
    NVIC_EnableIRQ(UART_RX_TX_IRQn);
    UART->C2 |= UART_C2_RIE_MASK;
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
        UART->C2 |= UART_C2_TIE_MASK;
    }
    cortex_int_restore(state);

    return cnt;
}

int32_t uart_read_data(uint8_t *data, uint16_t size)
{
    return circ_buf_read(&read_buffer, data, size);
}

void uart_enable_flow_control(bool enabled)
{
    // Flow control not implemented for this platform
}

void UART_RX_TX_IRQHandler(void)
{
    uint32_t s1;
    volatile uint8_t errorData;
    // read interrupt status
    s1 = UART->S1;
    // mask off interrupts that are not enabled
    if (!(UART->C2 & UART_C2_RIE_MASK)) {
        s1 &= ~UART_S1_RDRF_MASK;
    }
    if (!(UART->C2 & UART_C2_TIE_MASK)) {
        s1 &= ~UART_S1_TDRE_MASK;
    }

    // handle character to transmit
    if (s1 & UART_S1_TDRE_MASK) {
        // Assert that there is data in the buffer
        util_assert(circ_buf_count_used(&write_buffer) > 0);
        // Send out data
        UART1->D = circ_buf_pop(&write_buffer);
        // Turn off the transmitter if that was the last byte
        if (circ_buf_count_used(&write_buffer) == 0) {
            // disable TIE interrupt
            UART->C2 &= ~(UART_C2_TIE_MASK);
        }
    }

    // handle received character
    if (s1 & UART_S1_RDRF_MASK) {
        if ((s1 & UART_S1_NF_MASK) || (s1 & UART_S1_FE_MASK)) {
            errorData = UART->D;
        } else {
            uint32_t free;
            uint8_t data;
            
            data = UART1->D;
            free = circ_buf_count_free(&read_buffer);
            if (free > RX_OVRF_MSG_SIZE) {
                circ_buf_push(&read_buffer, data);
            } else if (config_get_overflow_detect()) {
                if (RX_OVRF_MSG_SIZE == free) {
                    circ_buf_write(&read_buffer, (uint8_t*)RX_OVRF_MSG, RX_OVRF_MSG_SIZE);
                } else {
                    // Drop newest
                }
            } else {
                // Drop oldest
                circ_buf_pop(&read_buffer);
                circ_buf_push(&read_buffer, data);
            }
        }
    }
}
