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

#include <string.h>
#include "max32625.h"
#include "IO_Config.h"
#include "clkman_regs.h"
#include "ioman_regs.h"
#include "gpio_regs.h"
#include "uart_regs.h"
#include "pwrman_regs.h"
#include "uart.h"

// Size must be 2^n
#define  BUFFER_SIZE (4096)

#define UART_ERRORS (MXC_F_UART_INTFL_RX_FRAMING_ERR | \
                     MXC_F_UART_INTFL_RX_PARITY_ERR | \
                     MXC_F_UART_INTFL_RX_FIFO_OVERFLOW)


// Track bit rate to avoid calculation from bus clock, clock scaler and baud divisor values
static uint32_t baudrate;

static mxc_uart_regs_t *CdcAcmUart = NULL;
static mxc_uart_fifo_regs_t *CdcAcmUartFifo = NULL;
static IRQn_Type CdcAcmUartIrqNumber = MXC_IRQ_EXT_COUNT;

static struct {
    uint8_t  data[BUFFER_SIZE];
    volatile uint16_t idx_in;
    volatile uint16_t idx_out;
    volatile int16_t cnt_in;
    volatile int16_t cnt_out;
} write_buffer, read_buffer;

/******************************************************************************/
static void set_bitrate(uint32_t bps)
{
    uint32_t baud_divisor;

    baud_divisor = SystemCoreClock / (1 << (MXC_CLKMAN->sys_clk_ctrl_8_uart - 1));
    baud_divisor /= (bps * 16);
    CdcAcmUart->baud = baud_divisor;

    baudrate = bps;
}

/******************************************************************************/
int32_t uart_set_instance(uint32_t inst)
{
    if (inst == 0) {
        CdcAcmUart = MXC_UART0;
        CdcAcmUartFifo = MXC_UART0_FIFO;
        CdcAcmUartIrqNumber = UART0_IRQn;
    } else if (inst == 2) {
        CdcAcmUart = MXC_UART2;
        CdcAcmUartFifo = MXC_UART2_FIFO;
        CdcAcmUartIrqNumber = UART2_IRQn;
    } else {
        return 0;
    }
    return 1;
}

/******************************************************************************/
int32_t uart_initialize(void)
{
    int idx;

    if (CdcAcmUart == MXC_UART0) {
        MXC_CLKMAN->clk_gate_ctrl1 |= MXC_F_CLKMAN_CLK_GATE_CTRL1_UART0_CLK_GATER;
        if (MXC_CLKMAN->sys_clk_ctrl_8_uart != MXC_S_CLKMAN_CLK_SCALE_DIV_4) {
            MXC_CLKMAN->sys_clk_ctrl_8_uart = MXC_S_CLKMAN_CLK_SCALE_DIV_4;
        }

        // Configure GPIO for UART
        MXC_IOMAN->uart0_req = ((MXC_V_IOMAN_MAP_A << MXC_F_IOMAN_UART0_REQ_IO_MAP_POS) | MXC_F_IOMAN_UART0_REQ_IO_REQ);
        while (MXC_IOMAN->uart0_ack != ((MXC_V_IOMAN_MAP_A << MXC_F_IOMAN_UART0_REQ_IO_MAP_POS) | MXC_F_IOMAN_UART0_REQ_IO_REQ));

    } else if (CdcAcmUart == MXC_UART2) {
        MXC_CLKMAN->clk_gate_ctrl1 |= MXC_F_CLKMAN_CLK_GATE_CTRL1_UART2_CLK_GATER;
        if (MXC_CLKMAN->sys_clk_ctrl_8_uart != MXC_S_CLKMAN_CLK_SCALE_DIV_4) {
            MXC_CLKMAN->sys_clk_ctrl_8_uart = MXC_S_CLKMAN_CLK_SCALE_DIV_4;
        }

        // Configure GPIO for UART
        MXC_IOMAN->uart2_req = ((MXC_V_IOMAN_MAP_A << MXC_F_IOMAN_UART2_REQ_IO_MAP_POS) | MXC_F_IOMAN_UART2_REQ_IO_REQ);
        while (MXC_IOMAN->uart2_ack != ((MXC_V_IOMAN_MAP_A << MXC_F_IOMAN_UART2_REQ_IO_MAP_POS) | MXC_F_IOMAN_UART2_REQ_IO_REQ));
    } else {
        return 0;
    }

    idx = MXC_UART_GET_IDX(CdcAcmUart);
    MXC_PWRMAN->peripheral_reset |= (MXC_F_PWRMAN_PERIPHERAL_RESET_UART0 << idx);
    MXC_PWRMAN->peripheral_reset &= ~((MXC_F_PWRMAN_PERIPHERAL_RESET_UART0 << idx));

    // Flush RX and TX FIFOS
    CdcAcmUart->ctrl &= ~(MXC_F_UART_CTRL_RX_FIFO_EN | MXC_F_UART_CTRL_TX_FIFO_EN);

    // Disable interrupts
    CdcAcmUart->inten = 0;
    CdcAcmUart->intfl = CdcAcmUart->intfl;

    // Set the parity, size, stop and flow configuration
    CdcAcmUart->ctrl |= (MXC_S_UART_CTRL_DATA_SIZE_8_BITS | MXC_S_UART_CTRL_PARITY_DISABLE);

    // Set receive fifo threshold to 0
    CdcAcmUart->rx_fifo_ctrl &= ~MXC_F_UART_RX_FIFO_CTRL_FIFO_AF_LVL;
    CdcAcmUart->rx_fifo_ctrl |= (0 << MXC_F_UART_RX_FIFO_CTRL_FIFO_AF_LVL_POS);

    // Enable receive and transmit fifos
    CdcAcmUart->ctrl |= (MXC_F_UART_CTRL_RX_FIFO_EN | MXC_F_UART_CTRL_TX_FIFO_EN);

    NVIC_EnableIRQ(CdcAcmUartIrqNumber);

    // Set transmit almost empty level to three-quarters of the fifo size
    CdcAcmUart->tx_fifo_ctrl &= ~MXC_F_UART_TX_FIFO_CTRL_FIFO_AE_LVL;
    CdcAcmUart->tx_fifo_ctrl |= (MXC_UART_FIFO_DEPTH - (MXC_UART_FIFO_DEPTH >> 2)) << MXC_F_UART_TX_FIFO_CTRL_FIFO_AE_LVL_POS;

    // Enable RX and TX interrupts
    CdcAcmUart->inten = (MXC_F_UART_INTEN_RX_FIFO_NOT_EMPTY | MXC_F_UART_INTFL_RX_FIFO_OVERFLOW | MXC_F_UART_INTEN_TX_FIFO_AE);

    // Enable UART
    CdcAcmUart->ctrl |= MXC_F_UART_CTRL_UART_EN;

    return 1;
}

/******************************************************************************/
int32_t uart_uninitialize(void)
{
    // Disable UART
    CdcAcmUart->ctrl &= ~MXC_F_UART_CTRL_UART_EN;

    // Disable interrupts
    CdcAcmUart->inten = 0;
    NVIC_DisableIRQ(CdcAcmUartIrqNumber);

    // Clear buffers
    memset(&write_buffer, 0, sizeof(write_buffer));
    memset(&read_buffer, 0, sizeof(read_buffer));

    return 1;
}

/******************************************************************************/
void uart_set_control_line_state(uint16_t ctrl_bmp)
{
}

/******************************************************************************/
int32_t uart_reset(void)
{
    // Clear buffers
    memset(&write_buffer, 0, sizeof(write_buffer));
    memset(&read_buffer, 0, sizeof(read_buffer));

    return 1;
}

/******************************************************************************/
int32_t uart_set_configuration(UART_Configuration *config)
{
    uint32_t ctrl;

    // Get current configuration; clearing parameters that may be configured here
    ctrl = CdcAcmUart->ctrl & ~(MXC_F_UART_CTRL_PARITY |
                                MXC_F_UART_CTRL_DATA_SIZE |
                                MXC_F_UART_CTRL_EXTRA_STOP |
                                MXC_F_UART_CTRL_CTS_EN |
                                MXC_F_UART_CTRL_RTS_EN);

    switch (config->Parity) {
        case UART_PARITY_NONE: break;
        case UART_PARITY_ODD: ctrl |= MXC_S_UART_CTRL_PARITY_ODD;
        case UART_PARITY_EVEN: ctrl |= MXC_S_UART_CTRL_PARITY_EVEN;
        case UART_PARITY_MARK: return 0;
        case UART_PARITY_SPACE: return 0;
    }

    switch (config->DataBits) {
        case UART_DATA_BITS_5:  ctrl |= MXC_S_UART_CTRL_DATA_SIZE_5_BITS; break;
        case UART_DATA_BITS_6:  ctrl |= MXC_S_UART_CTRL_DATA_SIZE_6_BITS; break;
        case UART_DATA_BITS_7:  ctrl |= MXC_S_UART_CTRL_DATA_SIZE_7_BITS; break;
        case UART_DATA_BITS_8:  ctrl |= MXC_S_UART_CTRL_DATA_SIZE_8_BITS; break;
        case UART_DATA_BITS_16: return 0;
    }

    switch (config->StopBits) {
        case UART_STOP_BITS_1:    break;
        case UART_STOP_BITS_1_5:
        case UART_STOP_BITS_2:    ctrl |= MXC_F_UART_CTRL_EXTRA_STOP; break;
    }

    switch (config->FlowControl) {
        case UART_FLOW_CONTROL_NONE:      break;
        case UART_FLOW_CONTROL_RTS_CTS:   return 0;
        case UART_FLOW_CONTROL_XON_XOFF:  return 0;
    }

    set_bitrate(config->Baudrate);

    // Set the new configuration
    CdcAcmUart->ctrl = ctrl;

    return 1;
}

/******************************************************************************/
int32_t uart_get_configuration(UART_Configuration *config)
{
    uint32_t ctrl;

    // Capture current configuration
    ctrl = CdcAcmUart->ctrl;

    if (!(ctrl & MXC_S_UART_CTRL_PARITY_DISABLE)) {
        config->Parity = UART_PARITY_NONE;
    } else if (ctrl & MXC_S_UART_CTRL_PARITY_ODD) {
        config->Parity = UART_PARITY_ODD;
    } else {
        // Note both EVEN and MARK parity are captured here
        config->Parity = UART_PARITY_EVEN;
    }

    switch (ctrl & MXC_F_UART_CTRL_DATA_SIZE) {
        case MXC_S_UART_CTRL_DATA_SIZE_5_BITS: config->DataBits = UART_DATA_BITS_5; break;
        case MXC_S_UART_CTRL_DATA_SIZE_6_BITS: config->DataBits = UART_DATA_BITS_6; break;
        case MXC_S_UART_CTRL_DATA_SIZE_7_BITS: config->DataBits = UART_DATA_BITS_7; break;
        case MXC_S_UART_CTRL_DATA_SIZE_8_BITS: config->DataBits = UART_DATA_BITS_8; break;
    }

    if (!(ctrl & MXC_F_UART_CTRL_EXTRA_STOP)) {
        config->StopBits = UART_STOP_BITS_1;
    } else {
        config->StopBits = UART_STOP_BITS_2;
    }

    if ((ctrl & (MXC_F_UART_CTRL_CTS_EN | MXC_F_UART_CTRL_RTS_EN)) == (MXC_F_UART_CTRL_CTS_EN | MXC_F_UART_CTRL_RTS_EN)) {
        config->FlowControl = UART_FLOW_CONTROL_RTS_CTS;
    } else {
        // Not true if only one of ...CST_EN and ...RTS_EN are asserted
        config->FlowControl = UART_FLOW_CONTROL_NONE;
    }

    config->Baudrate = baudrate;

    return 1;
}

/******************************************************************************/
int32_t uart_write_free(void)
{
    return BUFFER_SIZE - (write_buffer.cnt_in - write_buffer.cnt_out);
}

/******************************************************************************/
int32_t uart_write_data(uint8_t *data, uint16_t size)
{
    uint16_t xfer_count = size;

    if (write_buffer.cnt_in == write_buffer.cnt_out) {
        while ((((CdcAcmUart->tx_fifo_ctrl & MXC_F_UART_TX_FIFO_CTRL_FIFO_ENTRY) >> MXC_F_UART_TX_FIFO_CTRL_FIFO_ENTRY_POS) < MXC_UART_FIFO_DEPTH) &&
                (xfer_count > 0)) {

            NVIC_DisableIRQ(CdcAcmUartIrqNumber);
            CdcAcmUart->intfl = MXC_F_UART_INTFL_TX_FIFO_AE;
            CdcAcmUartFifo->tx = *data++;
            xfer_count--;
            NVIC_EnableIRQ(CdcAcmUartIrqNumber);

        }
    }

    while (xfer_count > 0) {
        if ((write_buffer.cnt_in - write_buffer.cnt_out) < BUFFER_SIZE) {

            NVIC_DisableIRQ(CdcAcmUartIrqNumber);
            write_buffer.data[write_buffer.idx_in++] = *data++;
            write_buffer.idx_in &= (BUFFER_SIZE - 1);
            write_buffer.cnt_in++;
            xfer_count--;
            NVIC_EnableIRQ(CdcAcmUartIrqNumber);

        } else {
            break;
        }
    }
    return size - xfer_count;
}

/******************************************************************************/
int32_t uart_read_data(uint8_t *data, uint16_t size)
{
    int32_t cnt;

    for (cnt = 0; (cnt < size) && (read_buffer.cnt_in != read_buffer.cnt_out); cnt++) {
        *data++ = read_buffer.data[read_buffer.idx_out++];
        read_buffer.idx_out &= (BUFFER_SIZE - 1);
        read_buffer.cnt_out++;
    }

    return cnt;
}

/******************************************************************************/
void UART_IRQHandler(void)
{
    // Capture interrupt flag state at entry
    uint32_t intfl = CdcAcmUart->intfl;
    // Clear interrupts that will be serviced
    CdcAcmUart->intfl = intfl;

    if (intfl & MXC_F_UART_INTFL_RX_FIFO_OVERFLOW) {
        read_buffer.data[read_buffer.idx_in++] = '*';
        read_buffer.idx_in &= (BUFFER_SIZE - 1);
        read_buffer.cnt_in++;
    }

    if (intfl & (MXC_F_UART_INTFL_RX_FIFO_NOT_EMPTY | UART_ERRORS)) {
        while ((CdcAcmUart->rx_fifo_ctrl & MXC_F_UART_RX_FIFO_CTRL_FIFO_ENTRY) &&
                ((read_buffer.cnt_in - read_buffer.cnt_out) < BUFFER_SIZE)) {
            read_buffer.data[read_buffer.idx_in++] = CdcAcmUartFifo->rx;
            CdcAcmUart->intfl = MXC_F_UART_INTFL_RX_FIFO_NOT_EMPTY;
            read_buffer.idx_in &= (BUFFER_SIZE - 1);
            read_buffer.cnt_in++;
        }
        if (((read_buffer.cnt_in - read_buffer.cnt_out) >= BUFFER_SIZE)) {
            read_buffer.data[read_buffer.idx_in++] = '%';
            read_buffer.idx_in &= (BUFFER_SIZE - 1);
            read_buffer.cnt_in++;
        }
    }

    if (intfl & MXC_F_UART_INTFL_TX_FIFO_AE) {
        /*
        	Transfer data from write buffer to transmit FIFO if
        	a) write buffer contains data and
        	b) transmit FIFO is not full
        */
        while ((write_buffer.cnt_out != write_buffer.cnt_in) &&
                (((CdcAcmUart->tx_fifo_ctrl & MXC_F_UART_TX_FIFO_CTRL_FIFO_ENTRY) >> MXC_F_UART_TX_FIFO_CTRL_FIFO_ENTRY_POS) < MXC_UART_FIFO_DEPTH)) {
            CdcAcmUartFifo->tx = write_buffer.data[write_buffer.idx_out++];
            write_buffer.idx_out &= (BUFFER_SIZE - 1);
            write_buffer.cnt_out++;
        }
    }
}

/******************************************************************************/
void UART0_IRQHandler(void)
{
    UART_IRQHandler();
}

/******************************************************************************/
void UART1_IRQHandler(void)
{
    UART_IRQHandler();
}

/******************************************************************************/
void UART2_IRQHandler(void)
{
    UART_IRQHandler();
}
