/**
 * @file    usbd_LPC43xx_USBD0.c
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
#include "usb.h"
#include "LPC43xx.h"
#include "compiler.h"

#define __NO_USB_LIB_C
#include "usb_config.c"

/* Endpoint queue head */
typedef struct __EPQH {
    uint32_t cap;
    uint32_t curr_dTD;
    uint32_t next_dTD;
    uint32_t dTD_token;
    uint32_t buf[5];
    uint32_t reserved;
    uint32_t setup[2];
    uint32_t reserved1[4];
} EPQH;

/* Endpoint transfer descriptor */
typedef struct __dTD {
    uint32_t next_dTD;
    uint32_t dTD_token;
    uint32_t buf[5];
    uint32_t reserved;
} dTD;

/* Endpoint */
typedef struct __EP {
    uint8_t *buf;
    uint32_t maxPacket;
} EP;

EPQH __align(2048) EPQHx[(USBD_EP_NUM + 1) * 2];
dTD  __align(32) dTDx[(USBD_EP_NUM + 1) * 2];

EP Ep[(USBD_EP_NUM + 1) * 2];
uint32_t BufUsed;
uint32_t IsoEp;
uint32_t cmpl_pnd;

#define LPC_USBx LPC_USB0
#define ENDPTCTRL(EPNum) *(volatile uint32_t *)((uint32_t)(&LPC_USBx->ENDPTCTRL0) + 4 * EPNum)
#define EP_OUT_IDX(EPNum) (EPNum * 2    )
#define EP_IN_IDX(EPNum) (EPNum * 2 + 1)
#define HS(en) (USBD_HS_ENABLE * en)

/* reserve RAM for endpoint buffers */
#if USBD_VENDOR_ENABLE
/* custom class: user defined buffer size */
#define EP_BUF_POOL_SIZE 0x1000
uint8_t __align(4096) EPBufPool[EP_BUF_POOL_SIZE]
#else
/* supported classes are used */
uint8_t __align(4096) EPBufPool[
    USBD_MAX_PACKET0                                                                                                     * 2 +
    USBD_HID_ENABLE     *  (HS(USBD_HID_HS_ENABLE)      ? USBD_HID_HS_WMAXPACKETSIZE     : USBD_HID_WMAXPACKETSIZE)      * 2 +
    USBD_MSC_ENABLE     *  (HS(USBD_MSC_HS_ENABLE)      ? USBD_MSC_HS_WMAXPACKETSIZE     : USBD_MSC_WMAXPACKETSIZE)      * 2 +
    USBD_ADC_ENABLE     *  (HS(USBD_ADC_HS_ENABLE)      ? USBD_ADC_HS_WMAXPACKETSIZE     : USBD_ADC_WMAXPACKETSIZE)          +
    USBD_CDC_ACM_ENABLE * ((HS(USBD_CDC_ACM_HS_ENABLE) ? USBD_CDC_ACM_HS_WMAXPACKETSIZE  : USBD_CDC_ACM_WMAXPACKETSIZE)      +
                           (HS(USBD_CDC_ACM_HS_ENABLE) ? USBD_CDC_ACM_HS_WMAXPACKETSIZE1 : USBD_CDC_ACM_WMAXPACKETSIZE1) * 2) + 
    USBD_BULK_ENABLE     *  (HS(USBD_BULK_HS_ENABLE)      ? USBD_BULK_HS_WMAXPACKETSIZE     : USBD_BULK_WMAXPACKETSIZE)      * 2 
];
#endif

void USBD_PrimeEp(uint32_t EPNum, uint32_t cnt);

/*
 *  Usb interrupt enable/disable
 *    Parameters:      ena: enable/disable
 *                       0: disable interrupt
 *                       1: enable interrupt
 */

#ifdef __RTX
void __svc(1) USBD_Intr(int ena);
void __SVC_1(int ena)
{
#else
void USBD_Intr(int ena)
{
#endif

    if (ena) {
        NVIC_EnableIRQ(USB0_IRQn);          /* Enable USB interrupt */
    } else {
        NVIC_DisableIRQ(USB0_IRQn);         /* Disable USB interrupt */
    }
}


/*
 *  USB Device Initialize Function
 *   Called by the User to initialize USB Device
 *    Return Value:    None
 */

void USBD_Init(void)
{
    USBD_Intr(0);
    /* BASE_USB0_CLK */
    LPC_CGU->BASE_USB0_CLK =    (0x01 << 11) |       /* Autoblock En */
                                (0x07 << 24) ;       /* Clock source: PLL0 */
    LPC_CCU1->CLK_M4_USB0_CFG |= 1;

    while (!(LPC_CCU1->CLK_M4_USB0_STAT & 1));

    LPC_SCU->SFSP6_3 = 1;                 /* pwr en */
    LPC_SCU->SFSP6_6 = 3;                 /* pwr fault */
    LPC_SCU->SFSP8_1 = 1;                 /* port indicator LED control out 1 */
    LPC_SCU->SFSP8_2 = 1;                 /* port indicator LED control out 0 */
    LPC_USBx->USBCMD_D |= (1UL << 1);     /* usb reset */

    while (LPC_USBx->USBCMD_D & (1UL << 1));

    LPC_CREG->CREG0 &= ~(1 << 5);
    LPC_USBx->USBMODE_D  = 2 | (1UL << 3);/* device mode */
#if USBD_HS_ENABLE
    LPC_USBx->PORTSC1_D &= ~(1UL << 24);
#else
    LPC_USBx->PORTSC1_D |= (1UL << 24);
#endif
    LPC_USBx->OTGSC = 1 | (1UL << 3);
    Ep[EP_OUT_IDX(0)].maxPacket = USBD_MAX_PACKET0;
    LPC_USBx->USBINTR_D  = (1UL << 0) |   /* usb int enable */
                           (1UL << 2) |   /* port change detect int enable */
                           (1UL << 8) |   /* suspend int enable */
                           (1UL << 16) |  /* nak int enable */
                           (1UL << 6) |   /* reset int enable */
#ifdef __RTX
                           ((USBD_RTX_DevTask   != 0) ? (1UL << 7) : 0) |   /* SOF */
                           ((USBD_RTX_DevTask   != 0) ? (1UL << 1) : 0) ;   /* Error */
#else
                           ((USBD_P_SOF_Event   != 0) ? (1UL << 7) : 0) |   /* SOF */
                           ((USBD_P_Error_Event != 0) ? (1UL << 1) : 0) ;   /* Error */
#endif
    USBD_Reset();
    USBD_Intr(1);
}


/*
 *  USB Device Connect Function
 *   Called by the User to Connect/Disconnect USB Device
 *    Parameters:      con:   Connect/Disconnect
 *    Return Value:    None
 */

void USBD_Connect(uint32_t con)
{
    if (con) {
        LPC_USBx->USBCMD_D |= 1;            /* run */
    } else {
        LPC_USBx->USBCMD_D &= ~1;           /* stop */
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
    uint8_t *ptr;
    cmpl_pnd = 0;

    for (i = 1; i < USBD_EP_NUM + 1; i++) {
        ENDPTCTRL(i) &= ~((1UL << 7) | (1UL << 23));
    }

    /* clear interrupts */
    LPC_USBx->ENDPTNAK       = 0xFFFFFFFF;
    LPC_USBx->ENDPTNAKEN     = 0;
    LPC_USBx->USBSTS_D       = 0xFFFFFFFF;
    LPC_USBx->ENDPTSETUPSTAT = LPC_USBx->ENDPTSETUPSTAT;
    LPC_USBx->ENDPTCOMPLETE  = LPC_USBx->ENDPTCOMPLETE;

    while (LPC_USBx->ENDPTPRIME);

    LPC_USBx->ENDPTFLUSH = 0xFFFFFFFF;

    while (LPC_USBx->ENDPTFLUSH);

    LPC_USBx->USBCMD_D &= ~0x00FF0000;    /* immediate intrrupt treshold */
    /* clear endpoint queue heads */
    ptr = (uint8_t *)EPQHx;

    for (i = 0; i < sizeof(EPQHx); i++) {
        ptr[i] = 0;
    }

    /* clear endpoint transfer descriptors */
    ptr = (uint8_t *)dTDx;

    for (i = 0; i < sizeof(dTDx); i++) {
        ptr[i] = 0;
    }

    Ep[EP_OUT_IDX(0)].maxPacket  = USBD_MAX_PACKET0;
    Ep[EP_OUT_IDX(0)].buf        = EPBufPool;
    BufUsed                      = USBD_MAX_PACKET0;
    Ep[EP_IN_IDX(0)].maxPacket   = USBD_MAX_PACKET0;
    Ep[EP_IN_IDX(0)].buf         = &(EPBufPool[BufUsed]);
    BufUsed                     += USBD_MAX_PACKET0;
    dTDx[EP_OUT_IDX(0)].next_dTD = 1;
    dTDx[EP_IN_IDX(0)].next_dTD = 1;
    dTDx[EP_OUT_IDX(0)].dTD_token = (USBD_MAX_PACKET0 << 16) | /* total bytes */
                                    (1UL << 15);               /* int on compl */
    dTDx[EP_IN_IDX(0)].dTD_token = (USBD_MAX_PACKET0 << 16) |  /* total bytes */
                                   (1UL << 15);                /* int on compl */
    EPQHx[EP_OUT_IDX(0)].next_dTD = (uint32_t) &dTDx[EP_OUT_IDX(0)];
    EPQHx[EP_IN_IDX(0)].next_dTD = (uint32_t) &dTDx[EP_IN_IDX(0)];
    EPQHx[EP_OUT_IDX(0)].cap = ((USBD_MAX_PACKET0 & 0x0EFF) << 16) |
                               (1UL << 29) |
                               (1UL << 15);                    /* int on setup */
    EPQHx[EP_IN_IDX(0)].cap = (USBD_MAX_PACKET0 << 16) |
                              (1UL << 29) |
                              (1UL << 15);                    /* int on setup */
    LPC_USBx->ENDPOINTLISTADDR = (uint32_t)EPQHx;
    LPC_USBx->USBMODE_D |= (1UL << 3);    /* Setup lockouts off */
    LPC_USBx->ENDPTCTRL0 = 0x00C000C0;
    USBD_PrimeEp(0, Ep[EP_OUT_IDX(0)].maxPacket);
}


/*
 *  USB Device Suspend Function
 *   Called automatically on USB Device Suspend
 *    Return Value:    None
 */

void USBD_Suspend(void)
{
    /* Performed by Hardware */
}


/*
 *  USB Device Resume Function
 *   Called automatically on USB Device Resume
 *    Return Value:    None
 */

void USBD_Resume(void)
{
    /* Performed by Hardware */
}


/*
 *  USB Device Remote Wakeup Function
 *   Called automatically on USB Device Remote Wakeup
 *    Return Value:    None
 */

void USBD_WakeUp(void)
{
    LPC_USBx->PORTSC1_D |= (1UL << 6);
}


/*
 *  USB Device Remote Wakeup Configuration Function
 *    Parameters:      cfg:   Device Enable/Disable
 *    Return Value:    None
 */

void USBD_WakeUpCfg(uint32_t cfg)
{
    /* Not needed */
}


/*
 *  USB Device Set Address Function
 *    Parameters:      adr:   USB Device Address
 *    Return Value:    None
 */

void USBD_SetAddress(uint32_t adr, uint32_t setup)
{
    if (setup == 0) {
        LPC_USBx->DEVICEADDR  = (adr << 25);
        LPC_USBx->DEVICEADDR |= (1UL << 24);
    }
}


/*
 *  USB Device Configure Function
 *    Parameters:      cfg:   Device Configure/Deconfigure
 *    Return Value:    None
 */

void USBD_Configure(uint32_t cfg)
{
    uint32_t i;

    if (!cfg) {
        for (i = 2; i < (2 * (USBD_EP_NUM + 1)); i++) {
            Ep[i].buf = 0;
            Ep[i].maxPacket = 0;
        }

        BufUsed = 2 * USBD_MAX_PACKET0;
    }
}


/*
 *  Configure USB Device Endpoint according to Descriptor
 *    Parameters:      pEPD:  Pointer to Device Endpoint Descriptor
 *    Return Value:    None
 */

void USBD_ConfigEP(USB_ENDPOINT_DESCRIPTOR *pEPD)
{
    uint32_t num, val, type, idx;

    if ((pEPD->bEndpointAddress & USB_ENDPOINT_DIRECTION_MASK)) {
        val = 16;
        num = pEPD->bEndpointAddress & ~0x80;
        idx = EP_IN_IDX(num);

    } else {
        val = 0;
        num = pEPD->bEndpointAddress;
        idx = EP_OUT_IDX(num);
    }

    type = pEPD->bmAttributes & USB_ENDPOINT_TYPE_MASK;

    if (!(Ep[idx].buf)) {
        Ep[idx].buf          =  &(EPBufPool[BufUsed]);
        Ep[idx].maxPacket    =  pEPD->wMaxPacketSize;
        BufUsed             +=  pEPD->wMaxPacketSize;

        /* Isochronous endpoint */
        if (type == USB_ENDPOINT_TYPE_ISOCHRONOUS) {
            IsoEp |= (1UL << (num + val));
        }
    }

    dTDx[idx].buf[0]    = (uint32_t)(Ep[idx].buf);
    dTDx[idx].next_dTD  =  1;
    EPQHx[idx].cap      = (Ep[idx].maxPacket << 16) |
                          (1UL               << 29);
    ENDPTCTRL(num)  &= ~(0xFFFF << val);
    ENDPTCTRL(num)  |= ((type << 2) << val) |
                       ((1UL  << 6) << val);   /* Data toogle reset */
}


/*
 *  Set Direction for USB Device Control Endpoint
 *    Parameters:      dir:   Out (dir == 0), In (dir <> 0)
 *    Return Value:    None
 */

void USBD_DirCtrlEP(uint32_t dir)
{
    /* Not needed */
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
    if (EPNum & 0x80) {
        EPNum &= 0x7F;
        ENDPTCTRL(EPNum) |= (1UL << 23);    /* EP enabled */
    } else {
        ENDPTCTRL(EPNum) |= (1UL << 7);     /* EP enabled */
    }
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
    if (EPNum & 0x80) {
        EPNum &= 0x7F;
        ENDPTCTRL(EPNum) &= ~(1UL << 23);   /* EP disabled */
    } else {
        ENDPTCTRL(EPNum)     &= ~(1UL << 7); /* EP disabled */
    }
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
    if (EPNum & 0x80) {
        EPNum &= 0x7F;
        EPQHx[EP_IN_IDX(EPNum)].dTD_token &= 0xC0;
        LPC_USBx->ENDPTFLUSH = (1UL << (EPNum + 16));  /* flush endpoint */

        while (LPC_USBx->ENDPTFLUSH & (1UL << (EPNum + 16)));

        ENDPTCTRL(EPNum) |= (1UL << 22);    /* data toggle reset */

    } else {
        EPQHx[EP_OUT_IDX(EPNum)].dTD_token &= 0xC0;
        LPC_USBx->ENDPTFLUSH = (1UL << EPNum);         /* flush endpoint */

        while (LPC_USBx->ENDPTFLUSH & (1UL << EPNum));

        ENDPTCTRL(EPNum) |= (1UL << 6);     /* data toggle reset */
        USBD_PrimeEp(EPNum, Ep[EP_OUT_IDX(EPNum)].maxPacket);
    }
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
    if (EPNum & 0x80) {
        EPNum &= 0x7F;
        ENDPTCTRL(EPNum) |= (1UL << 16);    /* IN endpoint stall */
    } else {
        ENDPTCTRL(EPNum) |= (1UL << 0);     /* OUT endpoint stall */
    }
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
    if (EPNum & 0x80) {
        EPNum &= 0x7F;
        ENDPTCTRL(EPNum) &= ~(1UL << 16);   /* clear stall */
        ENDPTCTRL(EPNum) |= (1UL << 22);    /* data toggle reset */

        while (ENDPTCTRL(EPNum) & (1UL << 16));

        USBD_ResetEP(EPNum | 0x80);

    } else {
        ENDPTCTRL(EPNum) &= ~(1UL << 0);    /* clear stall */
        ENDPTCTRL(EPNum) |= (1UL << 6);     /* data toggle reset */
    }
}


/*
 *  Clear USB Device Endpoint Buffer
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_ClearEPBuf(uint32_t EPNum)
{
}


/*
 *  USB Device Prime endpoint function
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     cnt:   Bytes to transfer/receive
 *    Return Value:    None
 */

void USBD_PrimeEp(uint32_t EPNum, uint32_t cnt)
{
    uint32_t idx, val;

    /* IN endpoint */
    if (EPNum & 0x80) {
        EPNum &= 0x7F;
        idx    = EP_IN_IDX(EPNum);
        val    = (1UL << (EPNum + 16));
    }

    /* OUT endpoint */
    else {
        val    = (1UL << EPNum);
        idx    = EP_OUT_IDX(EPNum);
    }

    dTDx[idx].buf[0]    = (uint32_t)(Ep[idx].buf);
    dTDx[idx].next_dTD  = 1;

    if (IsoEp & val) {
        if (Ep[idx].maxPacket <= cnt) {
            dTDx[idx].dTD_token = (1 << 10);             /* MultO = 1 */

        } else if ((Ep[idx].maxPacket * 2) <= cnt) {
            dTDx[idx].dTD_token = (2 << 10);             /* MultO = 2 */

        } else {
            dTDx[idx].dTD_token = (3 << 10);             /* MultO = 3 */
        }

    } else {
        dTDx[idx].dTD_token = 0;
    }

    dTDx[idx].dTD_token  |= (cnt << 16) |         /* bytes to transfer */
                            (1UL << 15) |         /* int on complete */
                            0x80;                 /* status - active */
    EPQHx[idx].next_dTD   = (uint32_t)(&dTDx[idx]);
    EPQHx[idx].dTD_token &= ~0xC0;
    LPC_USBx->ENDPTPRIME = (val);

    while ((LPC_USBx->ENDPTPRIME & val));
}


/*
 *  Read USB Device Endpoint Data
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

uint32_t USBD_ReadEP(uint32_t EPNum, uint8_t *pData, U32 size)
{
    uint32_t cnt  = 0;
    uint32_t i;

    /* Setup packet */
    if ((LPC_USBx->ENDPTSETUPSTAT & 1) && (!EPNum)) {
        LPC_USBx->ENDPTSETUPSTAT = 1;

        while (LPC_USBx->ENDPTSETUPSTAT & 1);

        do {
            *((__packed uint32_t *) pData)      = EPQHx[EP_OUT_IDX(0)].setup[0];
            *((__packed uint32_t *)(pData + 4)) = EPQHx[EP_OUT_IDX(0)].setup[1];
            cnt = 8;
            LPC_USBx->USBCMD_D |= (1UL << 13);
        } while (!(LPC_USBx->USBCMD_D & (1UL << 13)));

        LPC_USBx->USBCMD_D &= (~(1UL << 13));
        LPC_USBx->ENDPTFLUSH = (1UL << EPNum) | (1UL << (EPNum + 16));

        while (LPC_USBx->ENDPTFLUSH & ((1UL << (EPNum + 16)) | (1UL << EPNum)));

        while (LPC_USBx->ENDPTSETUPSTAT & 1);

        USBD_PrimeEp(EPNum, Ep[EP_OUT_IDX(EPNum)].maxPacket);
    }

    /* OUT Packet */
    else {
        if (Ep[EP_OUT_IDX(EPNum)].buf) {
            cnt = Ep[EP_OUT_IDX(EPNum)].maxPacket -
                  ((dTDx[EP_OUT_IDX(EPNum)].dTD_token >> 16) & 0x7FFF);

            cnt = cnt < size ? cnt : size;

            for (i = 0; i < cnt; i++) {
                pData[i] =  Ep[EP_OUT_IDX(EPNum)].buf[i];
            }
        }

        LPC_USBx->ENDPTCOMPLETE = (1UL << EPNum);
        cmpl_pnd &= ~(1UL << EPNum);
        USBD_PrimeEp(EPNum, Ep[EP_OUT_IDX(EPNum)].maxPacket);
    }

    return (cnt);
}


/*
 *  Write USB Device Endpoint Data
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *                     cnt:   Number of bytes to write
 *    Return Value:    Number of bytes written
 */

uint32_t USBD_WriteEP(uint32_t EPNum, uint8_t *pData, uint32_t cnt)
{
    uint32_t i;
    EPNum &= 0x7f;

    for (i = 0; i < cnt; i++) {
        Ep[EP_IN_IDX(EPNum)].buf[i] = pData[i];
    }

    USBD_PrimeEp(EPNum | 0x80, cnt);
    return (cnt);
}


/*
 *  Get USB Device Last Frame Number
 *    Parameters:      None
 *    Return Value:    Frame Number
 */

uint32_t USBD_GetFrame(void)
{
    return ((LPC_USBx->FRINDEX_D >> 3) & 0x0FFF);
}


#ifdef __RTX
uint32_t LastError;                     /* Last Error */

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

void USB0_IRQHandler(void)
{
    NVIC_DisableIRQ(USB0_IRQn);
    USBD_SignalHandler();
}

void USBD_Handler(void)
{
    uint32_t sts, cmpl, num;
    sts  = LPC_USBx->USBSTS_D & LPC_USBx->USBINTR_D;
    cmpl = LPC_USBx->ENDPTCOMPLETE;
    LPC_USBx->USBSTS_D = sts;                     /* clear interupt flags */

    /* reset interrupt */
    if (sts & (1UL << 6)) {
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
    }

    /* suspend interrupt */
    if (sts & (1UL << 8)) {
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
    }

    /* SOF interrupt */
    if (sts & (1UL << 7)) {
        if (IsoEp) {
            for (num = 0; num < USBD_EP_NUM + 1; num++) {
                if (IsoEp & (1UL << num)) {
                    USBD_PrimeEp(num, Ep[EP_OUT_IDX(num)].maxPacket);
                }
            }

        } else {
#ifdef __RTX

            if (USBD_RTX_DevTask) {
                isr_evt_set(USBD_EVT_SOF, USBD_RTX_DevTask);
            }

#else

            if (USBD_P_SOF_Event) {
                USBD_P_SOF_Event();
            }

#endif
        }
    }

    /* port change detect interrupt */
    if (sts & (1UL << 2)) {
        if (((LPC_USBx->PORTSC1_D >> 26) & 0x03) == 2) {
            USBD_HighSpeed = __TRUE;
        }

        USBD_Resume();
#ifdef __RTX

        if (USBD_RTX_DevTask) {
            isr_evt_set(USBD_EVT_RESUME,  USBD_RTX_DevTask);
        }

#else

        if (USBD_P_Resume_Event) {
            USBD_P_Resume_Event();
        }

#endif
    }

    /* USB interrupt - completed transfer */
    if (sts & 1) {
        /* Setup Packet */
        if (LPC_USBx->ENDPTSETUPSTAT) {
#ifdef __RTX

            if (USBD_RTX_EPTask[0]) {
                isr_evt_set(USBD_EVT_SETUP, USBD_RTX_EPTask[0]);
            }

#else

            if (USBD_P_EP[0]) {
                USBD_P_EP[0](USBD_EVT_SETUP);
            }

#endif
        }

        /* IN Packet */
        if (cmpl & (0x3F << 16)) {
            for (num = 0; num < USBD_EP_NUM + 1; num++) {
                if (((cmpl >> 16) & 0x3F) & (1UL << num)) {
                    LPC_USBx->ENDPTCOMPLETE = (1UL << (num + 16));    /* Clear completed */
#ifdef __RTX

                    if (USBD_RTX_EPTask[num]) {
                        isr_evt_set(USBD_EVT_IN,  USBD_RTX_EPTask[num]);
                    }

#else

                    if (USBD_P_EP[num]) {
                        USBD_P_EP[num](USBD_EVT_IN);
                    }

#endif
                }
            }
        }

        /* OUT Packet */
        if (cmpl & 0x3F) {
            for (num = 0; num < USBD_EP_NUM + 1; num++) {
                if ((cmpl ^ cmpl_pnd) & cmpl & (1UL << num)) {
                    cmpl_pnd |= 1UL << num;
#ifdef __RTX

                    if (USBD_RTX_EPTask[num]) {
                        isr_evt_set(USBD_EVT_OUT, USBD_RTX_EPTask[num]);

                    } else if (IsoEp & (1UL << num)) {
                        if (USBD_RTX_DevTask) {
                            isr_evt_set(USBD_EVT_SOF, USBD_RTX_DevTask);
                        }
                    }

#else

                    if (USBD_P_EP[num]) {
                        USBD_P_EP[num](USBD_EVT_OUT);

                    } else if (IsoEp & (1UL << num)) {
                        if (USBD_P_SOF_Event) {
                            USBD_P_SOF_Event();
                        }
                    }

#endif
                }
            }
        }
    }

    /* error interrupt */
    if (sts & (1UL << 1)) {
        for (num = 0; num < USBD_EP_NUM + 1; num++) {
            if (cmpl & (1UL << num)) {
#ifdef __RTX

                if (USBD_RTX_DevTask) {
                    LastError = dTDx[EP_OUT_IDX(num)].dTD_token & 0xE8;
                    isr_evt_set(USBD_EVT_ERROR, USBD_RTX_DevTask);
                }

#else

                if (USBD_P_Error_Event) {
                    USBD_P_Error_Event(dTDx[EP_OUT_IDX(num)].dTD_token & 0xE8);
                }

#endif
            }

            if (cmpl & (1UL << (num + 16))) {
#ifdef __RTX

                if (USBD_RTX_DevTask) {
                    LastError = dTDx[EP_IN_IDX(num)].dTD_token & 0xE8;
                    isr_evt_set(USBD_EVT_ERROR, USBD_RTX_DevTask);
                }

#else

                if (USBD_P_Error_Event) {
                    USBD_P_Error_Event(dTDx[EP_IN_IDX(num)].dTD_token & 0xE8);
                }

#endif
            }
        }
    }

    NVIC_EnableIRQ(USB0_IRQn);
}
