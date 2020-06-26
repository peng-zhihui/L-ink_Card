/*
 * Copyright (c) 2004-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <rl_usb.h>
#include "NuMicro.h"

#define __NO_USB_LIB_C
#include "usb_config.c"

/* Bit definition of CEPCTL register */
#define HSUSBD_CEPCTL_NAKCLR               ((uint32_t)0x00000000UL)
#define HSUSBD_CEPCTL_STALL                ((uint32_t)0x00000002UL)
#define HSUSBD_CEPCTL_ZEROLEN              ((uint32_t)0x00000004UL)
#define HSUSBD_CEPCTL_FLUSH                ((uint32_t)0x00000008UL)

/* Bit definition of EPxRSPCTL register */
#define HSUSBD_EP_RSPCTL_FLUSH             ((uint32_t)0x00000001UL)
#define HSUSBD_EP_RSPCTL_MODE_AUTO         ((uint32_t)0x00000000UL)
#define HSUSBD_EP_RSPCTL_MODE_MANUAL       ((uint32_t)0x00000002UL)
#define HSUSBD_EP_RSPCTL_MODE_FLY          ((uint32_t)0x00000004UL)
#define HSUSBD_EP_RSPCTL_MODE_MASK         ((uint32_t)0x00000006UL)
#define HSUSBD_EP_RSPCTL_TOGGLE            ((uint32_t)0x00000008UL)
#define HSUSBD_EP_RSPCTL_HALT              ((uint32_t)0x00000010UL)
#define HSUSBD_EP_RSPCTL_ZEROLEN           ((uint32_t)0x00000020UL)
#define HSUSBD_EP_RSPCTL_SHORTTXEN         ((uint32_t)0x00000040UL)
#define HSUSBD_EP_RSPCTL_DISBUF            ((uint32_t)0x00000080UL)

/* Bit definition of EPxCFG register */
#define HSUSBD_EP_CFG_VALID                ((uint32_t)0x00000001UL)
#define HSUSBD_EP_CFG_TYPE_BULK            ((uint32_t)0x00000002UL)
#define HSUSBD_EP_CFG_TYPE_INT             ((uint32_t)0x00000004UL)
#define HSUSBD_EP_CFG_TYPE_ISO             ((uint32_t)0x00000006UL)
#define HSUSBD_EP_CFG_TYPE_MASK            ((uint32_t)0x00000006UL)
#define HSUSBD_EP_CFG_DIR_OUT              ((uint32_t)0x00000000UL)
#define HSUSBD_EP_CFG_DIR_IN               ((uint32_t)0x00000008UL)

#define HSUSBD_ENABLE_USB()               ((uint32_t)(HSUSBD->PHYCTL |= (HSUSBD_PHYCTL_PHYEN_Msk|HSUSBD_PHYCTL_DPPUEN_Msk)))
#define HSUSBD_DISABLE_USB()              ((uint32_t)(HSUSBD->PHYCTL &= ~HSUSBD_PHYCTL_DPPUEN_Msk))
#define HSUSBD_ENABLE_PHY()               ((uint32_t)(HSUSBD->PHYCTL |= HSUSBD_PHYCTL_PHYEN_Msk))
#define HSUSBD_DISABLE_PHY()              ((uint32_t)(HSUSBD->PHYCTL &= ~HSUSBD_PHYCTL_PHYEN_Msk))
#define HSUSBD_SET_SE0()                  ((uint32_t)(HSUSBD->PHYCTL &= ~HSUSBD_PHYCTL_DPPUEN_Msk))
#define HSUSBD_CLR_SE0()                  ((uint32_t)(HSUSBD->PHYCTL |= HSUSBD_PHYCTL_DPPUEN_Msk))
#define HSUSBD_SET_ADDR(addr)             (HSUSBD->FADDR = (addr))
#define HSUSBD_GET_ADDR()                 ((uint32_t)(HSUSBD->FADDR))
#define HSUSBD_ENABLE_USB_INT(intr)       (HSUSBD->GINTEN = (intr))
#define HSUSBD_ENABLE_BUS_INT(intr)       (HSUSBD->BUSINTEN = (intr))
#define HSUSBD_GET_BUS_INT_FLAG()         (HSUSBD->BUSINTSTS)
#define HSUSBD_CLR_BUS_INT_FLAG(flag)     (HSUSBD->BUSINTSTS = (flag))
#define HSUSBD_ENABLE_CEP_INT(intr)       (HSUSBD->CEPINTEN = (intr))
#define HSUSBD_CLR_CEP_INT_FLAG(flag)     (HSUSBD->CEPINTSTS = (flag))
#define HSUSBD_SET_CEP_STATE(flag)        (HSUSBD->CEPCTL = (flag))
#define HSUSBD_START_CEP_IN(size)         (HSUSBD->CEPTXCNT = (size))
#define HSUSBD_SET_MAX_PAYLOAD(ep, size)  (HSUSBD->EP[(ep)].EPMPS = (size))
#define HSUSBD_ENABLE_EP_INT(ep, intr)    (HSUSBD->EP[(ep)].EPINTEN = (intr))
#define HSUSBD_GET_EP_INT_FLAG(ep)        (HSUSBD->EP[(ep)].EPINTSTS)
#define HSUSBD_CLR_EP_INT_FLAG(ep, flag)  (HSUSBD->EP[(ep)].EPINTSTS = (flag))
#define HSUSBD_SET_DMA_LEN(len)           (HSUSBD->DMACNT = (len))
#define HSUSBD_SET_DMA_ADDR(addr)         (HSUSBD->DMAADDR = (addr))
#define HSUSBD_SET_DMA_READ(epnum)        (HSUSBD->DMACTL = (HSUSBD->DMACTL & ~HSUSBD_DMACTL_EPNUM_Msk) | HSUSBD_DMACTL_DMARD_Msk | (epnum) | 0x100)
#define HSUSBD_SET_DMA_WRITE(epnum)       (HSUSBD->DMACTL = (HSUSBD->DMACTL & ~(HSUSBD_DMACTL_EPNUM_Msk | HSUSBD_DMACTL_DMARD_Msk | 0x100)) | (epnum))
#define HSUSBD_ENABLE_DMA()               (HSUSBD->DMACTL |= HSUSBD_DMACTL_DMAEN_Msk)
#define HSUSBD_IS_ATTACHED()              ((uint32_t)(HSUSBD->PHYCTL & HSUSBD_PHYCTL_VBUSDET_Msk))

#define HSUSBD_MAX_EP       12UL
#define CEP                 0xFFUL
#define EPA                 0UL
#define EPB                 1UL
#define EPC                 2UL
#define EPD                 3UL
#define EPE                 4UL
#define EPF                 5UL
#define EPG                 6UL
#define EPH                 7UL
#define EPI                 8UL
#define EPJ                 9UL
#define EPK                 10UL
#define EPL                 11UL

#define USBD_EP_TO_NUM(ep)  ((ep == CEP) ? (0) : (1 + (ep - EPA)))
#define USBD_NUM_TO_EP(num) ((num == 0) ? (CEP) : (EPA + (num - 1)))

#define CEP_BUF_BASE        0
#define CEP_BUF_LEN         USBD_MAX_PACKET0

static uint32_t g_u32FreeBufAddr;
static uint8_t g_u8StatusIn;

/*
 *  USB Device Interrupt enable
 *   Called by USBD_Init to enable the USB Interrupt
 *    Return Value:    None
 */

#ifdef __RTX
void __svc(1) USBD_IntrEna(void);
void __SVC_1(void)
{
#else
void          USBD_IntrEna(void)
{
#endif
    NVIC_EnableIRQ(USBD20_IRQn);
}


/*
 *  USB Device Initialize Function
 *   Called by the User to initialize USB
 *    Return Value:    None
 */

void USBD_Init(void)
{
    uint32_t volatile i;
    /* Initial USB engine */
    HSUSBD_ENABLE_PHY();

    /* wait PHY clock ready */
    while (1) {
        HSUSBD->EP[EPA].EPMPS = 0x20UL;

        if (HSUSBD->EP[EPA].EPMPS == 0x20UL) {
            break;
        }
    }

    for (i = 0; i < 0x10000; i++);

    if (HSUSBD->OPER & HSUSBD_OPER_CURSPD_Msk) {
        USBD_HighSpeed = __TRUE;
    } else {
        USBD_HighSpeed = __FALSE;
    }

    /* Enable USB BUS, CEP global interrupt */
    HSUSBD_ENABLE_USB_INT(HSUSBD_GINTEN_USBIEN_Msk | HSUSBD_GINTEN_CEPIEN_Msk);
    /* Enable BUS interrupt */
    HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RESUMEIEN_Msk | HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_VBUSDETIEN_Msk | HSUSBD_BUSINTEN_SOFIEN_Msk);
    /* Reset Address to 0 */
    HSUSBD_SET_ADDR(0);
    /* Control endpoint */
    HSUSBD->CEPBUFST = CEP_BUF_BASE;
    HSUSBD->CEPBUFEND = CEP_BUF_BASE + CEP_BUF_LEN - 1UL;
    HSUSBD_ENABLE_CEP_INT(HSUSBD_CEPINTEN_SETUPPKIEN_Msk | HSUSBD_CEPINTEN_STSDONEIEN_Msk | HSUSBD_CEPINTEN_RXPKIEN_Msk | HSUSBD_CEPINTEN_TXPKIEN_Msk);
    USBD_IntrEna();
}


/*
 *  USB Device Connect Function
 *   Called by the User to Connect/Disconnect USB Device
 *    Parameters:      con:   Connect/Disconnect
 *    Return Value:    None
 */

void USBD_Connect(BOOL con)
{
    if (con) {
        HSUSBD_CLR_SE0();
    } else {
        HSUSBD_SET_SE0();
    }
}


/*
 *  USB Device Reset Function
 *   Called automatically on USB Device Reset
 *    Return Value:    None
 */

void USBD_Reset(void)
{
    uint32_t i;

    for (i = 0; i < HSUSBD_MAX_EP; i++) {
        HSUSBD->EP[EPA + i].EPRSPCTL = HSUSBD_EPRSPCTL_FLUSH_Msk;
    }

    if (HSUSBD->OPER & HSUSBD_OPER_CURSPD_Msk) {
        USBD_HighSpeed = __TRUE;
    } else {
        USBD_HighSpeed = __FALSE;
    }

    g_u32FreeBufAddr = CEP_BUF_BASE + CEP_BUF_LEN;
    g_u8StatusIn = 0;
    HSUSBD_SET_ADDR(0);
}


/*
 *  USB Device Suspend Function
 *   Called automatically on USB Device Suspend
 *    Return Value:    None
 */

void USBD_Suspend(void)
{
}


/*
 *  USB Device Resume Function
 *   Called automatically on USB Device Resume
 *    Return Value:    None
 */

void USBD_Resume(void)
{
}


/*
 *  USB Device Remote Wakeup Function
 *   Called automatically on USB Device Remote Wakeup
 *    Return Value:    None
 */

void USBD_WakeUp(void)
{
}


/*
 *  USB Device Remote Wakeup Configuration Function
 *    Parameters:      cfg:   Device Enable/Disable
 *    Return Value:    None
 */

void USBD_WakeUpCfg(BOOL cfg)
{
}


/*
 *  USB Device Set Address Function
 *    Parameters:      adr:   USB Device Address
 *                     setup: Called in setup stage (!=0), else after status stage
 *    Return Value:    None
 */

void USBD_SetAddress(U32 adr, U32 setup)
{
    if (setup) {
        return;
    }

    HSUSBD_SET_ADDR(adr);
}


/*
 *  USB Device Configure Function
 *    Parameters:      cfg:   Device Configure/Deconfigure
 *    Return Value:    None
 */

void USBD_Configure(BOOL cfg)
{
    if (cfg == __FALSE) {
        g_u32FreeBufAddr = CEP_BUF_BASE + CEP_BUF_LEN;
    }
}


/*
 *  Configure USB Device Endpoint according to Descriptor
 *    Parameters:      pEPD:  Pointer to Device Endpoint Descriptor
 *    Return Value:    None
 */

void USBD_ConfigEP(USB_ENDPOINT_DESCRIPTOR *pEPD)
{
    uint32_t u32Num, u32Ep, u32Size, u32Type, u32Dir;
    u32Num = pEPD->bEndpointAddress & 0x0F;
    u32Ep = USBD_NUM_TO_EP(u32Num);
    u32Size = pEPD->wMaxPacketSize;

    switch (pEPD->bmAttributes & USB_ENDPOINT_TYPE_MASK) {
        case USB_ENDPOINT_TYPE_ISOCHRONOUS:
            u32Type = HSUSBD_EP_CFG_TYPE_ISO;
            break;

        case USB_ENDPOINT_TYPE_BULK:
            u32Type = HSUSBD_EP_CFG_TYPE_BULK;
            break;

        case USB_ENDPOINT_TYPE_INTERRUPT:
            u32Type = HSUSBD_EP_CFG_TYPE_INT;
            break;
    }

    if (pEPD->bEndpointAddress & USB_ENDPOINT_DIRECTION_MASK) {
        u32Dir = HSUSBD_EP_CFG_DIR_IN;
    } else {
        u32Dir = HSUSBD_EP_CFG_DIR_OUT;
    }

    HSUSBD->EP[u32Ep].EPBUFST = g_u32FreeBufAddr;
    HSUSBD->EP[u32Ep].EPBUFEND = g_u32FreeBufAddr + u32Size - 1UL;
    HSUSBD_SET_MAX_PAYLOAD(u32Ep, u32Size);
    HSUSBD->EP[u32Ep].EPRSPCTL = (HSUSBD_EP_RSPCTL_FLUSH | HSUSBD_EP_RSPCTL_MODE_AUTO);
    HSUSBD->EP[u32Ep].EPCFG = (u32Type | u32Dir | HSUSBD_EP_CFG_VALID | (u32Num << 4));
    g_u32FreeBufAddr += u32Size;
}


/*
 *  Set Direction for USB Device Control Endpoint
 *    Parameters:      dir:   Out (dir == 0), In (dir <> 0)
 *    Return Value:    None
 */

void USBD_DirCtrlEP(U32 dir)
{
}


/*
 *  Enable USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_EnableEP(U32 EPNum)
{
    uint32_t u32Ep, u32Num, u32Intr;
    u32Num = EPNum & 0x0F;
    u32Ep = USBD_NUM_TO_EP(u32Num);
    HSUSBD->GINTEN |= (0x1UL << (HSUSBD_GINTEN_EPAIEN_Pos + (u32Num - USBD_EP_TO_NUM(EPA))));

    if (EPNum & 0x80) {
        u32Intr = HSUSBD_EPINTEN_TXPKIEN_Msk;
    } else {
        u32Intr = HSUSBD_EPINTEN_RXPKIEN_Msk | HSUSBD_EPINTEN_SHORTRXIEN_Msk | HSUSBD_EPINTEN_BUFFULLIEN_Msk;
    }

    HSUSBD_ENABLE_EP_INT(u32Ep, u32Intr);
}


/*
 *  Disable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_DisableEP(U32 EPNum)
{
    uint32_t u32Ep, u32Num;
    u32Num = EPNum & 0x0F;
    u32Ep = USBD_NUM_TO_EP(u32Num);
    HSUSBD->GINTEN &= ~(0x1UL << (HSUSBD_GINTEN_EPAIEN_Pos + (u32Num - USBD_EP_TO_NUM(EPA))));
    HSUSBD_ENABLE_EP_INT(u32Ep, 0);
}


/*
 *  Reset USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_ResetEP(U32 EPNum)
{
    uint32_t u32Ep, u32Num;
    u32Num = EPNum & 0x0F;
    u32Ep = USBD_NUM_TO_EP(u32Num);
    HSUSBD->EP[u32Ep].EPRSPCTL = (HSUSBD->EP[u32Ep].EPRSPCTL & HSUSBD_EP_RSPCTL_MODE_MASK) | HSUSBD_EPRSPCTL_FLUSH_Msk;
}


/*
 *  Set Stall for USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_SetStallEP(U32 EPNum)
{
    uint32_t u32Ep, u32Num;
    u32Num = EPNum & 0x0F;
    u32Ep = USBD_NUM_TO_EP(u32Num);

    if (u32Ep == CEP) {
        HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_STALL);
    } else {
        HSUSBD->EP[u32Ep].EPRSPCTL = (HSUSBD->EP[u32Ep].EPRSPCTL & 0xF7UL) | HSUSBD_EP_RSPCTL_HALT;
    }
}


/*
 *  Clear Stall for USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_ClrStallEP(U32 EPNum)
{
    uint32_t u32Ep, u32Num;
    u32Num = EPNum & 0x0F;
    u32Ep = USBD_NUM_TO_EP(u32Num);

    if (u32Ep == CEP) {
        HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR_Msk);
    } else {
        HSUSBD->EP[u32Ep].EPRSPCTL = (HSUSBD->EP[u32Ep].EPRSPCTL & HSUSBD_EP_RSPCTL_MODE_MASK) | HSUSBD_EP_RSPCTL_TOGGLE;
    }
}


/*
 *  Clear USB Device Endpoint Buffer
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_ClearEPBuf(U32 EPNum)
{
}


/*
 *  Read USB Device Endpoint Data
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

U32 USBD_ReadEP(U32 EPNum, U8 *pData, U32 bufsz)
{
    uint32_t u32Ep, u32Num, u32Len, i;
    u32Num = EPNum & 0x0F;
    u32Ep = USBD_NUM_TO_EP(u32Num);

    if (u32Num == 0) {
        if (pData == (uint8_t *)&USBD_SetupPacket) {
            *((uint16_t *)(pData + 0)) = (uint16_t)(HSUSBD->SETUP1_0 & 0xFFFFUL);
            *((uint16_t *)(pData + 2)) = (uint16_t)(HSUSBD->SETUP3_2 & 0xFFFFUL);
            *((uint16_t *)(pData + 4)) = (uint16_t)(HSUSBD->SETUP5_4 & 0xFFFFUL);
            *((uint16_t *)(pData + 6)) = (uint16_t)(HSUSBD->SETUP7_6 & 0xFFFFUL);
            return 8;
        } else {
            u32Len = HSUSBD->CEPDATCNT & 0xFFFFUL;

            if (u32Len > bufsz) {
                u32Len = bufsz;
            }

            for (i = 0; i < bufsz; i++) {
                pData[i] = inpb(&HSUSBD->CEPDAT);
            }

            return u32Len;
        }
    } else {
        u32Len = HSUSBD->EP[u32Ep].EPDATCNT & 0xFFFFUL;

        if (u32Len > bufsz) {
            u32Len = bufsz;
        }

        for (i = 0; i < u32Len; i++) {
            pData[i] = HSUSBD->EP[u32Ep].EPDAT_BYTE;
        }

        return u32Len;
    }
}


/*
 *  Write USB Device Endpoint Data
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *                     cnt:   Number of bytes to write
 *    Return Value:    Number of bytes written
 */

U32 USBD_WriteEP(U32 EPNum, U8 *pData, U32 cnt)
{
    uint32_t u32Ep, u32Num, i;
    u32Num = EPNum & 0x0F;
    u32Ep = USBD_NUM_TO_EP(u32Num);

    if (u32Num == 0) {
        if (pData != NULL) {
            if (cnt > 0) {
                for (i = 0; i < cnt; i++) {
                    HSUSBD->CEPDAT_BYTE = pData[i];
                }

                HSUSBD_START_CEP_IN(cnt);
            } else {
                HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_ZEROLEN);
            }
        } else if (cnt == 0) {
            g_u8StatusIn = 1;
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
        }

        return cnt;
    } else {
        if (cnt > 0) {
            for (i = 0; i < cnt; i++) {
                HSUSBD->EP[u32Ep].EPDAT_BYTE = pData[i];
            }

            HSUSBD->EP[u32Ep].EPRSPCTL = (HSUSBD->EP[u32Ep].EPRSPCTL & HSUSBD_EP_RSPCTL_HALT) | HSUSBD_EP_RSPCTL_SHORTTXEN;
        } else {
            HSUSBD->EP[u32Ep].EPRSPCTL = (HSUSBD->EP[u32Ep].EPRSPCTL & HSUSBD_EP_RSPCTL_HALT) | HSUSBD_EP_RSPCTL_ZEROLEN;
        }

        return cnt;
    }
}


/*
 *  Get USB Device Last Frame Number
 *    Parameters:      None
 *    Return Value:    Frame Number
 */

U32 USBD_GetFrame(void)
{
    return 0;
}


#ifdef __RTX
U32 LastError;

/*
 *  Get USB Last Error Code
 *    Parameters:      None
 *    Return Value:    Error Code
 */

U32 USBD_GetError(void)
{
    return (LastError);
}
#endif


/*
 *  USB Device Interrupt Service Routine
 */

void USBD20_IRQHandler(void)
{
    NVIC_DisableIRQ(USBD20_IRQn);
    USBD_SignalHandler();
}

void USBD_Handler_Main()
{
    __IO uint32_t IrqStL, IrqSt;
    uint32_t u32Ep, u32Num, i;
    IrqStL = HSUSBD->GINTSTS & HSUSBD->GINTEN;

    if (!IrqStL) {
        return;
    }

    if (IrqStL & HSUSBD_GINTSTS_USBIF_Msk) {
        IrqSt = HSUSBD->BUSINTSTS & HSUSBD->BUSINTEN;

        if (IrqSt & HSUSBD_BUSINTSTS_SOFIF_Msk) {
#ifdef __RTX

            if (USBD_RTX_DevTask) {
                isr_evt_set(USBD_EVT_SOF, USBD_RTX_DevTask);
            }

#else

            if (USBD_P_SOF_Event) {
                USBD_P_SOF_Event();
            }

#endif
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_SOFIF_Msk);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_RSTIF_Msk) {
            USBD_Reset();
            usbd_reset_core();
#ifdef __RTX

            if (USBD_RTX_DevTask) {
                isr_evt_set(USBD_EVT_RESET, USBD_RTX_DevTask);
            }

#else

            if (USBD_P_Reset_Event) {
                USBD_P_Reset_Event();
            }

#endif
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk | HSUSBD_BUSINTEN_SUSPENDIEN_Msk | HSUSBD_BUSINTEN_SOFIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_RSTIF_Msk);
            HSUSBD_CLR_CEP_INT_FLAG(0x1FFC);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_RESUMEIF_Msk) {
            USBD_WakeUp();
#ifdef __RTX

            if (USBD_RTX_DevTask) {
                isr_evt_set(USBD_EVT_RESUME,  USBD_RTX_DevTask);
            }

#else

            if (USBD_P_Resume_Event) {
                USBD_P_Resume_Event();
            }

#endif
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_SUSPENDIEN_Msk | HSUSBD_BUSINTEN_SOFIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_RESUMEIF_Msk);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_SUSPENDIF_Msk) {
            USBD_Suspend();
#ifdef __RTX

            if (USBD_RTX_DevTask) {
                isr_evt_set(USBD_EVT_SUSPEND, USBD_RTX_DevTask);
            }

#else

            if (USBD_P_Suspend_Event) {
                USBD_P_Suspend_Event();
            }

#endif
            HSUSBD_ENABLE_BUS_INT(HSUSBD_BUSINTEN_RSTIEN_Msk | HSUSBD_BUSINTEN_RESUMEIEN_Msk | HSUSBD_BUSINTEN_SOFIEN_Msk);
            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_SUSPENDIF_Msk);
        }

        if (IrqSt & HSUSBD_BUSINTSTS_VBUSDETIF_Msk) {
            if (HSUSBD_IS_ATTACHED()) {
                HSUSBD_ENABLE_USB();
            } else {
                HSUSBD_DISABLE_USB();
            }

            HSUSBD_CLR_BUS_INT_FLAG(HSUSBD_BUSINTSTS_VBUSDETIF_Msk);
        }
    }

    if (IrqStL & HSUSBD_GINTSTS_CEPIF_Msk) {
        IrqSt = HSUSBD->CEPINTSTS & HSUSBD->CEPINTEN;

        if (IrqSt & HSUSBD_CEPINTSTS_SETUPPKIF_Msk) {
#ifdef __RTX

            if (USBD_RTX_EPTask[0]) {
                isr_evt_set(USBD_EVT_SETUP, USBD_RTX_EPTask[0]);
            }

#else

            if (USBD_P_EP[0]) {
                USBD_P_EP[0](USBD_EVT_SETUP);
            }

#endif
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_SETUPPKIF_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_TXPKIF_Msk) {
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            HSUSBD_SET_CEP_STATE(HSUSBD_CEPCTL_NAKCLR);
#ifdef __RTX

            if (USBD_RTX_EPTask[0]) {
                isr_evt_set(USBD_EVT_IN, USBD_RTX_EPTask[0]);
            }

#else

            if (USBD_P_EP[0]) {
                USBD_P_EP[0](USBD_EVT_IN);
            }

#endif
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_TXPKIF_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_RXPKIF_Msk) {
#ifdef __RTX

            if (USBD_RTX_EPTask[0]) {
                isr_evt_set(USBD_EVT_OUT, USBD_RTX_EPTask[0]);
            }

#else

            if (USBD_P_EP[0]) {
                USBD_P_EP[0](USBD_EVT_OUT);
            }

#endif
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_RXPKIF_Msk);
            return;
        }

        if (IrqSt & HSUSBD_CEPINTSTS_STSDONEIF_Msk) {
            if (g_u8StatusIn == 0) {
#ifdef __RTX

                if (USBD_RTX_EPTask[0]) {
                    isr_evt_set(USBD_EVT_OUT, USBD_RTX_EPTask[0]);
                }

#else

                if (USBD_P_EP[0]) {
                    USBD_P_EP[0](USBD_EVT_OUT);
                }

#endif
            } else {
#ifdef __RTX

                if (USBD_RTX_EPTask[0]) {
                    isr_evt_set(USBD_EVT_IN, USBD_RTX_EPTask[0]);
                }

#else

                if (USBD_P_EP[0]) {
                    USBD_P_EP[0](USBD_EVT_IN);
                }

#endif
            }

            g_u8StatusIn = 0;
            HSUSBD_CLR_CEP_INT_FLAG(HSUSBD_CEPINTSTS_STSDONEIF_Msk);
            return;
        }
    }

    for (i = 0; i < HSUSBD_MAX_EP; i++) {
        u32Ep = EPA + i;
        u32Num = USBD_EP_TO_NUM(u32Ep);

        if (IrqStL & (0x1UL << (HSUSBD_GINTSTS_EPAIF_Pos + i))) {
            IrqSt = HSUSBD->EP[u32Ep].EPINTSTS & HSUSBD->EP[u32Ep].EPINTEN;

            if (IrqSt & HSUSBD_EPINTSTS_TXPKIF_Msk) {
#ifdef __RTX

                if (USBD_RTX_EPTask[u32Num]) {
                    isr_evt_set(USBD_EVT_IN, USBD_RTX_EPTask[u32Num]);
                }

#else

                if (USBD_P_EP[u32Num]) {
                    USBD_P_EP[u32Num](USBD_EVT_IN);
                }

#endif
            }

            if (IrqSt & (HSUSBD_EPINTSTS_RXPKIF_Msk | HSUSBD_EPINTSTS_SHORTRXIF_Msk | HSUSBD_EPINTEN_BUFFULLIEN_Msk)) {
#ifdef __RTX

                if (USBD_RTX_EPTask[u32Num]) {
                    isr_evt_set(USBD_EVT_OUT, USBD_RTX_EPTask[u32Num]);
                }

#else

                if (USBD_P_EP[u32Num]) {
                    USBD_P_EP[u32Num](USBD_EVT_OUT);
                }

#endif
            }

            HSUSBD_CLR_EP_INT_FLAG(u32Ep, IrqSt);
        }
    }
}

void USBD_Handler(void)
{
    USBD_Handler_Main();
    NVIC_EnableIRQ(USBD20_IRQn);
}
