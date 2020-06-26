/**
 * @file    usbd_kinetis.c
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
#include "fsl_device_registers.h"
#include "cortex_m.h"
#include "util.h"
#include "string.h"

#define __NO_USB_LIB_C
#include "usb_config.c"

typedef struct __BUF_DESC {
    uint8_t    stat;
    uint8_t    reserved;
    uint16_t   bc;
    uint32_t   buf_addr;
} BUF_DESC;

BUF_DESC __align(512) BD[(USBD_EP_NUM + 1) * 2 * 2];
uint8_t EPBuf[(USBD_EP_NUM + 1) * 2 * 2][64];
uint8_t OutEpSize[USBD_EP_NUM + 1];
uint8_t StatQueue[(USBD_EP_NUM + 1) * 2 * 2 + 1];
uint32_t StatQueueHead = 0;
uint32_t StatQueueTail = 0;
uint32_t LastIstat = 0;
uint8_t UsbSuspended = 0;
uint8_t Ep0ZlpOut = 0; 

uint32_t Data1  = 0x55555555;

#define BD_OWN_MASK        0x80
#define BD_DATA01_MASK     0x40
#define BD_KEEP_MASK       0x20
#define BD_NINC_MASK       0x10
#define BD_DTS_MASK        0x08
#define BD_STALL_MASK      0x04

#define TX    1
#define RX    0
#define ODD   0
#define EVEN  1
#define IDX(Ep, dir, Ev_Odd) ((((Ep & 0x0F) * 4) + (2 * dir) + (1 *  Ev_Odd)))

#define SETUP_TOKEN    0x0D
#define IN_TOKEN       0x09
#define OUT_TOKEN      0x01
#define TOK_PID(idx)   ((BD[idx].stat >> 2) & 0x0F)

inline static void protected_and(uint32_t *addr, uint32_t val)
{
    cortex_int_state_t state;
    state = cortex_int_get_and_disable();
    *addr = *addr & val;
    cortex_int_restore(state);
}
inline static void protected_or(uint32_t *addr, uint32_t val)
{
    cortex_int_state_t state;
    state = cortex_int_get_and_disable();
    *addr = *addr | val;
    cortex_int_restore(state);
}
inline static void protected_xor(uint32_t *addr, uint32_t val)
{
    cortex_int_state_t state;
    state = cortex_int_get_and_disable();
    *addr = *addr ^ val;
    cortex_int_restore(state);
}

inline static void stat_enque(uint32_t stat)
{
    cortex_int_state_t state;
    state = cortex_int_get_and_disable();
    StatQueue[StatQueueTail] = stat;
    StatQueueTail = (StatQueueTail + 1) % sizeof(StatQueue);
    cortex_int_restore(state);
}

inline static uint32_t stat_deque()
{
    cortex_int_state_t state;
    uint32_t stat;
    state = cortex_int_get_and_disable();
    stat = StatQueue[StatQueueHead];
    StatQueueHead = (StatQueueHead + 1) % sizeof(StatQueue);
    cortex_int_restore(state);

    return stat;
}

inline static uint32_t stat_is_empty()
{
    cortex_int_state_t state;
    uint32_t empty;
    state = cortex_int_get_and_disable();
    empty = StatQueueHead == StatQueueTail;
    cortex_int_restore(state);
    return empty;
}

/*
 *  USB Device Interrupt enable
 *   Called by USBD_Init to enable the USB Interrupt
 *    Return Value:    None
 */

void USBD_IntrEna(void)
{
    NVIC_EnableIRQ(USB0_IRQn);            /* Enable OTG interrupt               */
}


/*
 *  USB Device Initialize Function
 *   Called by the User to initialize USB
 *   Return Value:    None
 */

void USBD_Init(void)
{
    OutEpSize[0] = USBD_MAX_PACKET0;
    /* Enable all clocks needed for USB to function                             */
    /* Set USB clock to 48 MHz                                                  */
    SIM->SOPT2   |=   SIM_SOPT2_USBSRC_MASK     | /* MCGPLLCLK used as src      */
                      SIM_SOPT2_PLLFLLSEL_MASK  ; /* Select MCGPLLCLK as clock  */
#if defined(TARGET_MK20D5)
    SIM->CLKDIV2 &= ~(SIM_CLKDIV2_USBFRAC_MASK  | /* Clear CLKDIV2 FS values    */
                      SIM_CLKDIV2_USBDIV_MASK);
    SIM->CLKDIV2  =   SIM_CLKDIV2_USBDIV(0)     ; /* USB clk = (PLL*1/2)        */
    /*         = ( 48*1/1)=48     */
#endif // TARGET_MK20D5
    SIM->SCGC4   |=   SIM_SCGC4_USBOTG_MASK;      /* Enable USBOTG clock        */
    USBD_IntrEna();
    USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;

    while (USB0->USBTRC0 & USB_USBTRC0_USBRESET_MASK);

    USB0->BDTPAGE1 = (uint8_t)((uint32_t) BD >> 8);
    USB0->BDTPAGE2 = (uint8_t)((uint32_t) BD >> 16);
    USB0->BDTPAGE3 = (uint8_t)((uint32_t) BD >> 24);
    USB0->ISTAT   = 0xFF;                 /* clear interrupt flags              */
    /* enable interrupts                                                        */
    USB0->INTEN =                            USB_INTEN_USBRSTEN_MASK |
            USB_INTEN_TOKDNEEN_MASK |
            USB_INTEN_SLEEPEN_MASK  |
#ifdef __RTX
            ((USBD_RTX_DevTask   != 0) ? USB_INTEN_SOFTOKEN_MASK : 0) |
            ((USBD_RTX_DevTask   != 0) ? USB_INTEN_ERROREN_MASK  : 0) ;
#else
            ((USBD_P_SOF_Event   != 0) ? USB_INTEN_SOFTOKEN_MASK : 0) |
            ((USBD_P_Error_Event != 0) ? USB_INTEN_ERROREN_MASK  : 0) ;
#endif
    USB0->USBCTRL  = USB_USBCTRL_PDE_MASK;/* pull dawn on D+ and D-             */
    USB0->USBTRC0 |= (1 << 6);            /* bit 6 must be set to 1             */
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
        USB0->CTL  |= USB_CTL_USBENSOFEN_MASK;            /* enable USB           */
        USB0->CONTROL = USB_CONTROL_DPPULLUPNONOTG_MASK;  /* pull up on D+        */
    } else {
        USB0->CTL  &= ~USB_CTL_USBENSOFEN_MASK;           /* disable USB          */
        USB0->CONTROL &= ~USB_CONTROL_DPPULLUPNONOTG_MASK;/* pull down on D+      */
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

    NVIC_DisableIRQ(USB0_IRQn);

    for (i = 1; i < 16; i++) {
        USB0->ENDPOINT[i].ENDPT = 0x00;
    }

    memset(StatQueue, 0, sizeof(StatQueue));
    StatQueueHead = 0;
    StatQueueTail = 0;
    LastIstat = 0;
    UsbSuspended = 0;
    Ep0ZlpOut = 0;

    /* EP0 control endpoint                                                     */
    BD[IDX(0, RX, ODD)].bc       = USBD_MAX_PACKET0;
    BD[IDX(0, RX, ODD)].buf_addr = (uint32_t) & (EPBuf[IDX(0, RX, ODD)][0]);
    BD[IDX(0, RX, ODD)].stat     = BD_OWN_MASK | BD_DTS_MASK | BD_DATA01_MASK;
    BD[IDX(0, RX, EVEN)].stat     = 0;
    BD[IDX(0, TX, ODD)].buf_addr = (uint32_t) & (EPBuf[IDX(0, TX, ODD)][0]);
    BD[IDX(0, TX, EVEN)].buf_addr = 0;
    USB0->ENDPOINT[0].ENDPT = USB_ENDPT_EPHSHK_MASK | /* enable ep handshaking  */
                              USB_ENDPT_EPTXEN_MASK | /* enable TX (IN) tran.   */
                              USB_ENDPT_EPRXEN_MASK;  /* enable RX (OUT) tran.  */
    Data1 = 0x55555555;
    USB0->CTL    |=  USB_CTL_ODDRST_MASK;
    USB0->ISTAT   =  0xFF;                /* clear all interrupt status flags   */
    USB0->ERRSTAT =  0xFF;                /* clear all error flags              */
    USB0->ERREN   =  0xFF;                /* enable error interrupt sources     */
    USB0->ADDR    =  0x00;                /* set default address                */

    NVIC_EnableIRQ(USB0_IRQn);
}


/*
 *  USB Device Suspend Function
 *   Called automatically on USB Device Suspend
 *    Return Value:    None
 */

void USBD_Suspend(void)
{
    USB0->INTEN |= USB_INTEN_RESUMEEN_MASK;
}


/*
 *  USB Device Resume Function
 *   Called automatically on USB Device Resume
 *    Return Value:    None
 */

void USBD_Resume(void)
{
    USB0->INTEN &= ~USB_INTEN_RESUMEEN_MASK;
}


/*
 *  USB Device Remote Wakeup Function
 *   Called automatically on USB Device Remote Wakeup
 *    Return Value:    None
 */

void USBD_WakeUp(void)
{
    uint32_t i = 50000;

    if (USBD_DeviceStatus & USB_GETSTATUS_REMOTE_WAKEUP) {
        USB0->CTL |=  USB_CTL_RESUME_MASK;

        while (i--) {
            __nop();
        }

        USB0->CTL &= ~USB_CTL_RESUME_MASK;
    }
}


/*
 *  USB Device Remote Wakeup Configuration Function
 *    Parameters:      cfg:   Device Enable/Disable
 *    Return Value:    None
 */

void USBD_WakeUpCfg(uint32_t cfg)
{
    /* Not needed                                                               */
}


/*
 *  USB Device Set Address Function
 *    Parameters:      adr:   USB Device Address
 *    Return Value:    None
 */

void USBD_SetAddress(uint32_t  adr, uint32_t setup)
{
    if (!setup) {
        USB0->ADDR    = adr & 0x7F;
    }
}


/*
 *  USB Device Configure Function
 *    Parameters:      cfg:   Device Configure/Deconfigure
 *    Return Value:    None
 */

void USBD_Configure(uint32_t cfg)
{
}


/*
 *  Configure USB Device Endpoint according to Descriptor
 *    Parameters:      pEPD:  Pointer to Device Endpoint Descriptor
 *    Return Value:    None
 */

void USBD_ConfigEP(USB_ENDPOINT_DESCRIPTOR *pEPD)
{
    uint32_t num, val;
    num  = pEPD->bEndpointAddress;
    val  = pEPD->wMaxPacketSize;

    if (!(pEPD->bEndpointAddress & 0x80)) {
        OutEpSize[num] = val;
    }

    USBD_ResetEP(num);
}


/*
 *  Set Direction for USB Device Control Endpoint
 *    Parameters:      dir:   Out (dir == 0), In (dir <> 0)
 *    Return Value:    None
 */

void USBD_DirCtrlEP(uint32_t dir)
{
    /* Not needed                                                               */
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
        EPNum &= 0x0F;
        USB0->ENDPOINT[EPNum].ENDPT |= USB_ENDPT_EPHSHK_MASK | /*en ep handshaking*/
                                       USB_ENDPT_EPTXEN_MASK;  /*en TX (IN) tran  */
    } else {
        USB0->ENDPOINT[EPNum].ENDPT |= USB_ENDPT_EPHSHK_MASK | /*en ep handshaking*/
                                       USB_ENDPT_EPRXEN_MASK;  /*en RX (OUT) tran.*/
    }
}


/*
 *  Disable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_DisableEP(uint32_t EPNum)
{
    if (EPNum & 0x80) {
        EPNum &= 0x0F;
        USB0->ENDPOINT[EPNum].ENDPT &= ~(USB_ENDPT_EPHSHK_MASK |/*dis handshaking */
                                         USB_ENDPT_EPTXEN_MASK);/*dis TX(IN) tran */
    } else {
        USB0->ENDPOINT[EPNum].ENDPT &= ~(USB_ENDPT_EPHSHK_MASK |/*dis handshaking */
                                         USB_ENDPT_EPRXEN_MASK);/*dis RX(OUT) tran*/
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
        EPNum &= 0x0F;
        protected_or(&Data1, (1 << ((EPNum * 2) + 1)));
        BD[IDX(EPNum, TX, ODD)].buf_addr = (uint32_t) & (EPBuf[IDX(EPNum, TX, ODD)][0]);
        BD[IDX(EPNum, TX, EVEN)].buf_addr = 0;
    } else {
        protected_and(&Data1, ~(1 << (EPNum * 2)));
        BD[IDX(EPNum, RX, ODD)].bc       = OutEpSize[EPNum];
        BD[IDX(EPNum, RX, ODD)].buf_addr = (uint32_t) & (EPBuf[IDX(EPNum, RX, ODD)][0]);
        BD[IDX(EPNum, RX, ODD)].stat     = BD_OWN_MASK | BD_DTS_MASK;
        BD[IDX(EPNum, RX, EVEN)].stat     = 0;
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
    EPNum &= 0x0F;
    USB0->ENDPOINT[EPNum].ENDPT |= USB_ENDPT_EPSTALL_MASK;
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
    USB0->ENDPOINT[EPNum & 0x0F].ENDPT &= ~USB_ENDPT_EPSTALL_MASK;
    USBD_ResetEP(EPNum);
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
 *  Read USB Device Endpoint Data
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

uint32_t USBD_ReadEP(uint32_t EPNum, uint8_t *pData, uint32_t size)
{
    uint32_t n, sz, idx, setup = 0;
    idx = IDX(EPNum, RX, 0);
    sz  = BD[idx].bc;

    if ((EPNum == 0) && Ep0ZlpOut) {
        // This packet was a zero length data out packet. It has already
        // been processed by USB0_IRQHandler. Only toggle the DATAx bit
        // and return a size of 0.
        protected_xor(&Data1, (1 << (idx / 2)));
        return 0;
    }

    if ((EPNum == 0) && (TOK_PID(idx) == SETUP_TOKEN)) {
        setup = 1;
    }

    if (size < sz) {
        util_assert(0);
        sz = size;
    }

    for (n = 0; n < sz; n++) {
        pData[n] = EPBuf[idx][n];
    }

    BD[idx].bc = OutEpSize[EPNum];

    if ((Data1 >> (idx / 2) & 1) == ((BD[idx].stat >> 6) & 1)) {
        uint32_t xfer_size = (pData[7] << 8) | (pData[6] << 0);
        if (setup && (0 == xfer_size)) {     /* if no setup data stage,            */
            protected_and(&Data1, ~1);           /* set DATA0                          */
        } else {
            protected_xor(&Data1, (1 << (idx / 2)));
        }
    }

    if ((Data1 >> (idx / 2)) & 1) {
        BD[idx].stat  = BD_DTS_MASK | BD_DATA01_MASK;
        BD[idx].stat |= BD_OWN_MASK;
    } else {
        BD[idx].stat  = BD_DTS_MASK;
        BD[idx].stat |= BD_OWN_MASK;
    }

    return (sz);
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
    uint32_t idx, n;
    EPNum &= 0x0F;
    idx = IDX(EPNum, TX, 0);
    BD[idx].bc = cnt;

    for (n = 0; n < cnt; n++) {
        EPBuf[idx][n] = pData[n];
    }

    if ((Data1 >> (idx / 2)) & 1) {
        BD[idx].stat = BD_OWN_MASK | BD_DTS_MASK;
    } else {
        BD[idx].stat = BD_OWN_MASK | BD_DTS_MASK | BD_DATA01_MASK;
    }

    protected_xor(&Data1, (1 << (idx / 2)));
    return (cnt);
}

/*
 *  Get USB Device Last Frame Number
 *    Parameters:      None
 *    Return Value:    Frame Number
 */

uint32_t USBD_GetFrame(void)
{
    return ((USB0->FRMNUML | (USB0->FRMNUMH << 8) & 0x07FF));
}

#ifdef __RTX
U32 LastError;                          /* Last Error                         */

/*
 *  Get USB Device Last Error Code
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
void USB0_IRQHandler(void)
{
    uint32_t istat, num, dir, ev_odd;
    uint32_t new_istat;
    uint8_t suspended = 0;

    new_istat = istat = USB0->ISTAT;

    // Read all tokens
    if (istat & USB_ISTAT_TOKDNE_MASK) {
        while (istat & USB_ISTAT_TOKDNE_MASK) {
            uint8_t stat = USB0->STAT;
            num    = (stat >> 4) & 0x0F;
            dir    = (stat >> 3) & 0x01;
            ev_odd = (stat >> 2) & 0x01;

            // Consume all zero length OUT packets on endpoint 0 to prevent
            // a subsequent SETUP packet from being dropped
            if ((0 == num) && (RX == dir)) {
                uint32_t idx;
                idx = IDX(num, dir, ev_odd);
                if ((TOK_PID(idx) == OUT_TOKEN) && (BD[idx].bc == 0)) {
                    BD[idx].bc = OutEpSize[num];
                    if (BD[idx].stat & BD_DATA01_MASK) {
                        BD[idx].stat = BD_OWN_MASK | BD_DTS_MASK;
                    } else {
                        BD[idx].stat = BD_OWN_MASK | BD_DTS_MASK | BD_DATA01_MASK;
                    }
                    stat |= 1 << 0;
                }
            }

            stat_enque(stat);
            USB0->ISTAT = USB_ISTAT_TOKDNE_MASK;

            // Check if USB is suspending before checking istat
            suspended = suspended || USB0->CTL & USB_CTL_TXSUSPENDTOKENBUSY_MASK;
            istat = USB0->ISTAT;
        }
    }

    // Set global istat and suspended flags
    new_istat |= istat;
    protected_or(&LastIstat, new_istat);
    UsbSuspended |= suspended ? 1 : 0;
    USB0->ISTAT = istat;

    USBD_SignalHandler();
}

/*
 *  USB Device Service Routine
 */

void USBD_Handler(void)
{
    uint32_t istr, num, dir, ev_odd;
    cortex_int_state_t state;
    uint8_t suspended = 0;

    // Get ISTAT
    state = cortex_int_get_and_disable();
    istr = LastIstat;
    LastIstat = 0;
    suspended = UsbSuspended;
    UsbSuspended = 0;
    cortex_int_restore(state);


    /* reset interrupt                                                            */
    if (istr & USB_ISTAT_USBRST_MASK) {
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

    /* suspend interrupt                                                          */
    if (istr & USB_ISTAT_SLEEP_MASK) {
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

    /* resume interrupt                                                           */
    if (istr & USB_ISTAT_RESUME_MASK) {
        USBD_Resume();
#ifdef __RTX

        if (USBD_RTX_DevTask) {
            isr_evt_set(USBD_EVT_RESUME, USBD_RTX_DevTask);
        }

#else

        if (USBD_P_Resume_Event) {
            USBD_P_Resume_Event();
        }

#endif
    }

    /* Start Of Frame                                                             */
    if (istr & USB_ISTAT_SOFTOK_MASK) {
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

    /* Error interrupt                                                            */
    if (istr == USB_ISTAT_ERROR_MASK) {
#ifdef __RTX
        LastError = USB0->ERRSTAT;

        if (USBD_RTX_DevTask) {
            isr_evt_set(USBD_EVT_ERROR, USBD_RTX_DevTask);
        }

#else

        if (USBD_P_Error_Event) {
            USBD_P_Error_Event(USB0->ERRSTAT);
        }

#endif
        USB0->ERRSTAT = 0xFF;
    }

    /* token interrupt                                                            */
    if (istr & USB_ISTAT_TOKDNE_MASK) {
        while (!stat_is_empty()) {
            uint32_t stat;

            stat = stat_deque();
            num    = (stat >> 4) & 0x0F;
            dir    = (stat >> 3) & 0x01;
            ev_odd = (stat >> 2) & 0x01;

            /* setup packet                                                               */
            if ((num == 0) && (TOK_PID((IDX(num, dir, ev_odd))) == SETUP_TOKEN)) {
                Data1 &= ~0x02;
                BD[IDX(0, TX, EVEN)].stat &= ~BD_OWN_MASK;
                BD[IDX(0, TX, ODD)].stat  &= ~BD_OWN_MASK;
                Ep0ZlpOut = 0;
#ifdef __RTX

                if (USBD_RTX_EPTask[num]) {
                    isr_evt_set(USBD_EVT_SETUP, USBD_RTX_EPTask[num]);
                }

#else

                if (USBD_P_EP[num]) {
                    USBD_P_EP[num](USBD_EVT_SETUP);
                }

#endif

            } else {
                /* OUT packet                                                                 */
                if (TOK_PID((IDX(num, dir, ev_odd))) == OUT_TOKEN) {
                    if (0 == num) {
                        Ep0ZlpOut = stat & (1 << 0);
                    }
#ifdef __RTX

                    if (USBD_RTX_EPTask[num]) {
                        isr_evt_set(USBD_EVT_OUT, USBD_RTX_EPTask[num]);
                    }

#else

                    if (USBD_P_EP[num]) {
                        USBD_P_EP[num](USBD_EVT_OUT);
                    }

#endif
                }

                /* IN packet                                                                  */
                if (TOK_PID((IDX(num, dir, ev_odd))) == IN_TOKEN) {
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
    }

    if (suspended) {
        USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
    }
}
