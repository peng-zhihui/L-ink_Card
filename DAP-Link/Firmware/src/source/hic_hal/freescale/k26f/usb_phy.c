/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_usb.h"
#include "fsl_device_registers.h"

/*!
 * @brief ehci phy initialization.
 *
 * This function initialize ehci phy IP.
 *
 * @param[in] controllerId   ehci controller id, please reference to #usb_controller_index_t.
 * @param[in] freq            the external input clock.
 *                            for example: if the external input clock is 16M, the parameter freq should be 16000000.
 *
 * @retval kStatus_USB_Success      cancel successfully.
 * @retval kStatus_USB_Error        the freq value is incorrect.
 */
uint32_t USB_EhciPhyInit(uint8_t controllerId, uint32_t freq)
{
#if ((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
    USBPHY->TRIM_OVERRIDE_EN = 0x001fU;               /* override IFR value */
    USBPHY->CTRL &= ~USBPHY_CTRL_SFTRST_MASK;         /* release PHY from reset */
    USBPHY->PLL_SIC |= USBPHY_PLL_SIC_PLL_POWER_MASK; /* power up PLL */
    if (freq == 24000000U)
    {
        USBPHY->PLL_SIC |= USBPHY_PLL_SIC_PLL_DIV_SEL(0U);
    }
    else if (freq == 16000000U)
    {
        USBPHY->PLL_SIC |= USBPHY_PLL_SIC_PLL_DIV_SEL(1U);
    }
    else if (freq == 12000000U)
    {
        USBPHY->PLL_SIC |= USBPHY_PLL_SIC_PLL_DIV_SEL(2U);
    }
    else
    {
        return kStatus_USB_Error; /* error */
    }
    USBPHY->PLL_SIC &= ~USBPHY_PLL_SIC_PLL_BYPASS_MASK;     /* clear bypass bit */
    USBPHY->PLL_SIC |= USBPHY_PLL_SIC_PLL_EN_USB_CLKS_MASK; /* enable USB clock output from USB PHY PLL */
    USBPHY->CTRL &= ~USBPHY_CTRL_CLKGATE_MASK;              /* Clear to 0U to run clocks */

    USBPHY->CTRL |= USBPHY_CTRL_SET_ENUTMILEVEL2_MASK; /* support LS device. */
    USBPHY->CTRL |= USBPHY_CTRL_SET_ENUTMILEVEL3_MASK; /* support external FS Hub with LS device connected. */
    /* PWD register provides overall control of the PHY power state */
    USBPHY->PWD = 0U;

    while (!(USBPHY->PLL_SIC & USBPHY_PLL_SIC_PLL_LOCK_MASK))
    {
    }

    /* Decode to trim the nominal 17.78mA current source for the High Speed TX drivers on USB_DP and USB_DM. */
    USBPHY->TX = ((USBPHY->TX & (~USBPHY_TX_D_CAL_MASK)) | USBPHY_TX_D_CAL(0xcU));
#endif

    return kStatus_USB_Success;
}

/*!
 * @brief ehci phy initialization for suspend and resume.
 *
 * This function initialize ehci phy IP for suspend and resume.
 *
 * @param[in] controllerId   ehci controller id, please reference to #usb_controller_index_t.
 * @param[in] freq            the external input clock.
 *                            for example: if the external input clock is 16M, the parameter freq should be 16000000.
 *
 * @retval kStatus_USB_Success      cancel successfully.
 * @retval kStatus_USB_Error        the freq value is incorrect.
 */
uint32_t USB_EhciLowPowerPhyInit(uint8_t controllerId, uint32_t freq)
{
#if ((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
    USBPHY->TRIM_OVERRIDE_EN = 0x001fU;               /* override IFR value */
    USBPHY->CTRL &= ~USBPHY_CTRL_SFTRST_MASK;         /* release PHY from reset */
    USBPHY->PLL_SIC |= USBPHY_PLL_SIC_PLL_POWER_MASK; /* power up PLL */
    if (freq == 24000000U)
    {
        USBPHY->PLL_SIC |= USBPHY_PLL_SIC_PLL_DIV_SEL(0U);
    }
    else if (freq == 16000000U)
    {
        USBPHY->PLL_SIC |= USBPHY_PLL_SIC_PLL_DIV_SEL(1U);
    }
    else if (freq == 12000000U)
    {
        USBPHY->PLL_SIC |= USBPHY_PLL_SIC_PLL_DIV_SEL(2U);
    }
    else
    {
        return kStatus_USB_Error; /* error */
    }
    USBPHY->PLL_SIC &= ~USBPHY_PLL_SIC_PLL_BYPASS_MASK;     /* clear bypass bit */
    USBPHY->PLL_SIC |= USBPHY_PLL_SIC_PLL_EN_USB_CLKS_MASK; /* enable USB clock output from USB PHY PLL */
    USBPHY->CTRL &= ~USBPHY_CTRL_CLKGATE_MASK;              /* Clear to 0U to run clocks */
    USBPHY->CTRL |=
        USBPHY_CTRL_AUTORESUME_EN_MASK | USBPHY_CTRL_ENAUTOCLR_CLKGATE_MASK | USBPHY_CTRL_ENAUTOCLR_PHY_PWD_MASK;
    USBPHY->CTRL |= USBPHY_CTRL_SET_ENUTMILEVEL2_MASK; /* support LS device. */
    USBPHY->CTRL |= USBPHY_CTRL_SET_ENUTMILEVEL3_MASK; /* support external FS Hub with LS device connected. */
    /* PWD register provides overall control of the PHY power state */
    USBPHY->PWD = 0U;

    while (!(USBPHY->PLL_SIC & USBPHY_PLL_SIC_PLL_LOCK_MASK))
    {
    }

    /* now the 480MHz USB clock is up, then configure fractional divider after PLL with PFD
     * pfd clock = 480MHz*18/N, where N=18~35
     * Please note that USB1PFDCLK has to be less than 180MHz for RUN or HSRUN mode
     */
    USBPHY->ANACTRL |= USBPHY_ANACTRL_PFD_FRAC(24);   /* N=24 */
    USBPHY->ANACTRL |= USBPHY_ANACTRL_PFD_CLK_SEL(4); /* div by 4 */

    USBPHY->ANACTRL &= ~USBPHY_ANACTRL_DEV_PULLDOWN_MASK;
    USBPHY->ANACTRL &= ~USBPHY_ANACTRL_PFD_CLKGATE_MASK;
    while (!(USBPHY->ANACTRL & USBPHY_ANACTRL_PFD_STABLE_MASK))
    {
    }

    /* Decode to trim the nominal 17.78mA current source for the High Speed TX drivers on USB_DP and USB_DM. */
    USBPHY->TX = ((USBPHY->TX & (~USBPHY_TX_D_CAL_MASK)) | USBPHY_TX_D_CAL(0xcU));
#endif

    return kStatus_USB_Success;
}

/*!
 * @brief ehci phy de-initialization.
 *
 * This function de-initialize ehci phy IP.
 *
 * @param[in] controllerId   ehci controller id, please reference to #usb_controller_index_t.
 */
void USB_EhciPhyDeinit(uint8_t controllerId)
{
#if ((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
    USBPHY->PLL_SIC &= ~USBPHY_PLL_SIC_PLL_POWER_MASK;       /* power down PLL */
    USBPHY->PLL_SIC &= ~USBPHY_PLL_SIC_PLL_EN_USB_CLKS_MASK; /* disable USB clock output from USB PHY PLL */
    USBPHY->CTRL |= USBPHY_CTRL_CLKGATE_MASK;                /* set to 1U to gate clocks */
#endif
}

/*!
 * @brief ehci phy disconnect detection enable or disable.
 *
 * This function enable/disable host ehci disconnect detection.
 *
 * @param[in] controllerId   ehci controller id, please reference to #usb_controller_index_t.
 * @param[in] enable
 *            1U - enable;
 *            0U - disable;
 */
void USB_EhcihostPhyDisconnectDetectCmd(uint8_t controllerId, uint8_t enable)
{
#if ((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
    if (enable)
    {
        USBPHY->CTRL |= USBPHY_CTRL_ENHOSTDISCONDETECT_MASK;
    }
    else
    {
        USBPHY->CTRL &= (~USBPHY_CTRL_ENHOSTDISCONDETECT_MASK);
    }
#endif
}
