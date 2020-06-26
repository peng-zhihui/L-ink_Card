/**
 * @file    swd_host.c
 * @brief   Implementation of swd_host.h
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
 * Copyright 2019, Cypress Semiconductor Corporation 
 * or a subsidiary of Cypress Semiconductor Corporation.
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

#ifndef TARGET_MCU_CORTEX_A
#include "cmsis_os2.h"
#include "target_config.h"
#include "swd_host.h"
#include "debug_cm.h"
#include "DAP_config.h"
#include "DAP.h"
#include "target_family.h"
#include "device.h"

// Default NVIC and Core debug base addresses
// TODO: Read these addresses from ROM.
#define NVIC_Addr    (0xe000e000)
#define DBG_Addr     (0xe000edf0)

// AP CSW register, base value
#define CSW_VALUE (CSW_RESERVED | CSW_MSTRDBG | CSW_HPROT | CSW_DBGSTAT | CSW_SADDRINC)

#define DCRDR 0xE000EDF8
#define DCRSR 0xE000EDF4
#define DHCSR 0xE000EDF0
#define REGWnR (1 << 16)

#define MAX_SWD_RETRY 100//10
#define MAX_TIMEOUT   1000000  // Timeout for syscalls on target

// Use the CMSIS-Core definition if available.
#if !defined(SCB_AIRCR_PRIGROUP_Pos)
#define SCB_AIRCR_PRIGROUP_Pos              8U                                            /*!< SCB AIRCR: PRIGROUP Position */
#define SCB_AIRCR_PRIGROUP_Msk             (7UL << SCB_AIRCR_PRIGROUP_Pos)                /*!< SCB AIRCR: PRIGROUP Mask */
#endif

typedef struct {
    uint32_t select;
    uint32_t csw;
} DAP_STATE;

typedef struct {
    uint32_t r[16];
    uint32_t xpsr;
} DEBUG_STATE;

static SWD_CONNECT_TYPE reset_connect = CONNECT_NORMAL;

static DAP_STATE dap_state;
static uint32_t  soft_reset = SYSRESETREQ;

static uint32_t swd_get_apsel(uint32_t adr)
{
    uint32_t apsel = target_get_apsel();
    if (!apsel)
        return adr & 0xff000000;
    else
        return apsel;
}

void swd_set_reset_connect(SWD_CONNECT_TYPE type)
{
    reset_connect = type;
}

void int2array(uint8_t *res, uint32_t data, uint8_t len)
{
    uint8_t i = 0;

    for (i = 0; i < len; i++) {
        res[i] = (data >> 8 * i) & 0xff;
    }
}

uint8_t swd_transfer_retry(uint32_t req, uint32_t *data)
{
    uint8_t i, ack;

    for (i = 0; i < MAX_SWD_RETRY; i++) {
        ack = SWD_Transfer(req, data);

        // if ack != WAIT
        if (ack != DAP_TRANSFER_WAIT) {
            return ack;
        }
    }

    return ack;
}

void swd_set_soft_reset(uint32_t soft_reset_type)
{
    soft_reset = soft_reset_type;
}

uint8_t swd_init(void)
{
    //TODO - DAP_Setup puts GPIO pins in a hi-z state which can
    //       cause problems on re-init.  This needs to be investigated
    //       and fixed.
    DAP_Setup();
    PORT_SWD_SETUP();
    return 1;
}

uint8_t swd_off(void)
{
    PORT_OFF();
    return 1;
}

uint8_t swd_clear_errors(void)
{
    if (!swd_write_dp(DP_ABORT, STKCMPCLR | STKERRCLR | WDERRCLR | ORUNERRCLR)) {
        return 0;
    }
    return 1;
}

// Read debug port register.
uint8_t swd_read_dp(uint8_t adr, uint32_t *val)
{
    uint32_t tmp_in;
    uint8_t tmp_out[4];
    uint8_t ack;
    uint32_t tmp;
    tmp_in = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(adr);
    ack = swd_transfer_retry(tmp_in, (uint32_t *)tmp_out);
    *val = 0;
    tmp = tmp_out[3];
    *val |= (tmp << 24);
    tmp = tmp_out[2];
    *val |= (tmp << 16);
    tmp = tmp_out[1];
    *val |= (tmp << 8);
    tmp = tmp_out[0];
    *val |= (tmp << 0);
    return (ack == 0x01);
}

// Write debug port register
uint8_t swd_write_dp(uint8_t adr, uint32_t val)
{
    uint32_t req;
    uint8_t data[4];
    uint8_t ack;

    //check if the right bank is already selected
    if ((adr == DP_SELECT) && (dap_state.select == val)) {
        return 1;
    }

    req = SWD_REG_DP | SWD_REG_W | SWD_REG_ADR(adr);
    int2array(data, val, 4);
    ack = swd_transfer_retry(req, (uint32_t *)data);
    if ((ack == DAP_TRANSFER_OK) && (adr == DP_SELECT)) {
        dap_state.select = val;
    }
    return (ack == 0x01);
}

// Read access port register.
uint8_t swd_read_ap(uint32_t adr, uint32_t *val)
{
    uint8_t tmp_in, ack;
    uint8_t tmp_out[4];
    uint32_t tmp;
    uint32_t apsel = swd_get_apsel(adr);
    uint32_t bank_sel = adr & APBANKSEL;

    if (!swd_write_dp(DP_SELECT, apsel | bank_sel)) {
        return 0;
    }

    tmp_in = SWD_REG_AP | SWD_REG_R | SWD_REG_ADR(adr);
    // first dummy read
    swd_transfer_retry(tmp_in, (uint32_t *)tmp_out);
    ack = swd_transfer_retry(tmp_in, (uint32_t *)tmp_out);
    *val = 0;
    tmp = tmp_out[3];
    *val |= (tmp << 24);
    tmp = tmp_out[2];
    *val |= (tmp << 16);
    tmp = tmp_out[1];
    *val |= (tmp << 8);
    tmp = tmp_out[0];
    *val |= (tmp << 0);
    return (ack == 0x01);
}

// Write access port register
uint8_t swd_write_ap(uint32_t adr, uint32_t val)
{
    uint8_t data[4];
    uint8_t req, ack;
    uint32_t apsel = swd_get_apsel(adr);
    uint32_t bank_sel = adr & APBANKSEL;

    if (!swd_write_dp(DP_SELECT, apsel | bank_sel)) {
        return 0;
    }

    switch (adr) {
        case AP_CSW:
            if (dap_state.csw == val) {
                return 1;
            }

            dap_state.csw = val;
            break;

        default:
            break;
    }

    req = SWD_REG_AP | SWD_REG_W | SWD_REG_ADR(adr);
    int2array(data, val, 4);

    if (swd_transfer_retry(req, (uint32_t *)data) != 0x01) {
        return 0;
    }

    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = swd_transfer_retry(req, NULL);
    return (ack == 0x01);
}


// Write 32-bit word aligned values to target memory using address auto-increment.
// size is in bytes.
static uint8_t swd_write_block(uint32_t address, uint8_t *data, uint32_t size)
{
    uint8_t tmp_in[4], req;
    uint32_t size_in_words;
    uint32_t i, ack;

    if (size == 0) {
        return 0;
    }

    size_in_words = size / 4;

    // CSW register
    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    // TAR write
    req = SWD_REG_AP | SWD_REG_W | (1 << 2);
    int2array(tmp_in, address, 4);

    if (swd_transfer_retry(req, (uint32_t *)tmp_in) != 0x01) {
        return 0;
    }

    // DRW write
    req = SWD_REG_AP | SWD_REG_W | (3 << 2);

    for (i = 0; i < size_in_words; i++) {
        if (swd_transfer_retry(req, (uint32_t *)data) != 0x01) {
            return 0;
        }

        data += 4;
    }

    // dummy read
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = swd_transfer_retry(req, NULL);
    return (ack == 0x01);
}

// Read 32-bit word aligned values from target memory using address auto-increment.
// size is in bytes.
static uint8_t swd_read_block(uint32_t address, uint8_t *data, uint32_t size)
{
    uint8_t tmp_in[4], req, ack;
    uint32_t size_in_words;
    uint32_t i;

    if (size == 0) {
        return 0;
    }

    size_in_words = size / 4;

    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    // TAR write
    req = SWD_REG_AP | SWD_REG_W | AP_TAR;
    int2array(tmp_in, address, 4);

    if (swd_transfer_retry(req, (uint32_t *)tmp_in) != DAP_TRANSFER_OK) {
        return 0;
    }

    // read data
    req = SWD_REG_AP | SWD_REG_R | AP_DRW;

    // initiate first read, data comes back in next read
    if (swd_transfer_retry(req, NULL) != 0x01) {
        return 0;
    }

    for (i = 0; i < (size_in_words - 1); i++) {
        if (swd_transfer_retry(req, (uint32_t *)data) != DAP_TRANSFER_OK) {
            return 0;
        }

        data += 4;
    }

    // read last word
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = swd_transfer_retry(req, (uint32_t *)data);
    return (ack == 0x01);
}

// Read target memory.
static uint8_t swd_read_data(uint32_t addr, uint32_t *val)
{
    uint8_t tmp_in[4];
    uint8_t tmp_out[4];
    uint8_t req, ack;
    uint32_t tmp;
    // put addr in TAR register
    int2array(tmp_in, addr, 4);
    req = SWD_REG_AP | SWD_REG_W | (1 << 2);

    if (swd_transfer_retry(req, (uint32_t *)tmp_in) != 0x01) {
        return 0;
    }

    // read data
    req = SWD_REG_AP | SWD_REG_R | (3 << 2);

    if (swd_transfer_retry(req, (uint32_t *)tmp_out) != 0x01) {
        return 0;
    }

    // dummy read
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = swd_transfer_retry(req, (uint32_t *)tmp_out);
    *val = 0;
    tmp = tmp_out[3];
    *val |= (tmp << 24);
    tmp = tmp_out[2];
    *val |= (tmp << 16);
    tmp = tmp_out[1];
    *val |= (tmp << 8);
    tmp = tmp_out[0];
    *val |= (tmp << 0);
    return (ack == 0x01);
}

// Write target memory.
static uint8_t swd_write_data(uint32_t address, uint32_t data)
{
    uint8_t tmp_in[4];
    uint8_t req, ack;
    // put addr in TAR register
    int2array(tmp_in, address, 4);
    req = SWD_REG_AP | SWD_REG_W | (1 << 2);

    if (swd_transfer_retry(req, (uint32_t *)tmp_in) != 0x01) {
        return 0;
    }

    // write data
    int2array(tmp_in, data, 4);
    req = SWD_REG_AP | SWD_REG_W | (3 << 2);

    if (swd_transfer_retry(req, (uint32_t *)tmp_in) != 0x01) {
        return 0;
    }

    // dummy read
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = swd_transfer_retry(req, NULL);
    return (ack == 0x01) ? 1 : 0;
}

// Read 32-bit word from target memory.
uint8_t swd_read_word(uint32_t addr, uint32_t *val)
{
    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    if (!swd_read_data(addr, val)) {
        return 0;
    }

    return 1;
}

// Write 32-bit word to target memory.
uint8_t swd_write_word(uint32_t addr, uint32_t val)
{
    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    if (!swd_write_data(addr, val)) {
        return 0;
    }

    return 1;
}

// Read 8-bit byte from target memory.
uint8_t swd_read_byte(uint32_t addr, uint8_t *val)
{
    uint32_t tmp;

    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE8)) {
        return 0;
    }

    if (!swd_read_data(addr, &tmp)) {
        return 0;
    }

    *val = (uint8_t)(tmp >> ((addr & 0x03) << 3));
    return 1;
}

// Write 8-bit byte to target memory.
uint8_t swd_write_byte(uint32_t addr, uint8_t val)
{
    uint32_t tmp;

    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE8)) {
        return 0;
    }

    tmp = val << ((addr & 0x03) << 3);

    if (!swd_write_data(addr, tmp)) {
        return 0;
    }

    return 1;
}

// Read unaligned data from target memory.
// size is in bytes.
uint8_t swd_read_memory(uint32_t address, uint8_t *data, uint32_t size)
{
    uint32_t n;

    // Read bytes until word aligned
    while ((size > 0) && (address & 0x3)) {
        if (!swd_read_byte(address, data)) {
            return 0;
        }

        address++;
        data++;
        size--;
    }

    // Read word aligned blocks
    while (size > 3) {
        // Limit to auto increment page size
        n = TARGET_AUTO_INCREMENT_PAGE_SIZE - (address & (TARGET_AUTO_INCREMENT_PAGE_SIZE - 1));

        if (size < n) {
            n = size & 0xFFFFFFFC; // Only count complete words remaining
        }

        if (!swd_read_block(address, data, n)) {
            return 0;
        }

        address += n;
        data += n;
        size -= n;
    }

    // Read remaining bytes
    while (size > 0) {
        if (!swd_read_byte(address, data)) {
            return 0;
        }

        address++;
        data++;
        size--;
    }

    return 1;
}

// Write unaligned data to target memory.
// size is in bytes.
uint8_t swd_write_memory(uint32_t address, uint8_t *data, uint32_t size)
{
    uint32_t n = 0;

    // Write bytes until word aligned
    while ((size > 0) && (address & 0x3)) {
        if (!swd_write_byte(address, *data)) {
            return 0;
        }

        address++;
        data++;
        size--;
    }

    // Write word aligned blocks
    while (size > 3) {
        // Limit to auto increment page size
        n = TARGET_AUTO_INCREMENT_PAGE_SIZE - (address & (TARGET_AUTO_INCREMENT_PAGE_SIZE - 1));

        if (size < n) {
            n = size & 0xFFFFFFFC; // Only count complete words remaining
        }

        if (!swd_write_block(address, data, n)) {
            return 0;
        }

        address += n;
        data += n;
        size -= n;
    }

    // Write remaining bytes
    while (size > 0) {
        if (!swd_write_byte(address, *data)) {
            return 0;
        }

        address++;
        data++;
        size--;
    }

    return 1;
}

// Execute system call.
static uint8_t swd_write_debug_state(DEBUG_STATE *state)
{
    uint32_t i, status;

    if (!swd_write_dp(DP_SELECT, 0)) {
        return 0;
    }

    // R0, R1, R2, R3
    for (i = 0; i < 4; i++) {
        if (!swd_write_core_register(i, state->r[i])) {
            return 0;
        }
    }

    // R9
    if (!swd_write_core_register(9, state->r[9])) {
        return 0;
    }

    // R13, R14, R15
    for (i = 13; i < 16; i++) {
        if (!swd_write_core_register(i, state->r[i])) {
            return 0;
        }
    }

    // xPSR
    if (!swd_write_core_register(16, state->xpsr)) {
        return 0;
    }

    if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_MASKINTS | C_HALT)) {
        return 0;
    }

    if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_MASKINTS)) {
        return 0;
    }

    // check status
    if (!swd_read_dp(DP_CTRL_STAT, &status)) {
        return 0;
    }

    if (status & (STICKYERR | WDATAERR)) {
        return 0;
    }

    return 1;
}

uint8_t swd_read_core_register(uint32_t n, uint32_t *val)
{
    int i = 0, timeout = 100;

    if (!swd_write_word(DCRSR, n)) {
        return 0;
    }

    // wait for S_REGRDY
    for (i = 0; i < timeout; i++) {
        if (!swd_read_word(DHCSR, val)) {
            return 0;
        }

        if (*val & S_REGRDY) {
            break;
        }
    }

    if (i == timeout) {
        return 0;
    }

    if (!swd_read_word(DCRDR, val)) {
        return 0;
    }

    return 1;
}

uint8_t swd_write_core_register(uint32_t n, uint32_t val)
{
    int i = 0, timeout = 100;

    if (!swd_write_word(DCRDR, val)) {
        return 0;
    }

    if (!swd_write_word(DCRSR, n | REGWnR)) {
        return 0;
    }

    // wait for S_REGRDY
    for (i = 0; i < timeout; i++) {
        if (!swd_read_word(DHCSR, &val)) {
            return 0;
        }

        if (val & S_REGRDY) {
            return 1;
        }
    }

    return 0;
}

static uint8_t swd_wait_until_halted(void)
{
    // Wait for target to stop
    uint32_t val, i, timeout = MAX_TIMEOUT;

    for (i = 0; i < timeout; i++) {
        if (!swd_read_word(DBG_HCSR, &val)) {
            return 0;
        }

        if (val & S_HALT) {
            return 1;
        }
    }

    return 0;
}

uint8_t swd_flash_syscall_exec(const program_syscall_t *sysCallParam, uint32_t entry, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, flash_algo_return_t return_type)
{
    DEBUG_STATE state = {{0}, 0};
    // Call flash algorithm function on target and wait for result.
    state.r[0]     = arg1;                   // R0: Argument 1
    state.r[1]     = arg2;                   // R1: Argument 2
    state.r[2]     = arg3;                   // R2: Argument 3
    state.r[3]     = arg4;                   // R3: Argument 4
    state.r[9]     = sysCallParam->static_base;    // SB: Static Base
    state.r[13]    = sysCallParam->stack_pointer;  // SP: Stack Pointer
    state.r[14]    = sysCallParam->breakpoint;     // LR: Exit Point
    state.r[15]    = entry;                        // PC: Entry Point
    state.xpsr     = 0x01000000;          // xPSR: T = 1, ISR = 0

    if (!swd_write_debug_state(&state)) {
        return 0;
    }

    if (!swd_wait_until_halted()) {
        return 0;
    }

    if (!swd_read_core_register(0, &state.r[0])) {
        return 0;
    }

    //remove the C_MASKINTS
    if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)) {
        return 0;
    }

    if ( return_type == FLASHALGO_RETURN_POINTER ) {
        // Flash verify functions return pointer to byte following the buffer if successful.
        if (state.r[0] != (arg1 + arg2)) {
            return 0;
        }
    }
    else {
        // Flash functions return 0 if successful.
        if (state.r[0] != 0) {
            return 0;
        }
    }

    return 1;
}

// SWD Reset
static uint8_t swd_reset(void)
{
    uint8_t tmp_in[8];
    uint8_t i = 0;

    for (i = 0; i < 8; i++) {
        tmp_in[i] = 0xff;
    }

    SWJ_Sequence(51, tmp_in);
    return 1;
}

// SWD Switch
static uint8_t swd_switch(uint16_t val)
{
    uint8_t tmp_in[2];
    tmp_in[0] = val & 0xff;
    tmp_in[1] = (val >> 8) & 0xff;
    SWJ_Sequence(16, tmp_in);
    return 1;
}

// SWD Read ID
static uint8_t swd_read_idcode(uint32_t *id)
{
    uint8_t tmp_in[1];
    uint8_t tmp_out[4];
    tmp_in[0] = 0x00;
    SWJ_Sequence(8, tmp_in);

    if (swd_read_dp(0, (uint32_t *)tmp_out) != 0x01) {
        return 0;
    }

    *id = (tmp_out[3] << 24) | (tmp_out[2] << 16) | (tmp_out[1] << 8) | tmp_out[0];
    return 1;
}


uint8_t JTAG2SWD()
{
    uint32_t tmp = 0;

    if (!swd_reset()) {
        return 0;
    }

    if (!swd_switch(0xE79E)) {
        return 0;
    }

    if (!swd_reset()) {
        return 0;
    }

    if (!swd_read_idcode(&tmp)) {
        return 0;
    }

    return 1;
}

uint8_t swd_init_debug(void)
{
    uint32_t tmp = 0;
    int i = 0;
    int timeout = 100;
    // init dap state with fake values
    dap_state.select = 0xffffffff;
    dap_state.csw = 0xffffffff;

    int8_t retries = 4;
    int8_t do_abort = 0;
    do {
        if (do_abort) {
            //do an abort on stale target, then reset the device
            swd_write_dp(DP_ABORT, DAPABORT);
            swd_set_target_reset(1);
            osDelay(2);
            swd_set_target_reset(0);
            osDelay(2);
            do_abort = 0;
        }
        swd_init();
        // call a target dependant function
        // this function can do several stuff before really
        // initing the debug
        if (g_target_family && g_target_family->target_before_init_debug) {
            g_target_family->target_before_init_debug();
        }

        if (!JTAG2SWD()) {
            do_abort = 1;
            continue;
        }

        if (!swd_clear_errors()) {
            do_abort = 1;
            continue;
        }

        if (!swd_write_dp(DP_SELECT, 0)) {
            do_abort = 1;
            continue;

        }

        // Power up
        if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
            do_abort = 1;
            continue;
        }

        for (i = 0; i < timeout; i++) {
            if (!swd_read_dp(DP_CTRL_STAT, &tmp)) {
                do_abort = 1;
                break;
            }
            if ((tmp & (CDBGPWRUPACK | CSYSPWRUPACK)) == (CDBGPWRUPACK | CSYSPWRUPACK)) {
                // Break from loop if powerup is complete
                break;
            }
        }
        if ((i == timeout) || (do_abort == 1)) {
            // Unable to powerup DP
            do_abort = 1;
            continue;
        }

        if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ | TRNNORMAL | MASKLANE)) {
            do_abort = 1;
            continue;
        }

        // call a target dependant function:
        // some target can enter in a lock state
        // this function can unlock these targets
        if (g_target_family && g_target_family->target_unlock_sequence) {
            g_target_family->target_unlock_sequence();
        }

        if (!swd_write_dp(DP_SELECT, 0)) {
            do_abort = 1;
            continue;
        }

        return 1;

    } while (--retries > 0);

    return 0;
}

uint8_t swd_set_target_state_hw(target_state_t state)
{
    uint32_t val;
    int8_t ap_retries = 2;
    /* Calling swd_init prior to entering RUN state causes operations to fail. */
    if (state != RUN) {
        swd_init();
    }

    switch (state) {
        case RESET_HOLD:
            swd_set_target_reset(1);
            break;

        case RESET_RUN:
            swd_set_target_reset(1);
            osDelay(2);
            swd_set_target_reset(0);
            osDelay(2);
            swd_off();
            break;

        case RESET_PROGRAM:
            if (!swd_init_debug()) {
                return 0;
            }

            if (reset_connect == CONNECT_UNDER_RESET) {
                // Assert reset
                swd_set_target_reset(1);
                osDelay(2);
            }

            // Enable debug
            while(swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN) == 0) {
                if( --ap_retries <=0 )
                    return 0;
                // Target is in invalid state?
                swd_set_target_reset(1);
                osDelay(2);
                swd_set_target_reset(0);
                osDelay(2);
            }

            // Enable halt on reset
            if (!swd_write_word(DBG_EMCR, VC_CORERESET)) {
                return 0;
            }

            if (reset_connect == CONNECT_NORMAL) {
                // Assert reset
                swd_set_target_reset(1);
                osDelay(2);
            }

            // Deassert reset
            swd_set_target_reset(0);
            osDelay(2);

            do {
                if (!swd_read_word(DBG_HCSR, &val)) {
                    return 0;
                }
            } while ((val & S_HALT) == 0);

            // Disable halt on reset
            if (!swd_write_word(DBG_EMCR, 0)) {
                return 0;
            }

            break;

        case NO_DEBUG:
            if (!swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }

            break;

        case DEBUG:
            if (!JTAG2SWD()) {
                return 0;
            }

            if (!swd_clear_errors()) {
                return 0;
            }

            // Ensure CTRL/STAT register selected in DPBANKSEL
            if (!swd_write_dp(DP_SELECT, 0)) {
                return 0;
            }

            // Power up
            if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
                return 0;
            }

            // Enable debug
            if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN)) {
                return 0;
            }

            break;

        case HALT:
            if (!swd_init_debug()) {
                return 0;
            }

            // Enable debug and halt the core (DHCSR <- 0xA05F0003)
            if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)) {
                return 0;
            }

            // Wait until core is halted
            do {
                if (!swd_read_word(DBG_HCSR, &val)) {
                    return 0;
                }
            } while ((val & S_HALT) == 0);
            break;

        case RUN:
            if (!swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }
            swd_off();
            break;

        case POST_FLASH_RESET:
            // This state should be handled in target_reset.c, nothing needs to be done here.
            break;

        default:
            return 0;
    }

    return 1;
}

uint8_t swd_set_target_state_sw(target_state_t state)
{
    uint32_t val;
    int8_t ap_retries = 2;
    /* Calling swd_init prior to enterring RUN state causes operations to fail. */
    if (state != RUN) {
        swd_init();
    }

    switch (state) {
        case RESET_HOLD:
            swd_set_target_reset(1);
            break;

        case RESET_RUN:
            swd_set_target_reset(1);
            osDelay(2);
            swd_set_target_reset(0);
            osDelay(2);
            swd_off();
            break;

        case RESET_PROGRAM:
            if (!swd_init_debug()) {
                return 0;
            }

            // Enable debug and halt the core (DHCSR <- 0xA05F0003)
            while (swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT) == 0) {
                if ( --ap_retries <=0 ) {
                    return 0;
                }
                // Target is in invalid state?
                swd_set_target_reset(1);
                osDelay(2);
                swd_set_target_reset(0);
                osDelay(2);
            }

            // Wait until core is halted
            do {
                if (!swd_read_word(DBG_HCSR, &val)) {
                    return 0;
                }
            } while ((val & S_HALT) == 0);

            // Enable halt on reset
            if (!swd_write_word(DBG_EMCR, VC_CORERESET)) {
                return 0;
            }

            // Perform a soft reset
            if (!swd_read_word(NVIC_AIRCR, &val)) {
                return 0;
            }

            if (!swd_write_word(NVIC_AIRCR, VECTKEY | (val & SCB_AIRCR_PRIGROUP_Msk) | soft_reset)) {
                return 0;
            }

            osDelay(2);

            do {
                if (!swd_read_word(DBG_HCSR, &val)) {
                    return 0;
                }
            } while ((val & S_HALT) == 0);

            // Disable halt on reset
            if (!swd_write_word(DBG_EMCR, 0)) {
                return 0;
            }

            break;

        case NO_DEBUG:
            if (!swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }

            break;

        case DEBUG:
            if (!JTAG2SWD()) {
                return 0;
            }

            if (!swd_clear_errors()) {
                return 0;
            }

            // Ensure CTRL/STAT register selected in DPBANKSEL
            if (!swd_write_dp(DP_SELECT, 0)) {
                return 0;
            }

            // Power up
            if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
                return 0;
            }

            // Enable debug
            if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN)) {
                return 0;
            }

            break;

        case HALT:
            if (!swd_init_debug()) {
                return 0;
            }

            // Enable debug and halt the core (DHCSR <- 0xA05F0003)
            if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)) {
                return 0;
            }

            // Wait until core is halted
            do {
                if (!swd_read_word(DBG_HCSR, &val)) {
                    return 0;
                }
            } while ((val & S_HALT) == 0);
            break;

        case RUN:
            if (!swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }
            swd_off();
            break;

        case POST_FLASH_RESET:
            // This state should be handled in target_reset.c, nothing needs to be done here.
            break;

        default:
            return 0;
    }

    return 1;
}
#endif
