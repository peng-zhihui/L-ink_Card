/**
 * @file    usbd_LPC11Uxx.c
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
#include "LPC11Uxx.h"
#include "compiler.h"
#include "util.h"

#define __NO_USB_LIB_C
#include "usb_config.c"


#define BUF_ACTIVE         (1UL << 31)
#define EP_DISABLED        (1UL << 30)
#define EP_STALL           (1UL << 29)
#define TOOGLE_RESET       (1UL << 28)
#define EP_TYPE            (1UL << 26)

#define N_BYTES(n)         ((n & 0x3FF) << 16)
#define BUF_ADDR(addr)     (((addr) >> 6) & 0xFFFF)

#define EP_OUT_IDX(EPNum)  (EPNum * 2    )
#define EP_IN_IDX(EPNum)   (EPNum * 2 + 1)

#define EP_LIST_BASE       0x20004000
#define EP_BUF_BASE        (U32)(EP_LIST_BASE + 0x100)

typedef struct BUF_INFO {
    U32  buf_len;
    U32  buf_ptr;
} EP_BUF_INFO;

EP_BUF_INFO EPBufInfo[(USBD_EP_NUM + 1) * 2];
volatile U32 EPList[(USBD_EP_NUM + 1) * 2]  __at(EP_LIST_BASE);

static U32 addr = 3 * 64 + EP_BUF_BASE;
static U32 ctrl_out_next = 0;

/*
 *  Get EP CmdStat pointer
 *    Parameters:    EPNum: endpoint number
 *
 */

U32 *GetEpCmdStatPtr(U32 EPNum)
{
    U32 ptr = 0;

    if (EPNum & 0x80) {
        EPNum &= ~0x80;
        ptr = 8;
    }

    ptr += EP_LIST_BASE + EPNum * 16;
    return ((U32 *)ptr);
}


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
    if (ena) {
        NVIC_EnableIRQ(USB_IRQn);           /* Enable USB interrupt */
    } else {
        NVIC_DisableIRQ(USB_IRQn);          /* Disable USB interrupt */
    }
}
#endif



/*
 *  USB Device Initialize Function
 *   Called by the User to initialize USB Device
 *    Return Value:    None
 */

void USBD_Init(void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1UL <<  6);
    LPC_SYSCON->SYSAHBCLKCTRL |= (1UL << 14) |
                                 (1UL << 27);
    LPC_USB->DEVCMDSTAT  |= (1UL << 9);     /* PLL ON */
    LPC_IOCON->PIO0_3    &=  ~(0x1F);
    LPC_IOCON->PIO0_3    |= (1UL << 0);     /* Secondary function VBUS */
    LPC_IOCON->PIO0_6    &=   ~7;
    LPC_IOCON->PIO0_6    |= (1UL << 0);     /* Secondary function USB CON */
    LPC_SYSCON->PDRUNCFG &= ~((1UL << 8) |  /* USB PLL powered */
                              (1UL << 10)); /* USB transceiver powered */
    LPC_USB->DATABUFSTART = EP_BUF_BASE & 0xFFC00000;
    LPC_USB->EPLISTSTART  = EP_LIST_BASE;
    NVIC_EnableIRQ(USB_IRQn);
    USBD_Reset();
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
        LPC_USB->DEVCMDSTAT |= (1UL << 16);    /* Set device connect status */
    } else {
        LPC_USB->DEVCMDSTAT &= ~(1UL << 16);   /* Clear device connect status */
    }

    return;
}


/*
 *  USB Device Reset Function
 *   Called automatically on USB Device Reset
 *    Return Value:    None
 */

void USBD_Reset(void)
{
    U32 i;
    U32 *ptr;
    addr = 3 * 64 + EP_BUF_BASE;

    for (i = 2; i < (5 * 4); i++) {
        EPList[i] = (1UL << 30);            /* EPs disabled */
    }

    ctrl_out_next = 0;
    EPBufInfo[0].buf_len = USBD_MAX_PACKET0;
    EPBufInfo[0].buf_ptr = EP_BUF_BASE;
    EPBufInfo[1].buf_len = USBD_MAX_PACKET0;
    EPBufInfo[1].buf_ptr = EP_BUF_BASE + 2 * 64;
    ptr  = GetEpCmdStatPtr(0);
    *ptr = N_BYTES(EPBufInfo[0].buf_len) |     /* EP0 OUT */
           BUF_ADDR(EPBufInfo[0].buf_ptr) |
           BUF_ACTIVE;
    ptr++;
    *ptr = BUF_ADDR(EPBufInfo[0].buf_ptr + 64);/* SETUP */
    LPC_USB->DEVCMDSTAT |= (1UL << 7);         /*USB device enable */
    LPC_USB->INTSTAT     = 0x2FC;              /* clear EP interrupt flags */
    LPC_USB->INTEN = ((1UL << 30) |            /* SOF intr enable */
                      (1UL << 0)     | /* EP0 OUT intr enable */
                      (1UL << 1)     | /* EP0 IN intr enable */
                      (1UL << 31));    /* stat change int en */
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
    LPC_SYSCON->USBCLKCTRL = 1;
    LPC_USB->DEVCMDSTAT &= ~(1UL << 17);  /*clear device suspend status */

    while (LPC_USB->DEVCMDSTAT & (1UL << 17));

    LPC_SYSCON->USBCLKCTRL = 0;
}


/*
 *  USB Device Remote Wakeup Configuration Function
 *    Parameters:      cfg:   Device Enable/Disable
 *    Return Value:    None
 */

void USBD_WakeUpCfg(BOOL cfg)
{
    if (cfg == __TRUE) {
        LPC_USB->DEVCMDSTAT &= ~(1UL << 9); /*PPL_ON=0, in suspend clk is stoped */
    } else {
        LPC_USB->DEVCMDSTAT |= (1UL << 9);  /*PPL_ON=1, in suspend clk isnt stoped */
        LPC_SYSCON->USBCLKCTRL = 0;
    }
}


/*
 *  USB Device Set Address Function
 *    Parameters:      adr:   USB Device Address
 *    Return Value:    None
 */

void USBD_SetAddress(U32 adr, U32 setup)
{
    if (!setup) {
        LPC_USB->DEVCMDSTAT &= ~0x7F;
        LPC_USB->DEVCMDSTAT |= adr | (1UL << 7);
    }
}



/*
 *  USB Device Configure Function
 *    Parameters:      cfg:   Device Configure/Deconfigure
 *    Return Value:    None
 */

void USBD_Configure(BOOL cfg)
{
    addr = 3 * 64 + EP_BUF_BASE;
}


/*
 *  Configure USB Device Endpoint according to Descriptor
 *    Parameters:      pEPD:  Pointer to Device Endpoint Descriptor
 *    Return Value:    None
 */

void USBD_ConfigEP(USB_ENDPOINT_DESCRIPTOR *pEPD)
{
    U32 num, val, type;
    U32 *ptr;
    num  = pEPD->bEndpointAddress;
    val  = pEPD->wMaxPacketSize;
    type = pEPD->bmAttributes & USB_ENDPOINT_TYPE_MASK;

    /* IN EPs */
    if (num & 0x80) {
        num &= ~0x80;
        EPBufInfo[EP_IN_IDX(num)].buf_len  = val;
        EPBufInfo[EP_IN_IDX(num)].buf_ptr  = addr;
        addr += ((val + 63) >> 6) * 64;     /* calc new free buffer address */
        ptr  = GetEpCmdStatPtr(num | 0x80);
        *ptr = EP_DISABLED;

        if (type == USB_ENDPOINT_TYPE_ISOCHRONOUS) {
            *ptr |= EP_TYPE;
        }
    }

    /* OUT EPs */
    else {
        EPBufInfo[EP_OUT_IDX(num)].buf_len  = val;
        EPBufInfo[EP_OUT_IDX(num)].buf_ptr  = addr;
        ptr  = GetEpCmdStatPtr(num);
        *ptr = N_BYTES(EPBufInfo[EP_OUT_IDX(num)].buf_len) |
               BUF_ADDR(EPBufInfo[EP_OUT_IDX(num)].buf_ptr) |
               EP_DISABLED;

        if (type == USB_ENDPOINT_TYPE_ISOCHRONOUS) {
            *ptr |= EP_TYPE;
        }

        addr += ((val + 63) >> 6) * 64;     /* calc new free buffer address */
    }
}


/*
 *  Set Direction for USB Device Control Endpoint
 *    Parameters:      dir:   Out (dir == 0), In (dir <> 0)
 *    Return Value:    None
 */

void USBD_DirCtrlEP(U32 dir)
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

void USBD_EnableEP(U32 EPNum)
{
    U32 *ptr;;
    ptr = GetEpCmdStatPtr(EPNum);

    /* IN EP */
    if (EPNum & 0x80) {
        EPNum &= ~0x80;
        *ptr &= ~EP_DISABLED;
        LPC_USB->INTSTAT = (1 << EP_IN_IDX(EPNum));
        LPC_USB->INTEN  |= (1 << EP_IN_IDX(EPNum));
    }

    /* OUT EP */
    else {
        *ptr &= ~EP_DISABLED;
        *ptr |=  BUF_ACTIVE;
        LPC_USB->INTSTAT = (1 << EP_OUT_IDX(EPNum));
        LPC_USB->INTEN  |= (1 << EP_OUT_IDX(EPNum));
    }
}


/*
 *  Disable USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_DisableEP(U32 EPNum)
{
    U32 *ptr;
    ptr = GetEpCmdStatPtr(EPNum);
    *ptr = EP_DISABLED;

    if (EPNum & 0x80) {
        EPNum &= ~0x80;
        LPC_USB->INTEN &= ~(1 << EP_IN_IDX(EPNum));

    } else {
        LPC_USB->INTEN &= ~(1 << EP_OUT_IDX(EPNum));
    }
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
    U32 *ptr;
    ptr = GetEpCmdStatPtr(EPNum);
    *ptr |= TOOGLE_RESET;
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
    U32 *ptr;
    ptr = GetEpCmdStatPtr(EPNum);

    if (EPNum & 0x7F) {
        if (*ptr & BUF_ACTIVE) {
            *ptr &= ~(BUF_ACTIVE);
        }

    } else {
        if (EPNum & 0x80) {
            EPNum &= ~0x80;
            LPC_USB->EPSKIP |= (1 << EP_IN_IDX(EPNum));

            while (LPC_USB->EPSKIP & (1 << EP_IN_IDX(EPNum)));

        } else {
            LPC_USB->EPSKIP |= (1 << EP_OUT_IDX(EPNum));

            while (LPC_USB->EPSKIP & (1 << EP_OUT_IDX(EPNum)));
        }
    }

    if ((EPNum & 0x7F) == 0) {
        /* Endpoint is stalled so control out won't be next */
        ctrl_out_next = 0;
    }

    *ptr |=  EP_STALL;
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
    U32 *ptr;
    ptr = GetEpCmdStatPtr(EPNum);

    if (EPNum & 0x80) {
        *ptr &=  ~EP_STALL;

    } else {
        *ptr &=  ~EP_STALL;
        *ptr |=   BUF_ACTIVE;
    }

    USBD_ResetEP(EPNum);
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
    U32  cnt, i;
    U8  *dataptr;

    if (EPNum & 0x80) {
        EPNum &= ~0x80;
        dataptr = (U8 *)EPBufInfo[EP_IN_IDX(EPNum)].buf_ptr;
        cnt     =       EPBufInfo[EP_IN_IDX(EPNum)].buf_len;

        for (i = 0; i < cnt; i++) {
            dataptr[i] = 0;
        }

    } else {
        dataptr = (U8 *)EPBufInfo[EP_OUT_IDX(EPNum)].buf_ptr;
        cnt     =       EPBufInfo[EP_OUT_IDX(EPNum)].buf_len;

        for (i = 0; i < cnt; i++) {
            dataptr[i] = 0;
        }
    }
}


/*
 *  Read USB Device Endpoint Data
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

U32 USBD_ReadEP(U32 EPNum, U8 *pData, U32 size)
{
    U32 cnt, i, xfer_size;
    volatile U32 *ptr;
    U8 *dataptr;
    ptr = GetEpCmdStatPtr(EPNum);
    int timeout = 256;

    /* Setup packet */
    if ((EPNum == 0) && !ctrl_out_next && (LPC_USB->DEVCMDSTAT & (1UL << 8))) {
        cnt = USBD_MAX_PACKET0;

        if (size < cnt) {
            util_assert(0);
            cnt = size;
        }

        dataptr = (U8 *)(EPBufInfo[EP_OUT_IDX(EPNum)].buf_ptr + 64);

        for (i = 0; i < cnt; i++) {
            pData[i] = dataptr[i];
        }

        xfer_size = (pData[7] << 8) | (pData[6] << 0);
        if ((xfer_size > 0) && (pData[0] & (1 << 7))) {
            /* This control transfer has a data IN stage            */
            /* and ends with a zero length data OUT transfer.       */
            /* Ensure the data OUT token is not skipped even if     */
            /* a SETUP token arrives before USBD_ReadEP has         */
            /* been called.                                         */
            ctrl_out_next = 1;
        }

        LPC_USB->EPSKIP |= (1 << EP_IN_IDX(EPNum));

        while (LPC_USB->EPSKIP & (1 << EP_IN_IDX(EPNum)));

        if (*(ptr + 2) & EP_STALL) {
            *(ptr + 2) &= ~(EP_STALL);
        }

        if (*ptr & EP_STALL) {
            *ptr &= ~(EP_STALL);
        }

        LPC_USB->DEVCMDSTAT |= (1UL << 8);
    }

    /* OUT packet */
    else {
        ptr = GetEpCmdStatPtr(EPNum);
        cnt = EPBufInfo[EP_OUT_IDX(EPNum)].buf_len - ((*ptr >> 16) & 0x3FF);
        dataptr = (U8 *)EPBufInfo[EP_OUT_IDX(EPNum)].buf_ptr;

        while ((timeout-- > 0) && (*ptr & BUF_ACTIVE)); //spin on the hardware until it's done        
        util_assert(!(*ptr & BUF_ACTIVE)); //check for timeout
        
        if (size < cnt) {
            util_assert(0);
            cnt = size;
        }

        cnt = cnt < size ? cnt : size;

        for (i = 0; i < cnt; i++) {
            pData[i] = dataptr[i];
        }

        *ptr = N_BYTES(EPBufInfo[EP_OUT_IDX(EPNum)].buf_len) |
               BUF_ADDR(EPBufInfo[EP_OUT_IDX(EPNum)].buf_ptr) |
               BUF_ACTIVE;

        if (EPNum == 0) {
            /* If ctrl_out_next is set then this should be a zero length        */
            /* data OUT packet.                                                 */
            util_assert(!ctrl_out_next || (cnt == 0));
            ctrl_out_next = 0;
            if (LPC_USB->DEVCMDSTAT & (1UL << 8))  {
                // A setup packet is still pending so trigger another interrupt
                LPC_USB->INTSETSTAT |= (1 << 0);
            }
        }
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

U32 USBD_WriteEP(U32 EPNum, U8 *pData, U32 cnt)
{
    U32 i;
    volatile U32 *ptr;
    U32 *dataptr;
    ptr = GetEpCmdStatPtr(EPNum);
    EPNum &= ~0x80;

    while (*ptr & BUF_ACTIVE);

    *ptr &= ~(0x3FFFFFF);
    *ptr |=  BUF_ADDR(EPBufInfo[EP_IN_IDX(EPNum)].buf_ptr) |
             N_BYTES(cnt);
    dataptr = (U32 *)EPBufInfo[EP_IN_IDX(EPNum)].buf_ptr;

    for (i = 0; i < (cnt + 3) / 4; i++) {
        dataptr[i] = * ((__packed U32 *)pData);
        pData += 4;
    }

    if (EPNum && (*ptr & EP_STALL)) {
        return (0);
    }

    *ptr |= BUF_ACTIVE;
    return (cnt);
}


/*
 *  Get USB Device Last Frame Number
 *    Parameters:      None
 *    Return Value:    Frame Number
 */

U32 USBD_GetFrame(void)
{
    return (LPC_USB->INFO & 0x7FF);
}


/*
 *  USB Device Interrupt Service Routine
 */

void USB_IRQHandler(void)
{
    NVIC_DisableIRQ(USB_IRQn);
    USBD_SignalHandler();
}

void USBD_Handler(void)
{
    U32 sts, val, num, i;
    sts = LPC_USB->INTSTAT;
    LPC_USB->INTSTAT = sts;

    /* Device Status Interrupt (Reset, Connect change, Suspend/Resume) */
    if (sts & (1UL << 31)) {
        val = LPC_USB->DEVCMDSTAT;

        /* reset interrupt */
        if (val & (1UL << 26)) {
            LPC_USB->DEVCMDSTAT |= (1UL << 26);
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

        /* connect interrupt */
        if (val & (1UL << 24)) {
            LPC_USB->DEVCMDSTAT |= (1UL << 24);
#ifdef __RTX

            if (USBD_RTX_DevTask) {
                if (val & (1UL << 16)) {
                    isr_evt_set(USBD_EVT_POWER_ON,  USBD_RTX_DevTask);
                } else {
                    isr_evt_set(USBD_EVT_POWER_OFF, USBD_RTX_DevTask);
                }
            }

#else

            if (USBD_P_Power_Event) {
                USBD_P_Power_Event((val >> 16) & 1);
            }

#endif
        }

        /* suspend/resume interrupt */
        if (val & (1 << 25)) {
            LPC_USB->DEVCMDSTAT |= (1UL << 25);

            /* suspend interrupt */
            if (val & (1UL << 17)) {
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

            /* resume interrupt */
            else {
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
        }
    }

    /* Start of Frame */
    if (sts & (1UL << 30)) {
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

    /* EndPoint Interrupt */
    if (sts & 0x3FF) {
        const uint32_t endpoint_count = ((USBD_EP_NUM + 1) * 2);

        for (i = 0; i < endpoint_count; i++) {
            // Iterate through endpoints in the reverse order so IN endpoints
            // get processed before OUT endpoints if they are both pending.
            num = endpoint_count - i - 1;

            if (sts & (1UL << num)) {
                /* Setup */
                if ((num == 0) && !ctrl_out_next && (LPC_USB->DEVCMDSTAT & (1UL << 8))) {
#ifdef __RTX

                    if (USBD_RTX_EPTask[num / 2]) {
                        isr_evt_set(USBD_EVT_SETUP, USBD_RTX_EPTask[num / 2]);
                    }

#else

                    if (USBD_P_EP[num / 2]) {
                        USBD_P_EP[num / 2](USBD_EVT_SETUP);
                    }

#endif
                }

                /* OUT */
                else if ((num % 2) == 0) {
#ifdef __RTX

                    if (USBD_RTX_EPTask[num / 2]) {
                        isr_evt_set(USBD_EVT_OUT, USBD_RTX_EPTask[num / 2]);
                    }

#else

                    if (USBD_P_EP[num / 2]) {
                        USBD_P_EP[num / 2](USBD_EVT_OUT);
                    }

#endif
                }

                /* IN */
                else {
#ifdef __RTX

                    if (USBD_RTX_EPTask[num / 2]) {
                        isr_evt_set(USBD_EVT_IN,  USBD_RTX_EPTask[num / 2]);
                    }

#else

                    if (USBD_P_EP[num / 2]) {
                        USBD_P_EP[num / 2](USBD_EVT_IN);
                    }

#endif
                }
            }
        }
    }

    NVIC_EnableIRQ(USB_IRQn);
}
