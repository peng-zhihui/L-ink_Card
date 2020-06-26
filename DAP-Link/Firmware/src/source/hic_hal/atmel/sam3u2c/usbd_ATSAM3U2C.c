/**
 * @file    usbd_ATSAM3U2C.c
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

#include "rl_usb.h"
#include "sam3u.h"
#include "util.h"

#define __NO_USB_LIB_C
#include "usb_config.c"

#define UDPHS_EPTFIFO_BASE (0x20180000) /* (UDPHS_EPTFIFO) Base Address       */
uint32_t eptsta_copy[USBD_EP_NUM + 1];

/*
 *  Calculate EP size code Function
 *   Called during EndPoint configuration
 *    Return Value:    EP size code for given EP size
 */

static int USBD_CalcSizeEP(uint32_t size)
{
    if (size <=    8) {
        return (0);
    } else if (size <=   16) {
        return (1);
    } else if (size <=   32) {
        return (2);
    } else if (size <=   64) {
        return (3);
    } else if (size <=  128) {
        return (4);
    } else if (size <=  256) {
        return (5);
    } else if (size <=  512)  {
        return (6);
    } else if (size <= 1024) {
        return (7);
    }

    return (0);
}


/*
 *  Retrieve maximum EP size Function
 *   Called during EndPoint configuration
 *    Return Value:    maximum size for given EP
 */

static int USBD_GetSizeEP(uint32_t EPNum)
{
    switch (EPNum & 0x0F) {
        case 0:
            return (64);                      /* Maximum size is 64 bytes           */

        case 1:
        case 2:
            return (512);                     /* Maximum size is 512 bytes          */

        case 3:
        case 4:
            return (64);                      /* Maximum size is 64 bytes           */

        case 5:
        case 6:
            return (1024);                    /* Maximum size is 1024 bytes         */

        default:
            return (0);                       /* Non existant endpoint              */
    }
}


/*
 *  USB Device Interrupt enable
 *   Called by USBD_Init to enable the USB Interrupt
 *    Return Value:    None
 */

void USBD_IntrEna(void)
{
    NVIC_EnableIRQ(UDPHS_IRQn);           /* Enable USB interrupt               */
}


/*
 *  USB Device Initialize Function
 *   Called by the User to initialize USB Device
 *    Return Value:    None
 */

void USBD_Init(void)
{
    uint32_t n;
    /* Enables the 48MHz USB Clock UDPCK and System Peripheral USB Clock        */
    PMC->PMC_WPMR = 0x504D4300;                              /* Disable write protect  */
    PMC->PMC_PCER0  = (1 << ID_UDPHS);                       /* enable clock for UPDHS */
    PMC->CKGR_UCKR  = (CKGR_UCKR_UPLLCOUNT(15) | CKGR_UCKR_UPLLEN);

    while (!(PMC->PMC_SR & PMC_SR_LOCKU));                   /* wait until PLL is locked */

    PMC->PMC_WPMR = 0x504D4301;                              /* Enable write protect */
    /* Configure the pull-up on D+ and disconnect it                            */
    UDPHS->UDPHS_CTRL |= UDPHS_CTRL_DETACH;         /* Detach                   */
    UDPHS->UDPHS_CTRL |= UDPHS_CTRL_PULLD_DIS;      /* Disable Pull Down        */
    /* Reset IP UDPHS                                                           */
    UDPHS->UDPHS_CTRL &= ~UDPHS_CTRL_EN_UDPHS;
    UDPHS->UDPHS_CTRL |=  UDPHS_CTRL_EN_UDPHS;
#if (!USBD_HS_ENABLE)                             /* If HS disabled           */
    UDPHS->UDPHS_TST  |= (3 & UDPHS_TST_SPEED_CFG_Msk);
#endif

    /* Disable DMA for UDPHS                                                    */
    for (n = 1; n < (UDPHSDMA_NUMBER); n++) {
        /* RESET endpoint canal DMA:                                              */
        UDPHS->UDPHS_DMA[n].UDPHS_DMACONTROL = 0;     /* STOP command             */
        /* Disable endpoint                                                       */
        UDPHS->UDPHS_EPT[n].UDPHS_EPTCTLDIS  = 0xFFFFFFFF;
        /* Clear status endpoint                                                  */
        UDPHS->UDPHS_EPT[n].UDPHS_EPTCLRSTA  = 0xFFFFFFFF;
        /* Reset endpoint config                                                  */
        UDPHS->UDPHS_EPT[n].UDPHS_EPTCTLENB  = 0;
        /* Reset DMA channel (Buff count and Control field)                       */
        UDPHS->UDPHS_DMA[n].UDPHS_DMACONTROL = (0x1 << 1);    /* NON STOP command */
        /* Reset DMA channel 0 (STOP)                                             */
        UDPHS->UDPHS_DMA[n].UDPHS_DMACONTROL = 0;             /* STOP command     */
        /* Clear DMA channel status (read the register for clear it)              */
        UDPHS->UDPHS_DMA[n].UDPHS_DMASTATUS =  UDPHS->UDPHS_DMA[n].UDPHS_DMASTATUS;
    }

    UDPHS->UDPHS_IEN     = 0;
    UDPHS->UDPHS_CLRINT  = UDPHS_CLRINT_UPSTR_RES |
                           UDPHS_CLRINT_ENDOFRSM  |
                           UDPHS_CLRINT_WAKE_UP   |
                           UDPHS_CLRINT_ENDRESET  |
                           UDPHS_CLRINT_INT_SOF   |
                           UDPHS_CLRINT_MICRO_SOF |
                           UDPHS_CLRINT_DET_SUSPD;
    USBD_IntrEna();                       /* Enable USB interrupt               */
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
        UDPHS->UDPHS_CTRL &= ~UDPHS_CTRL_DETACH;          /* Pull Up on DP        */
        UDPHS->UDPHS_CTRL |=  UDPHS_CTRL_PULLD_DIS;       /* Disable Pull Down    */
    } else {
        UDPHS->UDPHS_CTRL |=  UDPHS_CTRL_DETACH;          /* Detach               */
        UDPHS->UDPHS_CTRL &= ~UDPHS_CTRL_PULLD_DIS;       /* Enable Pull Down     */
    }
}

/*
 *  USB Device Reset Function
 *   Called automatically on USB Device Reset
 *    Return Value:    None
 */
extern U8 USBD_ConfigDescriptor_HS[];
extern U8 USBD_ConfigDescriptor[];
void USBD_Reset(void)
{
    uint32_t ep, EPMask;
    EPMask = ((1 << (USBD_EP_NUM + 1)) - 1);

    /* Reset & Disable USB Endpoints                                            */
    for (ep = 0; ep <= USBD_EP_NUM; ep++) {
        UDPHS->UDPHS_EPT[ep].UDPHS_EPTCFG    =  0;
        UDPHS->UDPHS_EPT[ep].UDPHS_EPTCTLDIS = (0x1 <<  0);     /* Disable EP     */
        eptsta_copy[ep] = 0;
    }

    UDPHS->UDPHS_EPTRST = EPMask;                             /* Reset   EPs    */
    UDPHS->UDPHS_EPTRST = 0;
    /* Setup USB Interrupts */               /* note: Micro_SOF not yet handled */
#ifdef __RTX
    UDPHS->UDPHS_IEN = ((USBD_RTX_DevTask     != 0) ? UDPHS_IEN_DET_SUSPD : 0) |
                       ((USBD_RTX_DevTask     != 0) ? UDPHS_IEN_MICRO_SOF : 0) |
                       ((USBD_RTX_DevTask     != 0) ? UDPHS_IEN_INT_SOF   : 0) |
                       ((USBD_RTX_DevTask     != 0) ? UDPHS_IEN_ENDRESET  : 0) |
//                   ((USBD_RTX_DevTask     != 0) ? UDPHS_IEN_WAKE_UP   : 0) |
//                   ((USBD_RTX_DevTask     != 0) ? UDPHS_IEN_UPSTR_RES : 0) |
#else
    UDPHS->UDPHS_IEN = ((USBD_P_Suspend_Event != 0) ? UDPHS_IEN_DET_SUSPD : 0) |
                       ((USBD_P_SOF_Event     != 0) ? UDPHS_IEN_MICRO_SOF : 0) |
                       ((USBD_P_SOF_Event     != 0) ? UDPHS_IEN_INT_SOF   : 0) |
                       ((USBD_P_Reset_Event   != 0) ? UDPHS_IEN_ENDRESET  : 0) |
//                   ((USBD_P_WakeUp_Event  != 0) ? UDPHS_IEN_WAKE_UP   : 0) |
//                   ((USBD_P_Resume_Event  != 0) ? UDPHS_IEN_UPSTR_RES : 0) |
#endif
                       (EPMask << 8);
    /* Setup Control Endpoint 0                                                 */
    UDPHS->UDPHS_EPT[0].UDPHS_EPTCFG     = UDPHS_EPTCFG_BK_NUMBER_1            |
                                           UDPHS_EPTCFG_EPT_TYPE_CTRL8         |
                                           USBD_CalcSizeEP(USBD_MAX_PACKET0)   ;
    UDPHS->UDPHS_EPT[0].UDPHS_EPTCTLENB  = UDPHS_EPTCTLENB_RXRDY_TXKL          |
                                           UDPHS_EPTCTLENB_TX_COMPLT           |
                                           UDPHS_EPTCTLENB_RX_SETUP            |
                                           UDPHS_EPTCTLENB_STALL_SNT           |
                                           UDPHS_EPTCTLENB_NYET_DIS            |
                                           UDPHS_EPTCTLENB_EPT_ENABL;
                                           
    
#if (USBD_HS_ENABLE == 1)
    U8 * config_desc = USBD_ConfigDescriptor_HS;
#else
    U8 * config_desc = USBD_ConfigDescriptor;
#endif

    while (((USB_ENDPOINT_DESCRIPTOR *)config_desc)->bLength > 0) {
        if (((USB_ENDPOINT_DESCRIPTOR *)config_desc)->bDescriptorType == USB_ENDPOINT_DESCRIPTOR_TYPE) {
            uint32_t num, type, dir, size, banks, interval;
            USB_ENDPOINT_DESCRIPTOR *pEPD =  (USB_ENDPOINT_DESCRIPTOR *)config_desc;
            num      = pEPD->bEndpointAddress & 0x0F;
            type     = pEPD->bmAttributes & USB_ENDPOINT_TYPE_MASK;
            dir      = pEPD->bEndpointAddress >> 7;
            interval = pEPD->bInterval;
            size     = USBD_CalcSizeEP(pEPD->wMaxPacketSize);
            banks    = 1;
            UDPHS->UDPHS_EPT[num].UDPHS_EPTCFG    = (interval << 8) |
                                                    (banks    << 6) |
                                                    (type     << 4) |
                                                    (dir      << 3) |
                                                    (size     << 0) ;
        }
        config_desc += ((USB_ENDPOINT_DESCRIPTOR *)config_desc)->bLength;
    }
}


/*
 *  USB Device Suspend Function
 *   Called automatically on USB Device Suspend
 *    Return Value:    None
 */

void USBD_Suspend(void)
{
    UDPHS->UDPHS_IEN  &= ~UDPHS_IEN_DET_SUSPD;
    UDPHS->UDPHS_IEN  |=  UDPHS_IEN_WAKE_UP;
}


/*
 *  USB Device Resume Function
 *   Called automatically on USB Device Resume
 *    Return Value:    None
 */

void USBD_Resume(void)
{
    UDPHS->UDPHS_IEN  &= ~UDPHS_IEN_WAKE_UP;
    UDPHS->UDPHS_IEN  |=  UDPHS_IEN_DET_SUSPD;
}


/*
 *  USB Device Remote Wakeup Function
 *   Called automatically on USB Device Remote Wakeup
 *    Return Value:    None
 */

void USBD_WakeUp(void)
{
    UDPHS->UDPHS_IEN  |=  UDPHS_IEN_UPSTR_RES;
    UDPHS->UDPHS_CTRL |=  UDPHS_CTRL_REWAKEUP;
}


/*
 *  USB Device Remote Wakeup Configuration Function
 *    Parameters:      cfg:   Device Enable/Disable
 *    Return Value:    None
 */

void USBD_WakeUpCfg(BOOL cfg)
{
    if (cfg) {
        /* Enable wakeup mechanism */
    } else {
        UDPHS->UDPHS_CTRL &= ~UDPHS_CTRL_REWAKEUP;
        UDPHS->UDPHS_IEN  &= ~UDPHS_IEN_UPSTR_RES;
    }
}


/*
 *  USB Device Set Address Function
 *    Parameters:      adr:   USB Device Address
 *                     setup: Called in setup stage (!=0), else after status stage
 *    Return Value:    None
 */

void USBD_SetAddress(uint32_t adr, uint32_t setup)
{
    if (setup) {
        return;
    }

    if (adr) {
        UDPHS->UDPHS_CTRL |= (UDPHS_CTRL_FADDR_EN | adr);
    } else {
        UDPHS->UDPHS_CTRL &= ~(UDPHS_CTRL_FADDR_EN | UDPHS_CTRL_DEV_ADDR_Msk);
    }
}


/*
 *  USB Device Configure Function
 *    Parameters:      cfg:   Device Configure/Deconfigure
 *    Return Value:    None
 */

void USBD_Configure(BOOL cfg)
{
    /* Performed by Hardware */
}


/*
 *  Configure USB Device Endpoint according to Descriptor
 *    Parameters:      pEPD:  Pointer to Device Endpoint Descriptor
 *    Return Value:    None
 */

void USBD_ConfigEP(USB_ENDPOINT_DESCRIPTOR *pEPD)
{
    uint32_t num;//, type, dir, size, banks, interval;
    num      = pEPD->bEndpointAddress & 0x0F;
    /*type     = pEPD->bmAttributes & USB_ENDPOINT_TYPE_MASK;
    dir      = pEPD->bEndpointAddress >> 7;
    interval = pEPD->bInterval;
    size     = USBD_CalcSizeEP(pEPD->wMaxPacketSize);
    banks    = 1;
    */
    /* Check if MaxPacketSize fits for EndPoint                                 */
    if (pEPD->wMaxPacketSize <= USBD_GetSizeEP(num)) {
        /*UDPHS->UDPHS_EPT[num].UDPHS_EPTCFG    = (interval << 8) |
                                                (banks    << 6) |
                                                (type     << 4) |
                                                (dir      << 3) |
                                                //(size     << 0) ;
                                                 6;*/
        UDPHS->UDPHS_EPT[num].UDPHS_EPTCTLENB =
            (0x1 <<  9) |  /* Received OUT Data Interrupt Enable                    */
            (0x1 << 10) |  /* Transmitted IN Data Complete Interrupt Enable         */
            (0x0 <<  4) |  /* NYET Disable (Only for High Speed Bulk OUT endpoints) */
            (0x1 << 13) ;  /* Stall Sent /ISO CRC Error/Number of Transaction Error */
    }
}


/*
 *  Set Direction for USB Device Control Endpoint
 *    Parameters:      dir:   Out (dir == 0), In (dir <> 0)
 *    Return Value:    None
 */

void USBD_DirCtrlEP(uint32_t dir)
{
    /* Performed by Hardware */
}


/*
 *  Enable USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_EnableEP(uint32_t EPNum)
{
    UDPHS->UDPHS_EPT[EPNum & 0x0F].UDPHS_EPTCTLENB = (0x1 << 0);  /* EP Enable  */
    eptsta_copy[EPNum & 0x0F] = UDPHS->UDPHS_EPT[EPNum].UDPHS_EPTSETSTA;
}


/*
 *  Disable USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_DisableEP(uint32_t EPNum)
{
    UDPHS->UDPHS_EPT[EPNum & 0x0F].UDPHS_EPTCTLDIS = (0x1 << 0);  /* EP Disable */
    eptsta_copy[EPNum & 0x0F] = UDPHS->UDPHS_EPT[EPNum].UDPHS_EPTSETSTA;
}


/*
 *  Reset USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_ResetEP(uint32_t EPNum)
{
    EPNum &= 0x0F;
    UDPHS->UDPHS_EPT[EPNum].UDPHS_EPTCLRSTA = (0x1 << 6) |  /* Data Toggle Clear*/
            (0x1 << 5);   /* Stall Req Set    */
    UDPHS->UDPHS_EPTRST |= (1 << EPNum);                    /* Reset endpoint   */
    UDPHS->UDPHS_EPTRST &= ~(1 << EPNum);
    eptsta_copy[EPNum] = UDPHS->UDPHS_EPT[EPNum].UDPHS_EPTSETSTA;
}


/*
 *  Set Stall for USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_SetStallEP(uint32_t EPNum)
{
    UDPHS->UDPHS_EPT[EPNum & 0x0F].UDPHS_EPTSETSTA = (0x1 << 5);  /* Stall Set  */
    eptsta_copy[EPNum & 0x0F] |= 0x1 << 5;
}


/*
 *  Clear Stall for USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_ClrStallEP(uint32_t EPNum)
{
    UDPHS->UDPHS_EPT[EPNum & 0x0F].UDPHS_EPTCLRSTA = (0x1 << 6) | /* Clr Toggle */
            (0x1 << 5);  /* Stall Clear*/
    eptsta_copy[EPNum & 0x0F] &= ~(0x1 << 5);
}


/*
 *  Read USB Device Endpoint Data
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

uint32_t USBD_ReadEP(uint32_t EPNum, uint8_t *pData, uint32_t size)
{
    uint32_t cnt, n, copy_sz;
    uint8_t *pEPFIFO;                                /* Pointer to EP FIFO           */
    uint32_t eptsta;
    EPNum  &= 0x0F;
    eptsta = eptsta_copy[EPNum];
    pEPFIFO = (uint8_t *)((uint32_t *)UDPHS_EPTFIFO_BASE + (16384 * EPNum));
    cnt     = (eptsta >> 20) & 0x07FF;  /* Get by */
    copy_sz = cnt > size ? size : cnt;

    for (n = 0; n < copy_sz; n++) {
        *pData++ = *pEPFIFO++;
    }

    util_assert(cnt == copy_sz);

    if ((cnt == copy_sz) && (eptsta & (0x1 << 9))) {
        UDPHS->UDPHS_EPT[EPNum].UDPHS_EPTCLRSTA = (0x1 << 9);   /* Rece OUT Clear   */
    }

    /* RX_Setup must be cleared after Setup packet is read                      */
    if (eptsta & (0x1 << 12)) {
        UDPHS->UDPHS_EPT[EPNum].UDPHS_EPTCLRSTA = (0x1 << 12);  /* Rece SETUP Clear */
    }
    UDPHS->UDPHS_IEN |= (1 << (EPNum + 8));     /* Enable EP int after data read*/
    return (cnt);
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

uint32_t USBD_WriteEP(uint32_t EPNum, uint8_t *pData, uint32_t cnt)
{
    uint32_t n;
    uint8_t *pEPFIFO;                          /* Pointer to the endpoint FIFO       */
    uint32_t eptsta;
    EPNum &= 0x0F;
    eptsta = eptsta_copy[EPNum];

    // Cached value should match the real value
    util_assert((UDPHS->UDPHS_EPT[EPNum].UDPHS_EPTSTA & (0x1 << 5)) == (eptsta & (0x1 << 5)));
    if (eptsta & (0x1 << 5)) {  /* If EP is stall */
        return (cnt);
    }

    // Both register and cached value should indicate that the bank is ready (bit 11 clear)
    util_assert(!(UDPHS->UDPHS_EPT[EPNum].UDPHS_EPTSTA & (0x1 << 11)));
    util_assert(!(eptsta & (0x1 << 11)));

    pEPFIFO = (uint8_t *)((uint32_t *)UDPHS_EPTFIFO_BASE + (16384 * EPNum));

    for (n = 0; n < cnt; n++) {
        *pEPFIFO++ = *pData++;              /* Write data to FIFO                 */
    }

    UDPHS->UDPHS_EPT[EPNum].UDPHS_EPTSETSTA = (0x1 << 11);  /* Set packet ready */
    return (cnt);
}


/*
 *  Get USB Device Last Frame Number
 *    Parameters:      None
 *    Return Value:    Frame Number
 */

uint32_t USBD_GetFrame(void)
{
    uint32_t val;

    if ((UDPHS->UDPHS_FNUM & (1UL << 31)) == 0) {
        if (USBD_HighSpeed) {
            val = UDPHS->UDPHS_FNUM & 0x7FFFFFFF;
        } else {
            val = (UDPHS->UDPHS_FNUM & UDPHS_FNUM_FRAME_NUMBER_Msk) >> 3;
        }
    } else {
        val = 0xFFFFFFFF;
    }

    return (val);
}


#ifdef __RTX
uint32_t LastError;                          /* Last Error                         */

/*
 *  Get USB Device Last Error Code
 *    Parameters:      None
 *    Return Value:    Error Code
 */

uint32_t USBD_GetError(void)
{
    return (LastError);
}
#endif


/*
 *  USB Device Interrupt Service Routine
 */

void UDPHS_IRQHandler(void)
{
    NVIC_DisableIRQ(UDPHS_IRQn);
    USBD_SignalHandler();
}

/*
 *  USB Device Service Routine
 */

void USBD_Handler(void)
{
    uint32_t intsta, eptsta, n;
    intsta = UDPHS->UDPHS_INTSTA & UDPHS->UDPHS_IEN;

    /* End of Bus Reset Interrupt                                               */
    if (intsta & UDPHS_INTSTA_ENDRESET) {
        /* Get used speed (HighSpeed or FullSpeed)                                */
        USBD_HighSpeed = (UDPHS->UDPHS_INTSTA & UDPHS_INTSTA_SPEED) ? 1 : 0;
        USBD_Reset();
        usbd_reset_core();
#ifdef __RTX
        UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_ENDRESET;

        if (USBD_RTX_DevTask) {
            isr_evt_set(USBD_EVT_RESET, USBD_RTX_DevTask);
        }

#else

        if (USBD_P_Reset_Event) {
            USBD_P_Reset_Event();
        }

        UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_ENDRESET;
#endif
    }

    /* USB Suspend Interrupt                                                    */
    if (intsta & UDPHS_INTSTA_DET_SUSPD) {
        USBD_Suspend();
#ifdef __RTX
        UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_DET_SUSPD;

        if (USBD_RTX_DevTask) {
            isr_evt_set(USBD_EVT_SUSPEND, USBD_RTX_DevTask);
        }

#else

        if (USBD_P_Suspend_Event) {
            USBD_P_Suspend_Event();
        }

        UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_DET_SUSPD;
#endif
    }

    /* USB Resume Interrupt                                                     */
    if (intsta & UDPHS_INTSTA_WAKE_UP) {
        USBD_Resume();
#ifdef __RTX
        UDPHS->UDPHS_CLRINT = UDPHS_INTSTA_WAKE_UP;

        if (USBD_RTX_DevTask) {
            isr_evt_set(USBD_EVT_RESUME,  USBD_RTX_DevTask);
        }

#else

        if (USBD_P_Resume_Event) {
            USBD_P_Resume_Event();
        }

        UDPHS->UDPHS_CLRINT = UDPHS_INTSTA_WAKE_UP;
#endif
    }

    /* USB Remote Wakeup Interrupt                                              */
    if (intsta & UDPHS_INTSTA_UPSTR_RES) {
        UDPHS->UDPHS_CLRINT = UDPHS_INTSTA_UPSTR_RES;
    }

    /* Start of Frame Interrupt                                                 */
    if (intsta & UDPHS_INTSTA_INT_SOF) {
        /* Process the SOF interrupt even in high speed mode.
        The SOF and MICRO_SOF interrupt are never generated at the same
        time. Instead, when in high speed mode there is 1 SOF
        interrupt and 7 MICRO_SOF interrupts every 1ms. */

#ifdef __RTX
        UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_INT_SOF;

        if (USBD_RTX_DevTask) {
            isr_evt_set(USBD_EVT_SOF, USBD_RTX_DevTask);
        }

#else

        if (USBD_P_SOF_Event) {
            USBD_P_SOF_Event();
        }

        UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_INT_SOF;
#endif
    }

    /* Micro Frame Interrupt                                                    */
    if (intsta & UDPHS_INTSTA_MICRO_SOF) {
        if (USBD_HighSpeed == 1) {
#ifdef __RTX
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_MICRO_SOF;

            if (USBD_RTX_DevTask) {
                isr_evt_set(USBD_EVT_SOF, USBD_RTX_DevTask);
            }

#else

            if (USBD_P_SOF_Event) {
                USBD_P_SOF_Event();
            }

            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_MICRO_SOF;
#endif

        } else {
            UDPHS->UDPHS_CLRINT = UDPHS_CLRINT_MICRO_SOF;
        }
    }

    /* Endpoint Interrupts                                                      */
    for (n = 0; n <= USBD_EP_NUM; n++) {
        if (intsta & (1 << (n + 8))) {
            eptsta = UDPHS->UDPHS_EPT[n].UDPHS_EPTSTA;  /* Read EP status           */
            eptsta_copy[n] = eptsta;

            /* Data Packet Sent Interrupt                                           */
            if (eptsta & (1 << 10)) {         /* Transmitted IN Data Complete Int   */
                UDPHS->UDPHS_EPT[n].UDPHS_EPTCLRSTA = (1 << 10);    /* Tx IN Clear    */
#ifdef __RTX

                if (USBD_RTX_EPTask[n]) {       /* IN Packet                          */
                    isr_evt_set(USBD_EVT_IN,  USBD_RTX_EPTask[n]);
                }

#else

                if (USBD_P_EP[n]) {
                    USBD_P_EP[n](USBD_EVT_IN);
                }

#endif
            }

            /* Data Packet Received Interrupt                                       */
            if (eptsta & (1 << 9)) {          /* Received OUT Data Interrupt        */
                UDPHS->UDPHS_IEN &= ~(1 << (n + 8));      /* Disable EP int until read*/
#ifdef __RTX

                if (USBD_RTX_EPTask[n]) {       /* OUT Packet                         */
                    isr_evt_set(USBD_EVT_OUT, USBD_RTX_EPTask[n]);
                }

#else

                if (USBD_P_EP[n]) {
                    USBD_P_EP[n](USBD_EVT_OUT);
                }

#endif
            }

            /* STALL Packet Sent Interrupt                                          */
            if (eptsta & (0x1 << 13)) {       /* Stall Sent                         */
                if ((UDPHS->UDPHS_EPT[n].UDPHS_EPTCFG & UDPHS_EPTCFG_EPT_TYPE_Msk) == UDPHS_EPTCFG_EPT_TYPE_CTRL8) {
#ifdef __RTX

                    if (USBD_RTX_EPTask[n]) {
                        isr_evt_set(USBD_EVT_IN_STALL, USBD_RTX_EPTask[n]);
                    }

#else

                    if (USBD_P_EP[n]) {
                        USBD_P_EP[n](USBD_EVT_IN_STALL);
                    }

#endif
                }

                UDPHS->UDPHS_EPT[n].UDPHS_EPTCLRSTA = UDPHS_EPTCLRSTA_STALL_SNT;
            }

            /* Setup Packet Received Interrupt                                      */
            if (eptsta & (0x1 << 12)) {       /* Received SETUP Interrupt           */
                UDPHS->UDPHS_IEN &= ~(1 << (n + 8));      /* Disable EP int until read*/
#ifdef __RTX

                if (USBD_RTX_EPTask[n]) {       /* SETUP Packet                       */
                    isr_evt_set(USBD_EVT_SETUP, USBD_RTX_EPTask[n]);
                }

#else

                if (USBD_P_EP[n]) {
                    USBD_P_EP[n](USBD_EVT_SETUP);
                }

#endif
            }
        }
    }

    NVIC_EnableIRQ(UDPHS_IRQn);
}
