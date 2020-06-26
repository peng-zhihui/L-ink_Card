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

#include "FlashOS.h"
#include "max32625.h"
#include "flc_regs.h"

/******************************************************************************/
static inline int FLC_Busy(void)
{
    return (MXC_FLC->ctrl & (MXC_F_FLC_CTRL_WRITE | MXC_F_FLC_CTRL_MASS_ERASE | MXC_F_FLC_CTRL_PAGE_ERASE));
}

/******************************************************************************/
uint32_t Init(uint32_t adr, uint32_t clk, uint32_t fnc)
{
    /* Check if the flash controller is busy */
    if (FLC_Busy()) {
        return 1;
    }

    /* Enable automatic calculation of the clock divider to generate a 1MHz clock from the APB clock */
    MXC_FLC->perform |= MXC_F_FLC_PERFORM_AUTO_CLKDIV;

    /* The flash controller will stall any reads while flash operations are in
     * progress. Disable the legacy failure detection logic that would flag reads
     * during flash operations as errors.
     */
    MXC_FLC->perform |= MXC_F_FLC_PERFORM_EN_PREVENT_FAIL;

    return 0;
}

/******************************************************************************/
uint32_t UnInit(uint32_t fnc)
{
    /* Lock flash */
    MXC_FLC->ctrl &= ~MXC_F_FLC_CTRL_FLSH_UNLOCK;

    return  0;  // Finished without Errors
}

/******************************************************************************/
/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */
int EraseChip(void)
{
    /* Check if the flash controller is busy */
    if (FLC_Busy()) {
        return 1;
    }

    /* Clear stale errors. Interrupt flags can only be written to zero, so this is safe */
    MXC_FLC->intr &= ~MXC_F_FLC_INTR_FAILED_IF;

    /* Unlock flash */
    MXC_FLC->ctrl = (MXC_FLC->ctrl & ~MXC_F_FLC_CTRL_FLSH_UNLOCK) | (MXC_V_FLC_FLSH_UNLOCK_KEY << MXC_F_FLC_CTRL_FLSH_UNLOCK_POS);

    /* Write the Erase Code */
    MXC_FLC->ctrl = (MXC_FLC->ctrl & ~MXC_F_FLC_CTRL_ERASE_CODE) | (MXC_V_FLC_ERASE_CODE_MASS_ERASE << MXC_F_FLC_CTRL_ERASE_CODE_POS);

    /* Start the mass erase */
    MXC_FLC->ctrl |= MXC_F_FLC_CTRL_MASS_ERASE;

    /* Wait until flash operation is complete */
    while (FLC_Busy());

    /* Lock flash */
    MXC_FLC->ctrl &= ~(MXC_F_FLC_CTRL_FLSH_UNLOCK | MXC_F_FLC_CTRL_ERASE_CODE);

    /* Check for failures */
    if (MXC_FLC->intr & MXC_F_FLC_INTR_FAILED_IF) {
        /* Interrupt flags can only be written to zero, so this is safe */
        MXC_FLC->intr &= ~MXC_F_FLC_INTR_FAILED_IF;
        return 1;
    }

    return 0;
}

/******************************************************************************/
/*
 *  Erase Sector in Flash Memory
 *    Parameter:      address:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */
int EraseSector(unsigned long address)
{
    /* Wait until flash operation is complete */
    while (FLC_Busy());

    /* Clear stale errors. Interrupt flags can only be written to zero, so this is safe */
    MXC_FLC->intr &= ~MXC_F_FLC_INTR_FAILED_IF;

    /* Unlock flash */
    MXC_FLC->ctrl = (MXC_FLC->ctrl & ~MXC_F_FLC_CTRL_FLSH_UNLOCK) | (MXC_V_FLC_FLSH_UNLOCK_KEY << MXC_F_FLC_CTRL_FLSH_UNLOCK_POS);

    /* Write page erase code */
    MXC_FLC->ctrl = (MXC_FLC->ctrl & ~MXC_F_FLC_CTRL_ERASE_CODE) | (MXC_V_FLC_ERASE_CODE_PAGE_ERASE << MXC_F_FLC_CTRL_ERASE_CODE_POS);

    /* Erase the request page */
    MXC_FLC->faddr = address;
    MXC_FLC->ctrl |= MXC_F_FLC_CTRL_PAGE_ERASE;

    /* Wait until flash operation is complete */
    while (FLC_Busy());

    /* Lock flash */
    MXC_FLC->ctrl &= ~(MXC_F_FLC_CTRL_FLSH_UNLOCK | MXC_F_FLC_CTRL_ERASE_CODE);

    /* Check for failures */
    if (MXC_FLC->intr & MXC_F_FLC_INTR_FAILED_IF) {
        /* Interrupt flags can only be written to zero, so this is safe */
        MXC_FLC->intr &= ~MXC_F_FLC_INTR_FAILED_IF;
        return 1;
    }

    return 0;
}

/******************************************************************************/
/*
 *  Program Page in Flash Memory
 *    Parameter:      address:  Page Start Address
 *                    size:     Page Size
 *                    buffer:   Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */
int ProgramPage(unsigned long address, unsigned long size, unsigned char *buffer8)
{
    unsigned long remaining = size;
    unsigned long *buffer = (unsigned long *)buffer8;

    // Only accept 32-bit aligned pointers
    if ((unsigned long)buffer8 & 0x3) {
        return 1;
    }
    buffer = (unsigned long *)buffer8;

    /* Check if the flash controller is busy */
    if (FLC_Busy()) {
        return 1;
    }

    /* Unlock flash */
    MXC_FLC->ctrl = (MXC_FLC->ctrl & ~MXC_F_FLC_CTRL_FLSH_UNLOCK) | (MXC_V_FLC_FLSH_UNLOCK_KEY << MXC_F_FLC_CTRL_FLSH_UNLOCK_POS);

    while (remaining >= 4) {
        MXC_FLC->faddr = address;
        MXC_FLC->fdata = *buffer++;
        MXC_FLC->ctrl |= MXC_F_FLC_CTRL_WRITE_ENABLE;
        MXC_FLC->ctrl |= MXC_F_FLC_CTRL_WRITE;

        /* Wait until flash operation is complete */
        while (FLC_Busy());

        address += 4;
        remaining -= 4;
    }

    if (remaining > 0) {
        uint32_t last_word;
        uint32_t mask;

        last_word = 0xffffffff;
        mask = 0xff;

        while (remaining > 0) {
            last_word &= (*buffer | ~mask);
            mask <<= 8;
            remaining--;
        }

        MXC_FLC->faddr = address;
        MXC_FLC->fdata = last_word;
        MXC_FLC->ctrl |= MXC_F_FLC_CTRL_WRITE_ENABLE;

        /* Wait until flash operation is complete */
        while (FLC_Busy());
    }

    /* Lock flash */
    MXC_FLC->ctrl &= ~MXC_F_FLC_CTRL_FLSH_UNLOCK;

    /* Check for failures */
    if (MXC_FLC->intr & MXC_F_FLC_INTR_FAILED_IF) {
        /* Interrupt flags can only be written to zero, so this is safe */
        MXC_FLC->intr &= ~MXC_F_FLC_INTR_FAILED_IF;
        return 1;
    }

    return  0;
}
