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

/*----------------------------------------------------------------------------
 *      RL-ARM - USB
 *----------------------------------------------------------------------------
 *      Name:    usbreg.h
 *      Purpose: Hardware Layer Definitions for ST STM32F10x
 *---------------------------------------------------------------------------*/

#ifndef __USBREG_H
#define __USBREG_H


#define REG(x)  (*((volatile unsigned int *)(x)))

#define USB_BASE_ADDR   0x40005C00  /* USB Registers Base Address */
#define USB_PMA_ADDR    0x40006000  /* USB Packet Memory Area Address */


/* Common Registers */
#define CNTR    REG(USB_BASE_ADDR + 0x40)   /* Control Register */
#define ISTR    REG(USB_BASE_ADDR + 0x44)   /* Interrupt Status Register */
#define FNR     REG(USB_BASE_ADDR + 0x48)   /* Frame Number Register */
#define DADDR   REG(USB_BASE_ADDR + 0x4C)   /* Device Address Register */
#define BTABLE  REG(USB_BASE_ADDR + 0x50)   /* Buffer Table Address Register */

/* CNTR: Control Register Bit Definitions */
#define CNTR_CTRM       0x8000      /* Correct Transfer Interrupt Mask */
#define CNTR_PMAOVRM    0x4000      /* Packet Memory Aerea Over/underrun Interrupt Mask */
#define CNTR_ERRM       0x2000      /* Error Interrupt Mask */
#define CNTR_WKUPM      0x1000      /* Wake-up Interrupt Mask */
#define CNTR_SUSPM      0x0800      /* Suspend Mode Interrupt Mask  */
#define CNTR_RESETM     0x0400      /* USB Reset Interrupt Mask   */
#define CNTR_SOFM       0x0200      /* Start of Frame Interrupt Mask */
#define CNTR_ESOFM      0x0100      /* Expected Start of Frame Interrupt Mask */
#define CNTR_RESUME     0x0010      /* Resume Request */
#define CNTR_FSUSP      0x0008      /* Force Suspend */
#define CNTR_LPMODE     0x0004      /* Low-power Mode */
#define CNTR_PDWN       0x0002      /* Power Down */
#define CNTR_FRES       0x0001      /* Force USB Reset */

/* ISTR: Interrupt Status Register Bit Definitions */
#define ISTR_CTR        0x8000      /* Correct Transfer */
#define ISTR_PMAOVR     0x4000      /* Packet Memory Aerea Over/underrun */
#define ISTR_ERR        0x2000      /* Error */
#define ISTR_WKUP       0x1000      /* Wake-up */
#define ISTR_SUSP       0x0800      /* Suspend Mode */
#define ISTR_RESET      0x0400      /* USB Reset */
#define ISTR_SOF        0x0200      /* Start of Frame */
#define ISTR_ESOF       0x0100      /* Expected Start of Frame */
#define ISTR_DIR        0x0010      /* Direction of Transaction */
#define ISTR_EP_ID      0x000F      /* EndPoint Identifier */

/* FNR: Frame Number Register Bit Definitions */
#define FNR_RXDP        0x8000      /* D+ Data Line Status */
#define FNR_RXDM        0x4000      /* D- Data Line Status */
#define FNR_LCK         0x2000      /* Locked */
#define FNR_LSOF        0x1800      /* Lost SOF */
#define FNR_FN          0x07FF      /* Frame Number */

/* DADDR: Device Address Register Bit Definitions */
#define DADDR_EF        0x0080      /* Enable Function */
#define DADDR_ADD       0x007F      /* Device Address */


/* EndPoint Registers */
#define EPxREG(x)       REG(USB_BASE_ADDR + 4*(x))

/* EPxREG: EndPoint Registers Bit Definitions */
#define EP_CTR_RX       0x8000      /* Correct RX Transfer */
#define EP_DTOG_RX      0x4000      /* RX Data Toggle */
#define EP_STAT_RX      0x3000      /* RX Status */
#define EP_SETUP        0x0800      /* EndPoint Setup */
#define EP_TYPE         0x0600      /* EndPoint Type */
#define EP_KIND         0x0100      /* EndPoint Kind */
#define EP_CTR_TX       0x0080      /* Correct TX Transfer */
#define EP_DTOG_TX      0x0040      /* TX Data Toggle */
#define EP_STAT_TX      0x0030      /* TX Status */
#define EP_EA           0x000F      /* EndPoint Address */

/* EndPoint Register Mask (No Toggle Fields) */
#define EP_MASK         (EP_CTR_RX|EP_SETUP|EP_TYPE|EP_KIND|EP_CTR_TX|EP_EA)
/* EndPoint Register Mask (Write zero to clear) */
#define EP_MASK_RC_W0   (EP_CTR_RX|EP_CTR_TX)
/* Mask off all toggle bits and set write zero to clear bits to 1.          */
/* This creates a value that can be written back to the EndPoint register   */
/* which does not change any status bits.                                   */
#define EP_VAL_UNCHANGED(val)       (((val) & EP_MASK) | EP_MASK_RC_W0)

/* EP_TYPE: EndPoint Types */
#define EP_BULK         0x0000      /* BULK EndPoint */
#define EP_CONTROL      0x0200      /* CONTROL EndPoint */
#define EP_ISOCHRONOUS  0x0400      /* ISOCHRONOUS EndPoint */
#define EP_INTERRUPT    0x0600      /* INTERRUPT EndPoint */

/* EP_KIND: EndPoint Kind */
#define EP_DBL_BUF      EP_KIND     /* Double Buffer for Bulk Endpoint */
#define EP_STATUS_OUT   EP_KIND     /* Status Out for Control Endpoint */

/* EP_STAT_TX: TX Status */
#define EP_TX_DIS       0x0000      /* Disabled */
#define EP_TX_STALL     0x0010      /* Stalled */
#define EP_TX_NAK       0x0020      /* NAKed */
#define EP_TX_VALID     0x0030      /* Valid */

/* EP_STAT_RX: RX Status */
#define EP_RX_DIS       0x0000      /* Disabled */
#define EP_RX_STALL     0x1000      /* Stalled */
#define EP_RX_NAK       0x2000      /* NAKed */
#define EP_RX_VALID     0x3000      /* Valid */


/* Endpoint Buffer Descriptor */
typedef struct _EP_BUF_DSCR {
    U32 ADDR_TX;
    U32 COUNT_TX;
    U32 ADDR_RX;
    U32 COUNT_RX;
} EP_BUF_DSCR;

#define EP_ADDR_MASK    0xFFFE      /* Address Mask */
#define EP_COUNT_MASK   0x03FF      /* Count Mask */


#endif  /* __USBREG_H */
