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

#include "max32625.h"
#include "clkman_regs.h"
#include "gpio_regs.h"
#include "IO_Config.h"
#include "gpio.h"
#include "adc_regs.h"
#include "pwrman_regs.h"
#include "ioman_regs.h"

// For channels 4 and 5; the target VIO inputs
// 1.6V is the lowest VIO can operate at,
// so set this as minimum acceptable voltage.
// round(AIN/(5 * 1.2)*(2^10-1))
#define VIO_ADC_MIN 273

// Bitband pointers
volatile uint32_t *tck_in;
volatile uint32_t *tck_out;
volatile uint32_t *tms_in;
volatile uint32_t *tms_out;
volatile uint32_t *rst_in;
volatile uint32_t *rst_out;

uint32_t swdio_port;
uint32_t swdio_pin;
uint32_t swclk_port;
uint32_t swclk_pin;
uint32_t nreset_port;
uint32_t nreset_pin;

int32_t uart_set_instance(uint32_t inst);

/******************************************************************************/
static inline void use_vddioh(int port, int pin)
{
    if (port > 3) {
        MXC_IOMAN->use_vddioh_1 |= 1U << (((port - 4) * 8) + pin);
    } else {
        MXC_IOMAN->use_vddioh_0 |= 1U << ((port * 8) + pin);
    }
}

/******************************************************************************/
static uint16_t readADC(uint8_t ch)
{
    uint32_t ctrl_tmp;

    // Select channel
    ctrl_tmp = MXC_ADC->ctrl;
    ctrl_tmp &= ~MXC_F_ADC_CTRL_ADC_CHSEL;
    ctrl_tmp |= ((ch << MXC_F_ADC_CTRL_ADC_CHSEL_POS) & MXC_F_ADC_CTRL_ADC_CHSEL);

    // Clear channel configuration
    ctrl_tmp &= ~(MXC_F_ADC_CTRL_ADC_REFSCL | MXC_F_ADC_CTRL_ADC_SCALE | MXC_F_ADC_CTRL_BUF_BYPASS);

    // ADC reference scaling
    ctrl_tmp |= MXC_F_ADC_CTRL_ADC_REFSCL;

    if ((ch == SWD_VIO_CH) || (ch == HDR_VIO_CH)) {
        ctrl_tmp |= MXC_F_ADC_CTRL_ADC_SCALE;
    }

    // Do not bypass buffer, except when measuring near ground.
    // This may be the case for measuring 1-Wire ground detect on channels 2 and 3.
    if ((ch == SWD_GNDDET_CH) || (ch == HDR_GNDDET_CH)) {
        ctrl_tmp |= MXC_F_ADC_CTRL_BUF_BYPASS;
    }

    // Write this configuration
    MXC_ADC->ctrl = ctrl_tmp;

    // Clear conversion done interrupt flag
    MXC_ADC->intr = MXC_F_ADC_INTR_ADC_DONE_IF;

    // Start conversion
    MXC_ADC->ctrl |= MXC_F_ADC_CTRL_CPU_ADC_START;

    while (!(MXC_ADC->intr & MXC_F_ADC_INTR_ADC_DONE_IF));

    return (uint16_t)(MXC_ADC->data);
}

/******************************************************************************/
void target_set_interface(TARGET_INTERFACE mode)
{
    switch (mode) {
        case IO_SWD_EXT:
            swdio_port = PIN_SWDIO_PORT;
            swdio_pin = PIN_SWDIO_PIN;
            swclk_port = PIN_SWCLK_PORT;
            swclk_pin = PIN_SWCLK_PIN;
            nreset_port = PIN_nRESET_PORT;
            nreset_pin = PIN_nRESET_PIN;

            MXC_CLRBIT(&MXC_GPIO->out_val[EN_VDDIOH_PORT], EN_VDDIOH_PIN);      // Low to disable SWOUT
            MXC_SETBIT(&MXC_GPIO->out_val[IOH_OW_EN_PORT], IOH_OW_EN_PIN);      // High to power MAX14689
            MXC_SETBIT(&MXC_GPIO->out_val[SWD_DIP_SEL_PORT], SWD_DIP_SEL_PIN);  // High to connect Bn <-> Cn
            uart_set_instance(CDC_ACM_UART_SWD);
            break;
        case IO_DIP_EXT:
            swdio_port = PIN_DIP_SWDIO_PORT;
            swdio_pin = PIN_DIP_SWDIO_PIN;
            swclk_port = PIN_DIP_SWCLK_PORT;
            swclk_pin = PIN_DIP_SWCLK_PIN;
            nreset_port = PIN_DIP_nRESET_PORT;
            nreset_pin = PIN_DIP_nRESET_PIN;

            MXC_CLRBIT(&MXC_GPIO->out_val[EN_VDDIOH_PORT], EN_VDDIOH_PIN);      // Low to disable SWOUT
            MXC_SETBIT(&MXC_GPIO->out_val[IOH_OW_EN_PORT], IOH_OW_EN_PIN);      // High to power MAX14689
            MXC_CLRBIT(&MXC_GPIO->out_val[SWD_DIP_SEL_PORT], SWD_DIP_SEL_PIN);  // Low to connect Bn <-> An
            uart_set_instance(CDC_ACM_UART_DIP);
            break;
    }
}

/******************************************************************************/
void gpio_init(void)
{
    int i;
    uint32_t out_mode;

    // Ensure that the GPIO clock is enabled
    if (MXC_CLKMAN->sys_clk_ctrl_6_gpio == MXC_S_CLKMAN_CLK_SCALE_DISABLED) {
        MXC_CLKMAN->sys_clk_ctrl_6_gpio = MXC_S_CLKMAN_CLK_SCALE_DIV_1;
    }

    // Make all GPIO pins readable
    for (i = 0; i < MXC_GPIO_NUM_PORTS; i++) {
        MXC_GPIO->in_mode[i] = 0x00000000;
    }

    // Set to enable the board to power VDDIOH and in turn the target micro
    // Clear to disable the board from powering VDDIOH
    MXC_CLRBIT(&MXC_GPIO->out_val[EN_VDDIOH_PORT], EN_VDDIOH_PIN);
    out_mode = MXC_GPIO->out_mode[EN_VDDIOH_PORT];
    out_mode &= ~(0xFU << (4 * EN_VDDIOH_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_NORMAL << (4 * EN_VDDIOH_PIN));
    MXC_GPIO->out_mode[EN_VDDIOH_PORT] = out_mode;

    // LED initial state off
    MXC_GPIO->out_val[PIN_DAP_LED_PORT] |= (1 << PIN_DAP_LED_PIN);
    MXC_GPIO->out_val[PIN_MSD_LED_PORT] |= (1 << PIN_MSD_LED_PIN);
    MXC_GPIO->out_val[PIN_CDC_LED_PORT] |= (1 << PIN_CDC_LED_PIN);

    // LED outputs
    out_mode = MXC_GPIO->out_mode[PIN_DAP_LED_PORT];
    out_mode &= ~(0xFU << (4 * PIN_DAP_LED_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_OPEN_DRAIN << (4 * PIN_DAP_LED_PIN));
    MXC_GPIO->out_mode[PIN_DAP_LED_PORT] = out_mode;

    out_mode = MXC_GPIO->out_mode[PIN_MSD_LED_PORT];
    out_mode &= ~(0xFU << (4 * PIN_MSD_LED_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_OPEN_DRAIN << (4 * PIN_MSD_LED_PIN));
    MXC_GPIO->out_mode[PIN_MSD_LED_PORT] = out_mode;

    out_mode = MXC_GPIO->out_mode[PIN_CDC_LED_PORT];
    out_mode &= ~(0xFU << (4 * PIN_CDC_LED_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_OPEN_DRAIN << (4 * PIN_CDC_LED_PIN));
    MXC_GPIO->out_mode[PIN_CDC_LED_PORT] = out_mode;

    // Button Input
    out_mode = MXC_GPIO->out_mode[PIN_RESET_IN_NO_FWRD_PORT];
    out_mode &= ~(0xFU << (4 * PIN_RESET_IN_NO_FWRD_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_OPEN_DRAIN_WEAK_PULLUP << (4 * PIN_RESET_IN_NO_FWRD_PIN));
    MXC_GPIO->out_mode[PIN_RESET_IN_NO_FWRD_PORT] = out_mode;
    MXC_GPIO->out_val[PIN_RESET_IN_NO_FWRD_PORT] |= (0x1U << PIN_RESET_IN_NO_FWRD_PIN);

    // IOH_1W_EN (must be configured for strong drive)
    MXC_SETBIT(&MXC_GPIO->out_val[IOH_OW_EN_PORT], IOH_OW_EN_PIN);
    out_mode = MXC_GPIO->out_mode[IOH_OW_EN_PORT];
    out_mode &= ~(0xFU << (4 * IOH_OW_EN_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_FAST_DRIVE << (4 * IOH_OW_EN_PIN));
    MXC_GPIO->out_mode[IOH_OW_EN_PORT] = out_mode;

    // SWD_DIP_SEL (must be configured for strong drive)
    MXC_CLRBIT(&MXC_GPIO->out_val[SWD_DIP_SEL_PORT], SWD_DIP_SEL_PIN);
    out_mode = MXC_GPIO->out_mode[SWD_DIP_SEL_PORT];
    out_mode &= ~(0xFU << (4 * SWD_DIP_SEL_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_FAST_DRIVE << (4 * SWD_DIP_SEL_PIN));
    MXC_GPIO->out_mode[SWD_DIP_SEL_PORT] = out_mode;

    // Strong pull-up disable
    MXC_SETBIT(&MXC_GPIO->out_val[OWM_SUP_PORT], OWM_SUP_PIN);
    out_mode = MXC_GPIO->out_mode[OWM_SUP_PORT];
    out_mode &= ~(0xFU << (4 * OWM_SUP_PIN));
    out_mode |= (MXC_V_GPIO_OUT_MODE_NORMAL << (4 * OWM_SUP_PIN));
    MXC_GPIO->out_mode[OWM_SUP_PORT] = out_mode;

    // VDDIOH driver isn't strong enough with the strong pull-up
    MXC_IOMAN->use_vddioh_1 &= ~(1U << (((OWM_PORT - 4) * 8) + OWM_PIN));
    // 1V8 VDDIO is not high enough to turn off the FET if VDDIOH is 3V3
    MXC_IOMAN->use_vddioh_1 |= (1U << (((OWM_PORT - 4) * 8) + OWM_SUP_PIN));

    use_vddioh(PIN_nRESET_PORT, PIN_nRESET_PIN);
    use_vddioh(PIN_DIP_nRESET_PORT, PIN_DIP_nRESET_PIN);
    use_vddioh(PIN_SWCLK_PORT, PIN_SWCLK_PIN);
    use_vddioh(PIN_DIP_SWCLK_PORT, PIN_DIP_SWCLK_PIN);
    use_vddioh(PIN_SWDIO_PORT, PIN_SWDIO_PIN);
    use_vddioh(PIN_DIP_SWDIO_PORT, PIN_DIP_SWDIO_PIN);
    use_vddioh(PIN_TX_PORT, PIN_TX_PIN);
    use_vddioh(PIN_RX_PORT, PIN_RX_PIN);
    use_vddioh(PIN_DIP_TX_PORT, PIN_DIP_TX_PIN);
    use_vddioh(PIN_DIP_RX_PORT, PIN_DIP_RX_PIN);

    // Setup the ADC; read the ADC to set IO interface
    MXC_PWRMAN->pwr_rst_ctrl |= MXC_F_PWRMAN_PWR_RST_CTRL_AFE_POWERED;
    MXC_CLKMAN->clk_ctrl |= MXC_F_CLKMAN_CLK_CTRL_ADC_CLOCK_ENABLE;

    MXC_ADC->ctrl = (MXC_F_ADC_CTRL_ADC_PU |
                     MXC_F_ADC_CTRL_ADC_CLK_EN |
                     MXC_F_ADC_CTRL_BUF_PU |
                     MXC_F_ADC_CTRL_ADC_REFBUF_PU |
                     MXC_F_ADC_CTRL_ADC_CHGPUMP_PU);

    // Set IO interface
    if (readADC(SWD_VIO_CH) > VIO_ADC_MIN) {
        target_set_interface(IO_SWD_EXT);
    } else if (readADC(HDR_VIO_CH) > VIO_ADC_MIN) {
        target_set_interface(IO_DIP_EXT);
    } else {
        // Default to SWD interface
        target_set_interface(IO_SWD_EXT);
    }

}

/******************************************************************************/
void gpio_set_hid_led(gpio_led_state_t state)
{
    if (state == GPIO_LED_ON) {
        MXC_CLRBIT(&MXC_GPIO->out_val[PIN_DAP_LED_PORT], PIN_DAP_LED_PIN);
    } else {
        MXC_SETBIT(&MXC_GPIO->out_val[PIN_DAP_LED_PORT], PIN_DAP_LED_PIN);
    }
}

/******************************************************************************/
void gpio_set_msc_led(gpio_led_state_t state)
{
    if (state == GPIO_LED_ON) {
        MXC_CLRBIT(&MXC_GPIO->out_val[PIN_MSD_LED_PORT], PIN_MSD_LED_PIN);
    } else {
        MXC_SETBIT(&MXC_GPIO->out_val[PIN_MSD_LED_PORT], PIN_MSD_LED_PIN);
    }
}

/******************************************************************************/
void gpio_set_cdc_led(gpio_led_state_t state)
{
    if (state == GPIO_LED_ON) {
        MXC_CLRBIT(&MXC_GPIO->out_val[PIN_CDC_LED_PORT], PIN_CDC_LED_PIN);
    } else {
        MXC_SETBIT(&MXC_GPIO->out_val[PIN_CDC_LED_PORT], PIN_CDC_LED_PIN);
    }
}

/******************************************************************************/
uint8_t gpio_get_reset_btn_no_fwrd(void)
{
    return !MXC_GETBIT(&MXC_GPIO->in_val[PIN_RESET_IN_NO_FWRD_PORT], PIN_RESET_IN_NO_FWRD_PIN);
}

/******************************************************************************/
uint8_t gpio_get_reset_btn_fwrd(void)
{
    return 0;
}

/******************************************************************************/
void gpio_set_board_power(bool powerEnabled)
{
}
