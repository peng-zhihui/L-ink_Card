/**
 * @file    musca_a.c
 * @brief   board ID for the ARM MUSCA A board
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

#include "stdbool.h"
#include "virtual_fs.h"
#include "flash_manager.h"
#include "IO_Config.h"
#include "target_board.h"
#include "target_family.h"

void power_on_sequence(void);

uint8_t gpio_get_pbon_btn()
{
    return LPC_GPIO->PIN[PIN_PBON_PORT] & PIN_PBON ? 0 : 1;
}

static void prerun_board_config(void)
{
    /* configure MUSCA specific gpio */
    // configure PBON as input
    PIN_PBON_IOCON = PIN_PBON_IOCON_INIT;
    LPC_GPIO->DIR[PIN_PBON_PORT] &= ~PIN_PBON;
    // configure PSUON as output
    PIN_PSUON_IOCON = PIN_PSUON_IOCON_INIT;
    LPC_GPIO->CLR[PIN_PSUON_PORT] = PIN_PSUON;
    LPC_GPIO->DIR[PIN_PSUON_PORT] |= PIN_PSUON;
    // configure SHUTDOWN as input
    PIN_SHUTDOWN_IOCON = PIN_SHUTDOWN_IOCON_INIT;
    LPC_GPIO->DIR[PIN_SHUTDOWN_PORT] &= ~PIN_SHUTDOWN;
    // configure CB_nRST as output
    PIN_CB_nRST_IOCON = PIN_CB_nRST_IOCON_INIT;
    LPC_GPIO->CLR[PIN_CB_nRST_PORT] = PIN_CB_nRST;
    LPC_GPIO->DIR[PIN_CB_nRST_PORT] |= PIN_CB_nRST;
    // configure CFG_nRST as output
    PIN_CFG_nRST_IOCON = PIN_CFG_nRST_IOCON_INIT;
    LPC_GPIO->CLR[PIN_CFG_nRST_PORT] = PIN_CFG_nRST;
    LPC_GPIO->DIR[PIN_CFG_nRST_PORT] |= PIN_CFG_nRST;
    // configure SCC_LOAD as output
    PIN_SCC_LOAD_IOCON = PIN_SCC_LOAD_IOCON_INIT;
    LPC_GPIO->CLR[PIN_SCC_LOAD_PORT] = PIN_SCC_LOAD;
    LPC_GPIO->DIR[PIN_SCC_LOAD_PORT] |= PIN_SCC_LOAD;
    // configure SCC_WNR as output
    PIN_SCC_WNR_IOCON = PIN_SCC_WNR_IOCON_INIT;
    LPC_GPIO->CLR[PIN_SCC_WNR_PORT] = PIN_SCC_WNR;
    LPC_GPIO->DIR[PIN_SCC_WNR_PORT] |= PIN_SCC_WNR;
    // configure SCC_DATAIN as output
    PIN_SCC_DATAIN_IOCON = PIN_SCC_DATAIN_IOCON_INIT;
    LPC_GPIO->CLR[PIN_SCC_DATAIN_PORT] = PIN_SCC_DATAIN;
    LPC_GPIO->DIR[PIN_SCC_DATAIN_PORT] |= PIN_SCC_DATAIN;
    // configure SCC_CLK as output
    PIN_SCC_CLK_IOCON = PIN_SCC_CLK_IOCON_INIT;
    LPC_GPIO->CLR[PIN_SCC_CLK_PORT] = PIN_SCC_CLK;
    LPC_GPIO->DIR[PIN_SCC_CLK_PORT] |= PIN_SCC_CLK;
    // configure SCC_DATAOUT as input
    PIN_SCC_DATAOUT_IOCON = PIN_SCC_DATAOUT_IOCON_INIT;
    LPC_GPIO->DIR[PIN_SCC_DATAOUT_PORT] &= ~PIN_SCC_DATAOUT;
    // configure CS_nDET as input
    PIN_CS_nDET_IOCON = PIN_CS_nDET_IOCON_INIT;
    LPC_GPIO->DIR[PIN_CS_nDET_PORT] &= ~PIN_CS_nDET;

    PIN_USART_IOCON = PIN_USART_IOCON_INIT;

    // Switch on power supply - PSUON
    LPC_GPIO->SET[PIN_PSUON_PORT] = PIN_PSUON;

    power_on_sequence();

    flash_manager_set_page_erase(true);
}

const board_info_t g_board_info = {
    .board_id = "5006",
    .daplink_drive_name =     "MBED       ",
    .target_cfg = &target_device,
    .prerun_board_config = prerun_board_config,
};
