/**
 * @file    usb_lib.c
 * @brief   USB library
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

#include <string.h>
#include "rl_usb.h"
#include "usb.h"
#include "settings.h"

#pragma thumb
#pragma O3


/*------------------------------------------------------------------------------
 *      Library for usb_config.c
 *----------------------------------------------------------------------------*/

#ifdef  __USB_CONFIG__

/*------------------------------------------------------------------------------
 *      USB Device Configuration
 *----------------------------------------------------------------------------*/

#if    (USBD_ENABLE)

U8 USBD_AltSetting[USBD_IF_NUM_MAX];
U8 USBD_EP0Buf[USBD_MAX_PACKET0];
const U8 usbd_power = USBD_POWER;
const U8 usbd_hs_enable = USBD_HS_ENABLE;
const U8 usbd_bos_enable = USBD_BOS_ENABLE;
U16 usbd_if_num = USBD_IF_NUM_MAX;
const U8 usbd_ep_num = USBD_EP_NUM;
const U8 usbd_max_packet0 = USBD_MAX_PACKET0;


/*------------------------------------------------------------------------------
 *      USB Device Class Configuration
 *----------------------------------------------------------------------------*/

#if    (!USBD_HID_BINTERVAL)
#define USBD_HID_INTERVAL                1
#else
#define USBD_HID_INTERVAL                USBD_HID_BINTERVAL
#endif
#if    (!USBD_HID_HS_BINTERVAL)
#define USBD_HID_HS_INTERVAL             1
#else
#define USBD_HID_HS_INTERVAL            (2 << ((USBD_HID_HS_BINTERVAL & 0x0F)-1))
#endif

#if    (USBD_HID_ENABLE)
U8 usbd_hid_if_num; //assigned during runtime init
const U8 usbd_hid_ep_intin = USBD_HID_EP_INTIN;
const U8 usbd_hid_ep_intout = USBD_HID_EP_INTOUT;
const U16 usbd_hid_interval[2]  = {USBD_HID_INTERVAL, USBD_HID_HS_INTERVAL};
const U16 usbd_hid_maxpacketsize[2] = {USBD_HID_WMAXPACKETSIZE, USBD_HID_HS_WMAXPACKETSIZE};
const U8 usbd_hid_inreport_num = USBD_HID_INREPORT_NUM;
const U8 usbd_hid_outreport_num = USBD_HID_OUTREPORT_NUM;
const U16 usbd_hid_inreport_max_sz = USBD_HID_INREPORT_MAX_SZ;
const U16 usbd_hid_outreport_max_sz = USBD_HID_OUTREPORT_MAX_SZ;
const U16 usbd_hid_featreport_max_sz = USBD_HID_FEATREPORT_MAX_SZ;
U16 USBD_HID_PollingCnt;
U8 USBD_HID_IdleCnt[USBD_HID_INREPORT_NUM];
U8 USBD_HID_IdleReload[USBD_HID_INREPORT_NUM];
U8 USBD_HID_IdleSet[USBD_HID_INREPORT_NUM];
U8 USBD_HID_InReport[USBD_HID_INREPORT_MAX_SZ + 1];
U8 USBD_HID_OutReport[USBD_HID_OUTREPORT_MAX_SZ + 1];
U8 USBD_HID_FeatReport[USBD_HID_FEATREPORT_MAX_SZ + 1];
#endif

#if    (USBD_MSC_ENABLE)
U8 usbd_msc_if_num; //assigned during runtime init
const U8 usbd_msc_ep_bulkin = USBD_MSC_EP_BULKIN;
const U8 usbd_msc_ep_bulkout = USBD_MSC_EP_BULKOUT;
const U16 usbd_msc_maxpacketsize[2] = {USBD_MSC_WMAXPACKETSIZE, USBD_MSC_HS_WMAXPACKETSIZE};
const U8 *usbd_msc_inquiry_data = USBD_MSC_INQUIRY_DATA;
const U16 USBD_MSC_BulkBufSize = USBD_MSC_MAX_PACKET;
U8 USBD_MSC_BulkBuf[USBD_MSC_MAX_PACKET];
#endif

#if    (USBD_ADC_ENABLE)
const U8 usbd_adc_cif_num = USBD_ADC_CIF_NUM;
const U8 usbd_adc_sif1_num = USBD_ADC_SIF1_NUM;
const U8 usbd_adc_sif2_num = USBD_ADC_SIF2_NUM;
const U8 usbd_adc_ep_isoout = USBD_ADC_EP_ISOOUT;
const U32 usbd_adc_cfg_datafreq = USBD_ADC_TSAMFREQ;
const U32 usbd_adc_cfg_p_s = USBD_ADC_CFG_P_S;
const U32 usbd_adc_cfg_p_c = USBD_ADC_CFG_P_C;
const U32 usbd_adc_cfg_b_s = (8 * USBD_ADC_CFG_P_C *USBD_ADC_CFG_P_S);
S16  USBD_ADC_DataBuf[8 * USBD_ADC_CFG_P_C * USBD_ADC_CFG_P_S];
#endif

#ifndef USBD_CDC_ACM_ENABLE
#if    (USBD_CDC_ENABLE == 1)
#error "Please update usb_config.c file with new definitions for CDC, as new CDC is incompatible with the old one!"
#else
#define USBD_CDC_ACM_ENABLE  0
#endif
#endif

#if    (USBD_CDC_ACM_ENABLE)
U8 usbd_cdc_acm_cif_num = 0; //assigned during runtime init
U8 usbd_cdc_acm_dif_num = 0; //assigned during runtime init
const U8 usbd_cdc_acm_ep_intin = USBD_CDC_ACM_EP_INTIN;
const U8 usbd_cdc_acm_ep_bulkin = USBD_CDC_ACM_EP_BULKIN;
const U8 usbd_cdc_acm_ep_bulkout = USBD_CDC_ACM_EP_BULKOUT;
const U16 usbd_cdc_acm_sendbuf_sz = USBD_CDC_ACM_SENDBUF_SIZE;
const U16 usbd_cdc_acm_receivebuf_sz = USBD_CDC_ACM_RECEIVEBUF_SIZE;
const U16 usbd_cdc_acm_maxpacketsize[2] = {USBD_CDC_ACM_WMAXPACKETSIZE, USBD_CDC_ACM_HS_WMAXPACKETSIZE};
const U16 usbd_cdc_acm_maxpacketsize1[2] = {USBD_CDC_ACM_WMAXPACKETSIZE1, USBD_CDC_ACM_HS_WMAXPACKETSIZE1};
U8 USBD_CDC_ACM_SendBuf[USBD_CDC_ACM_SENDBUF_SIZE];
U8 USBD_CDC_ACM_ReceiveBuf[USBD_CDC_ACM_RECEIVEBUF_SIZE];
U8 USBD_CDC_ACM_NotifyBuf[10];
#endif

#if    (USBD_WEBUSB_ENABLE)
U8 usbd_webusb_if_num; //assigned during runtime init
const U8 usbd_webusb_vendor_code = USBD_WEBUSB_VENDOR_CODE;
#else
const U8 usbd_webusb_vendor_code;
#endif

#if    (USBD_WINUSB_ENABLE)
const U8 usbd_winusb_vendor_code = USBD_WINUSB_VENDOR_CODE;
#else
const U8 usbd_winusb_vendor_code;
#endif

#if    (USBD_BULK_ENABLE)
U8 usbd_bulk_if_num  = 0; //assigned during runtime init
const U8 usbd_bulk_ep_bulkin = USBD_BULK_EP_BULKIN;
const U8 usbd_bulk_ep_bulkout = USBD_BULK_EP_BULKOUT;
const U16 usbd_bulk_maxpacketsize[2] = {USBD_BULK_WMAXPACKETSIZE, USBD_BULK_HS_WMAXPACKETSIZE};
const U16 USBD_Bulk_BulkBufSize = USBD_BULK_MAX_PACKET;
U8 USBD_Bulk_BulkInBuf[USBD_BULK_MAX_PACKET];
U8 USBD_Bulk_BulkOutBuf[USBD_BULK_MAX_PACKET];
#endif

/*------------------------------------------------------------------------------
 *      USB Device Override Event Handler Fuctions
 *----------------------------------------------------------------------------*/

#if    (USBD_HID_ENABLE)
#ifndef __RTX
void USBD_Configure_Event(void)
{
    USBD_HID_Configure_Event();
}
#endif
#ifdef __RTX
#if   ((USBD_HID_EP_INTOUT != 0) && (USBD_HID_EP_INTIN != USBD_HID_EP_INTOUT))
#if    (USBD_HID_EP_INTIN == 1)
#define USBD_RTX_EndPoint1             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 2)
#define USBD_RTX_EndPoint2             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 3)
#define USBD_RTX_EndPoint3             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 4)
#define USBD_RTX_EndPoint4             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 5)
#define USBD_RTX_EndPoint5             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 6)
#define USBD_RTX_EndPoint6             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 7)
#define USBD_RTX_EndPoint7             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 8)
#define USBD_RTX_EndPoint8             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 9)
#define USBD_RTX_EndPoint9             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 10)
#define USBD_RTX_EndPoint10            USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 11)
#define USBD_RTX_EndPoint11            USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 12)
#define USBD_RTX_EndPoint12            USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 13)
#define USBD_RTX_EndPoint13            USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 14)
#define USBD_RTX_EndPoint14            USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 15)
#define USBD_RTX_EndPoint15            USBD_RTX_HID_EP_INTIN_Event
#endif

#if    (USBD_HID_EP_INTOUT == 1)
#define USBD_RTX_EndPoint1             USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 2)
#define USBD_RTX_EndPoint2             USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 3)
#define USBD_RTX_EndPoint3             USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 4)
#define USBD_RTX_EndPoint4             USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 5)
#define USBD_RTX_EndPoint5             USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 6)
#define USBD_RTX_EndPoint6             USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 7)
#define USBD_RTX_EndPoint7             USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 8)
#define USBD_RTX_EndPoint8             USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 9)
#define USBD_RTX_EndPoint9             USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 10)
#define USBD_RTX_EndPoint10            USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 11)
#define USBD_RTX_EndPoint11            USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 12)
#define USBD_RTX_EndPoint12            USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 13)
#define USBD_RTX_EndPoint13            USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 14)
#define USBD_RTX_EndPoint14            USBD_RTX_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 15)
#define USBD_RTX_EndPoint15            USBD_RTX_HID_EP_INTOUT_Event
#endif
#elif    (USBD_HID_EP_INTOUT != 0)
#if    (USBD_HID_EP_INTIN == 1)
#define USBD_RTX_EndPoint1             USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 2)
#define USBD_RTX_EndPoint2             USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 3)
#define USBD_RTX_EndPoint3             USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 4)
#define USBD_RTX_EndPoint4             USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 5)
#define USBD_RTX_EndPoint5             USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 6)
#define USBD_RTX_EndPoint6             USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 7)
#define USBD_RTX_EndPoint7             USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 8)
#define USBD_RTX_EndPoint8             USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 9)
#define USBD_RTX_EndPoint9             USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 10)
#define USBD_RTX_EndPoint10            USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 11)
#define USBD_RTX_EndPoint11            USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 12)
#define USBD_RTX_EndPoint12            USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 13)
#define USBD_RTX_EndPoint13            USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 14)
#define USBD_RTX_EndPoint14            USBD_RTX_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 15)
#define USBD_RTX_EndPoint15            USBD_RTX_HID_EP_INT_Event
#endif
#else
#if    (USBD_HID_EP_INTIN == 1)
#define USBD_RTX_EndPoint1             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 2)
#define USBD_RTX_EndPoint2             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 3)
#define USBD_RTX_EndPoint3             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 4)
#define USBD_RTX_EndPoint4             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 5)
#define USBD_RTX_EndPoint5             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 6)
#define USBD_RTX_EndPoint6             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 7)
#define USBD_RTX_EndPoint7             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 8)
#define USBD_RTX_EndPoint8             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 9)
#define USBD_RTX_EndPoint9             USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 10)
#define USBD_RTX_EndPoint10            USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 11)
#define USBD_RTX_EndPoint11            USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 12)
#define USBD_RTX_EndPoint12            USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 13)
#define USBD_RTX_EndPoint13            USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 14)
#define USBD_RTX_EndPoint14            USBD_RTX_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 15)
#define USBD_RTX_EndPoint15            USBD_RTX_HID_EP_INTIN_Event
#endif
#endif
#else
#if   (USBD_HID_EP_INTIN != USBD_HID_EP_INTOUT)
#if    (USBD_HID_EP_INTIN == 1)
#define USBD_EndPoint1                 USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 2)
#define USBD_EndPoint2                 USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 3)
#define USBD_EndPoint3                 USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 4)
#define USBD_EndPoint4                 USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 5)
#define USBD_EndPoint5                 USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 6)
#define USBD_EndPoint6                 USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 7)
#define USBD_EndPoint7                 USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 8)
#define USBD_EndPoint8                 USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 9)
#define USBD_EndPoint9                 USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 10)
#define USBD_EndPoint10                USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 11)
#define USBD_EndPoint11                USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 12)
#define USBD_EndPoint12                USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 13)
#define USBD_EndPoint13                USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 14)
#define USBD_EndPoint14                USBD_HID_EP_INTIN_Event
#elif  (USBD_HID_EP_INTIN == 15)
#define USBD_EndPoint15                USBD_HID_EP_INTIN_Event
#endif

#if    (USBD_HID_EP_INTOUT == 1)
#define USBD_EndPoint1                 USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 2)
#define USBD_EndPoint2                 USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 3)
#define USBD_EndPoint3                 USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 4)
#define USBD_EndPoint4                 USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 5)
#define USBD_EndPoint5                 USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 6)
#define USBD_EndPoint6                 USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 7)
#define USBD_EndPoint7                 USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 8)
#define USBD_EndPoint8                 USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 9)
#define USBD_EndPoint9                 USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 10)
#define USBD_EndPoint10                USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 11)
#define USBD_EndPoint11                USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 12)
#define USBD_EndPoint12                USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 13)
#define USBD_EndPoint13                USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 14)
#define USBD_EndPoint14                USBD_HID_EP_INTOUT_Event
#elif  (USBD_HID_EP_INTOUT == 15)
#define USBD_EndPoint15                USBD_HID_EP_INTOUT_Event
#endif

#else

#if    (USBD_HID_EP_INTIN == 1)
#define USBD_EndPoint1                 USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 2)
#define USBD_EndPoint2                 USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 3)
#define USBD_EndPoint3                 USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 4)
#define USBD_EndPoint4                 USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 5)
#define USBD_EndPoint5                 USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 6)
#define USBD_EndPoint6                 USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 7)
#define USBD_EndPoint7                 USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 8)
#define USBD_EndPoint8                 USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 9)
#define USBD_EndPoint9                 USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 10)
#define USBD_EndPoint10                USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 11)
#define USBD_EndPoint11                USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 12)
#define USBD_EndPoint12                USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 13)
#define USBD_EndPoint13                USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 14)
#define USBD_EndPoint14                USBD_HID_EP_INT_Event
#elif  (USBD_HID_EP_INTIN == 15)
#define USBD_EndPoint15                USBD_HID_EP_INT_Event
#endif

#endif /* (USBD_HID_EP_INTIN != USBD_HID_EP_INTOUT) */

#endif /*#ifdef __RTX*/
#else
BOOL USBD_ReqGetDescriptor_HID(U8 **pD, U32 *len)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Setup_HID_ReqToIF(void)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Out_HID_ReqToIF(void)
{
    return (__FALSE);
}
#endif  /* (USBD_HID_ENABLE) */

#if    (USBD_MSC_ENABLE)
#ifdef __RTX
#if    (USBD_MSC_EP_BULKIN != USBD_MSC_EP_BULKOUT)
#if    (USBD_MSC_EP_BULKIN == 1)
#define USBD_RTX_EndPoint1             USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 2)
#define USBD_RTX_EndPoint2             USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 3)
#define USBD_RTX_EndPoint3             USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 4)
#define USBD_RTX_EndPoint4             USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 5)
#define USBD_RTX_EndPoint5             USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 6)
#define USBD_RTX_EndPoint6             USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 7)
#define USBD_RTX_EndPoint7             USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 8)
#define USBD_RTX_EndPoint8             USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 9)
#define USBD_RTX_EndPoint9             USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 10)
#define USBD_RTX_EndPoint10            USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 11)
#define USBD_RTX_EndPoint11            USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 12)
#define USBD_RTX_EndPoint12            USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 13)
#define USBD_RTX_EndPoint13            USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 14)
#define USBD_RTX_EndPoint14            USBD_RTX_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 15)
#define USBD_RTX_EndPoint15            USBD_RTX_MSC_EP_BULKIN_Event
#endif

#if    (USBD_MSC_EP_BULKOUT == 1)
#define USBD_RTX_EndPoint1             USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 2)
#define USBD_RTX_EndPoint2             USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 3)
#define USBD_RTX_EndPoint3             USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 4)
#define USBD_RTX_EndPoint4             USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 5)
#define USBD_RTX_EndPoint5             USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 6)
#define USBD_RTX_EndPoint6             USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 7)
#define USBD_RTX_EndPoint7             USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 8)
#define USBD_RTX_EndPoint8             USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 9)
#define USBD_RTX_EndPoint9             USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 10)
#define USBD_RTX_EndPoint10            USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 11)
#define USBD_RTX_EndPoint11            USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 12)
#define USBD_RTX_EndPoint12            USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 13)
#define USBD_RTX_EndPoint13            USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 14)
#define USBD_RTX_EndPoint14            USBD_RTX_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 15)
#define USBD_RTX_EndPoint15            USBD_RTX_MSC_EP_BULKOUT_Event
#endif
#else
#if    (USBD_MSC_EP_BULKIN == 1)
#define USBD_RTX_EndPoint1             USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 2)
#define USBD_RTX_EndPoint2             USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 3)
#define USBD_RTX_EndPoint3             USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 4)
#define USBD_RTX_EndPoint4             USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 5)
#define USBD_RTX_EndPoint5             USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 6)
#define USBD_RTX_EndPoint6             USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 7)
#define USBD_RTX_EndPoint7             USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 8)
#define USBD_RTX_EndPoint8             USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 9)
#define USBD_RTX_EndPoint9             USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 10)
#define USBD_RTX_EndPoint10            USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 11)
#define USBD_RTX_EndPoint11            USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 12)
#define USBD_RTX_EndPoint12            USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 13)
#define USBD_RTX_EndPoint13            USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 14)
#define USBD_RTX_EndPoint14            USBD_RTX_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 15)
#define USBD_RTX_EndPoint15            USBD_RTX_MSC_EP_BULK_Event
#endif
#endif
#else
#if    (USBD_MSC_EP_BULKIN != USBD_MSC_EP_BULKOUT)
#if    (USBD_MSC_EP_BULKIN == 1)
#define USBD_EndPoint1                 USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 2)
#define USBD_EndPoint2                 USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 3)
#define USBD_EndPoint3                 USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 4)
#define USBD_EndPoint4                 USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 5)
#define USBD_EndPoint5                 USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 6)
#define USBD_EndPoint6                 USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 7)
#define USBD_EndPoint7                 USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 8)
#define USBD_EndPoint8                 USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 9)
#define USBD_EndPoint9                 USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 10)
#define USBD_EndPoint10                USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 11)
#define USBD_EndPoint11                USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 12)
#define USBD_EndPoint12                USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 13)
#define USBD_EndPoint13                USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 14)
#define USBD_EndPoint14                USBD_MSC_EP_BULKIN_Event
#elif  (USBD_MSC_EP_BULKIN == 15)
#define USBD_EndPoint15                USBD_MSC_EP_BULKIN_Event
#endif

#if    (USBD_MSC_EP_BULKOUT == 1)
#define USBD_EndPoint1                 USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 2)
#define USBD_EndPoint2                 USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 3)
#define USBD_EndPoint3                 USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 4)
#define USBD_EndPoint4                 USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 5)
#define USBD_EndPoint5                 USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 6)
#define USBD_EndPoint6                 USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 7)
#define USBD_EndPoint7                 USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 8)
#define USBD_EndPoint8                 USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 9)
#define USBD_EndPoint9                 USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 10)
#define USBD_EndPoint10                USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 11)
#define USBD_EndPoint11                USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 12)
#define USBD_EndPoint12                USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 13)
#define USBD_EndPoint13                USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 14)
#define USBD_EndPoint14                USBD_MSC_EP_BULKOUT_Event
#elif  (USBD_MSC_EP_BULKOUT == 15)
#define USBD_EndPoint15                USBD_MSC_EP_BULKOUT_Event
#endif
#else
#if    (USBD_MSC_EP_BULKIN == 1)
#define USBD_EndPoint1                 USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 2)
#define USBD_EndPoint2                 USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 3)
#define USBD_EndPoint3                 USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 4)
#define USBD_EndPoint4                 USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 5)
#define USBD_EndPoint5                 USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 6)
#define USBD_EndPoint6                 USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 7)
#define USBD_EndPoint7                 USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 8)
#define USBD_EndPoint8                 USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 9)
#define USBD_EndPoint9                 USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 10)
#define USBD_EndPoint10                USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 11)
#define USBD_EndPoint11                USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 12)
#define USBD_EndPoint12                USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 13)
#define USBD_EndPoint13                USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 14)
#define USBD_EndPoint14                USBD_MSC_EP_BULK_Event
#elif  (USBD_MSC_EP_BULKIN == 15)
#define USBD_EndPoint15                USBD_MSC_EP_BULK_Event
#endif
#endif
#endif
#else
void USBD_ReqClrFeature_MSC(U32 EPNum)
{

}
BOOL USBD_EndPoint0_Setup_MSC_ReqToIF(void)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Out_MSC_ReqToIF(void)
{
    return (__FALSE);
}
#endif  /* (USBD_MSC_ENABLE) */

#if    (USBD_ADC_ENABLE == 0)
BOOL USBD_EndPoint0_Setup_ADC_ReqToIF(void)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Setup_ADC_ReqToEP(void)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Out_ADC_ReqToIF(void)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Out_ADC_ReqToEP(void)
{
    return (__FALSE);
}
#endif  /* (USBD_ADC_ENABLE) */

#if    (USBD_CDC_ACM_ENABLE)
#ifdef __RTX
#if    (USBD_CDC_ACM_EP_INTIN == 1)
#define USBD_RTX_EndPoint1               USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 2)
#define USBD_RTX_EndPoint2               USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 3)
#define USBD_RTX_EndPoint3               USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 4)
#define USBD_RTX_EndPoint4               USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 5)
#define USBD_RTX_EndPoint5               USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 6)
#define USBD_RTX_EndPoint6               USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 7)
#define USBD_RTX_EndPoint7               USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 8)
#define USBD_RTX_EndPoint8               USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 9)
#define USBD_RTX_EndPoint9               USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 10)
#define USBD_RTX_EndPoint10              USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 11)
#define USBD_RTX_EndPoint11              USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 12)
#define USBD_RTX_EndPoint12              USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 13)
#define USBD_RTX_EndPoint13              USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 14)
#define USBD_RTX_EndPoint14              USBD_RTX_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 15)
#define USBD_RTX_EndPoint15              USBD_RTX_CDC_ACM_EP_INTIN_Event
#endif
#else
#if    (USBD_CDC_ACM_EP_INTIN == 1)
#define USBD_EndPoint1                   USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 2)
#define USBD_EndPoint2                   USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 3)
#define USBD_EndPoint3                   USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 4)
#define USBD_EndPoint4                   USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 5)
#define USBD_EndPoint5                   USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 6)
#define USBD_EndPoint6                   USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 7)
#define USBD_EndPoint7                   USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 8)
#define USBD_EndPoint8                   USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 9)
#define USBD_EndPoint9                   USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 10)
#define USBD_EndPoint10                  USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 11)
#define USBD_EndPoint11                  USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 12)
#define USBD_EndPoint12                  USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 13)
#define USBD_EndPoint13                  USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 14)
#define USBD_EndPoint14                  USBD_CDC_ACM_EP_INTIN_Event
#elif  (USBD_CDC_ACM_EP_INTIN == 15)
#define USBD_EndPoint15                  USBD_CDC_ACM_EP_INTIN_Event
#endif
#endif

#ifdef __RTX
#if    (USBD_CDC_ACM_EP_BULKIN != USBD_CDC_ACM_EP_BULKOUT)
#if    (USBD_CDC_ACM_EP_BULKIN == 1)
#define USBD_RTX_EndPoint1             USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 2)
#define USBD_RTX_EndPoint2             USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 3)
#define USBD_RTX_EndPoint3             USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 4)
#define USBD_RTX_EndPoint4             USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 5)
#define USBD_RTX_EndPoint5             USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 6)
#define USBD_RTX_EndPoint6             USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 7)
#define USBD_RTX_EndPoint7             USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 8)
#define USBD_RTX_EndPoint8             USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 9)
#define USBD_RTX_EndPoint9             USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 10)
#define USBD_RTX_EndPoint10            USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 11)
#define USBD_RTX_EndPoint11            USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 12)
#define USBD_RTX_EndPoint12            USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 13)
#define USBD_RTX_EndPoint13            USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 14)
#define USBD_RTX_EndPoint14            USBD_RTX_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 15)
#define USBD_RTX_EndPoint15            USBD_RTX_CDC_ACM_EP_BULKIN_Event
#endif

#if    (USBD_CDC_ACM_EP_BULKOUT == 1)
#define USBD_RTX_EndPoint1             USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 2)
#define USBD_RTX_EndPoint2             USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 3)
#define USBD_RTX_EndPoint3             USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 4)
#define USBD_RTX_EndPoint4             USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 5)
#define USBD_RTX_EndPoint5             USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 6)
#define USBD_RTX_EndPoint6             USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 7)
#define USBD_RTX_EndPoint7             USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 8)
#define USBD_RTX_EndPoint8             USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 9)
#define USBD_RTX_EndPoint9             USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 10)
#define USBD_RTX_EndPoint10            USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 11)
#define USBD_RTX_EndPoint11            USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 12)
#define USBD_RTX_EndPoint12            USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 13)
#define USBD_RTX_EndPoint13            USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 14)
#define USBD_RTX_EndPoint14            USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 15)
#define USBD_RTX_EndPoint15            USBD_RTX_CDC_ACM_EP_BULKOUT_Event
#endif
#else
#if    (USBD_CDC_ACM_EP_BULKIN == 1)
#define USBD_RTX_EndPoint1             USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 2)
#define USBD_RTX_EndPoint2             USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 3)
#define USBD_RTX_EndPoint3             USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 4)
#define USBD_RTX_EndPoint4             USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 5)
#define USBD_RTX_EndPoint5             USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 6)
#define USBD_RTX_EndPoint6             USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 7)
#define USBD_RTX_EndPoint7             USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 8)
#define USBD_RTX_EndPoint8             USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 9)
#define USBD_RTX_EndPoint9             USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 10)
#define USBD_RTX_EndPoint10            USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 11)
#define USBD_RTX_EndPoint11            USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 12)
#define USBD_RTX_EndPoint12            USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 13)
#define USBD_RTX_EndPoint13            USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 14)
#define USBD_RTX_EndPoint14            USBD_RTX_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 15)
#define USBD_RTX_EndPoint15            USBD_RTX_CDC_ACM_EP_BULK_Event
#endif
#endif
#else
#if    (USBD_CDC_ACM_EP_BULKIN != USBD_CDC_ACM_EP_BULKOUT)
#if    (USBD_CDC_ACM_EP_BULKIN == 1)
#define USBD_EndPoint1                 USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 2)
#define USBD_EndPoint2                 USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 3)
#define USBD_EndPoint3                 USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 4)
#define USBD_EndPoint4                 USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 5)
#define USBD_EndPoint5                 USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 6)
#define USBD_EndPoint6                 USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 7)
#define USBD_EndPoint7                 USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 8)
#define USBD_EndPoint8                 USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 9)
#define USBD_EndPoint9                 USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 10)
#define USBD_EndPoint10                USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 11)
#define USBD_EndPoint11                USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 12)
#define USBD_EndPoint12                USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 13)
#define USBD_EndPoint13                USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 14)
#define USBD_EndPoint14                USBD_CDC_ACM_EP_BULKIN_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 15)
#define USBD_EndPoint15                USBD_CDC_ACM_EP_BULKIN_Event
#endif

#if    (USBD_CDC_ACM_EP_BULKOUT == 1)
#define USBD_EndPoint1                 USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 2)
#define USBD_EndPoint2                 USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 3)
#define USBD_EndPoint3                 USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 4)
#define USBD_EndPoint4                 USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 5)
#define USBD_EndPoint5                 USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 6)
#define USBD_EndPoint6                 USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 7)
#define USBD_EndPoint7                 USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 8)
#define USBD_EndPoint8                 USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 9)
#define USBD_EndPoint9                 USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 10)
#define USBD_EndPoint10                USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 11)
#define USBD_EndPoint11                USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 12)
#define USBD_EndPoint12                USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 13)
#define USBD_EndPoint13                USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 14)
#define USBD_EndPoint14                USBD_CDC_ACM_EP_BULKOUT_Event
#elif  (USBD_CDC_ACM_EP_BULKOUT == 15)
#define USBD_EndPoint15                USBD_CDC_ACM_EP_BULKOUT_Event
#endif
#else
#if    (USBD_CDC_ACM_EP_BULKIN == 1)
#define USBD_EndPoint1                 USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 2)
#define USBD_EndPoint2                 USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 3)
#define USBD_EndPoint3                 USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 4)
#define USBD_EndPoint4                 USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 5)
#define USBD_EndPoint5                 USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 6)
#define USBD_EndPoint6                 USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 7)
#define USBD_EndPoint7                 USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 8)
#define USBD_EndPoint8                 USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 9)
#define USBD_EndPoint9                 USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 10)
#define USBD_EndPoint10                USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 11)
#define USBD_EndPoint11                USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 12)
#define USBD_EndPoint12                USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 13)
#define USBD_EndPoint13                USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 14)
#define USBD_EndPoint14                USBD_CDC_ACM_EP_BULK_Event
#elif  (USBD_CDC_ACM_EP_BULKIN == 15)
#define USBD_EndPoint15                USBD_CDC_ACM_EP_BULK_Event
#endif
#endif
#endif
#else
BOOL USBD_EndPoint0_Setup_CDC_ReqToIF(void)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Out_CDC_ReqToIF(void)
{
    return (__FALSE);
}
#endif  /* (USBD_CDC_ACM_ENABLE) */

#if    (USBD_BULK_ENABLE)

#if    (USBD_BULK_EP_BULKIN != USBD_BULK_EP_BULKOUT)

#if    (USBD_BULK_EP_BULKIN == 1)
#define USBD_EndPoint1                 USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 2)
#define USBD_EndPoint2                 USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 3)
#define USBD_EndPoint3                 USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 4)
#define USBD_EndPoint4                 USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 5)
#define USBD_EndPoint5                 USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 6)
#define USBD_EndPoint6                 USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 7)
#define USBD_EndPoint7                 USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 8)
#define USBD_EndPoint8                 USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 9)
#define USBD_EndPoint9                 USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 10)
#define USBD_EndPoint10                USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 11)
#define USBD_EndPoint11                USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 12)
#define USBD_EndPoint12                USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 13)
#define USBD_EndPoint13                USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 14)
#define USBD_EndPoint14                USBD_BULK_EP_BULKIN_Event
#elif  (USBD_BULK_EP_BULKIN == 15)
#define USBD_EndPoint15                USBD_BULK_EP_BULKIN_Event
#endif

#if    (USBD_BULK_EP_BULKOUT == 1)
#define USBD_EndPoint1                 USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 2)
#define USBD_EndPoint2                 USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 3)
#define USBD_EndPoint3                 USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 4)
#define USBD_EndPoint4                 USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 5)
#define USBD_EndPoint5                 USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 6)
#define USBD_EndPoint6                 USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 7)
#define USBD_EndPoint7                 USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 8)
#define USBD_EndPoint8                 USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 9)
#define USBD_EndPoint9                 USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 10)
#define USBD_EndPoint10                USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 11)
#define USBD_EndPoint11                USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 12)
#define USBD_EndPoint12                USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 13)
#define USBD_EndPoint13                USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 14)
#define USBD_EndPoint14                USBD_BULK_EP_BULKOUT_Event
#elif  (USBD_BULK_EP_BULKOUT == 15)
#define USBD_EndPoint15                USBD_BULK_EP_BULKOUT_Event
#endif

#else
#if    (USBD_BULK_EP_BULKIN == 1)
#define USBD_EndPoint1                 USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 2)
#define USBD_EndPoint2                 USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 3)
#define USBD_EndPoint3                 USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 4)
#define USBD_EndPoint4                 USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 5)
#define USBD_EndPoint5                 USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 6)
#define USBD_EndPoint6                 USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 7)
#define USBD_EndPoint7                 USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 8)
#define USBD_EndPoint8                 USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 9)
#define USBD_EndPoint9                 USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 10)
#define USBD_EndPoint10                USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 11)
#define USBD_EndPoint11                USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 12)
#define USBD_EndPoint12                USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 13)
#define USBD_EndPoint13                USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 14)
#define USBD_EndPoint14                USBD_BULK_EP_BULK_Event
#elif  (USBD_BULK_EP_BULKIN == 15)
#define USBD_EndPoint15                USBD_BULK_EP_BULK_Event
#endif
#endif

#endif  /* (USBD_BULK_ENABLE) */

#if    (USBD_CLS_ENABLE)
#else
BOOL USBD_EndPoint0_Setup_CLS_ReqToDEV(void)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Setup_CLS_ReqToIF(void)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Setup_CLS_ReqToEP(void)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Out_CLS_ReqToDEV(void)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Out_CLS_ReqToIF(void)
{
    return (__FALSE);
}
BOOL USBD_EndPoint0_Out_CLS_ReqToEP(void)
{
    return (__FALSE);
}
#endif  /* (USBD_CLS_ENABLE) */

#if   ((USBD_CDC_ACM_ENABLE))
#ifndef __RTX
void USBD_Reset_Event(void)
{
#if    (USBD_CDC_ACM_ENABLE)
    USBD_CDC_ACM_Reset_Event();
#endif
#if    (USBD_MSC_ENABLE)
    USBD_MSC_Reset_Event();
#endif    
}
#endif
#endif  /* ((USBD_CDC_ACM_ENABLE)) */

#if   ((USBD_HID_ENABLE) || (USBD_ADC_ENABLE) || (USBD_CDC_ACM_ENABLE) || (USBD_CLS_ENABLE))
#ifndef __RTX
void USBD_SOF_Event(void)
{
#if    (USBD_HID_ENABLE)
    USBD_HID_SOF_Event();
#endif
#if    (USBD_ADC_ENABLE)
    USBD_ADC_SOF_Event();
#endif
#if    (USBD_CDC_ACM_ENABLE)
    USBD_CDC_ACM_SOF_Event();
#endif
#if    (USBD_CLS_ENABLE)
    USBD_CLS_SOF_Event();
#endif
}
#endif
#endif  /* ((USBD_HID_ENABLE) || (USBD_ADC_ENABLE) || (USBD_CDC_ACM_ENABLE) || (USBD_CLS_ENABLE)) */

/* USB Device - Device Events Callback Functions */
__weak void USBD_Power_Event(BOOL power);
__weak void USBD_Reset_Event(void);
__weak void USBD_Suspend_Event(void);
__weak void USBD_Resume_Event(void);
__weak void USBD_WakeUp_Event(void);
__weak void USBD_SOF_Event(void);
__weak void USBD_Error_Event(U32 error);

/* USB Device - Device Events Callback Pointers */
void (* const USBD_P_Power_Event)(BOOL power) = USBD_Power_Event;
void (* const USBD_P_Reset_Event)(void) = USBD_Reset_Event;
void (* const USBD_P_Suspend_Event)(void) = USBD_Suspend_Event;
void (* const USBD_P_Resume_Event)(void) = USBD_Resume_Event;
void (* const USBD_P_WakeUp_Event)(void) = USBD_WakeUp_Event;
void (* const USBD_P_SOF_Event)(void) = USBD_SOF_Event;
void (* const USBD_P_Error_Event)(U32 error) = USBD_Error_Event;

/* USB Device - Endpoint Events Callback Functions */
extern void USBD_EndPoint0(U32 event);
#ifndef       USBD_EndPoint1
__weak void USBD_EndPoint1(U32 event);
#endif
#ifndef       USBD_EndPoint2
__weak void USBD_EndPoint2(U32 event);
#endif
#ifndef       USBD_EndPoint3
__weak void USBD_EndPoint3(U32 event);
#endif
#ifndef       USBD_EndPoint4
__weak void USBD_EndPoint4(U32 event);
#endif
#ifndef       USBD_EndPoint5
__weak void USBD_EndPoint5(U32 event);
#endif
#ifndef       USBD_EndPoint6
__weak void USBD_EndPoint6(U32 event);
#endif
#ifndef       USBD_EndPoint7
__weak void USBD_EndPoint7(U32 event);
#endif
#ifndef       USBD_EndPoint8
__weak void USBD_EndPoint8(U32 event);
#endif
#ifndef       USBD_EndPoint9
__weak void USBD_EndPoint9(U32 event);
#endif
#ifndef       USBD_EndPoint10
__weak void USBD_EndPoint10(U32 event);
#endif
#ifndef       USBD_EndPoint11
__weak void USBD_EndPoint11(U32 event);
#endif
#ifndef       USBD_EndPoint12
__weak void USBD_EndPoint12(U32 event);
#endif
#ifndef       USBD_EndPoint13
__weak void USBD_EndPoint13(U32 event);
#endif
#ifndef       USBD_EndPoint14
__weak void USBD_EndPoint14(U32 event);
#endif
#ifndef       USBD_EndPoint15
__weak void USBD_EndPoint15(U32 event);
#endif

/* USB Device - Endpoint Events Callback Pointers */
void (* const USBD_P_EP[16])(U32 event) = {
    USBD_EndPoint0,
    USBD_EndPoint1,
    USBD_EndPoint2,
    USBD_EndPoint3,
    USBD_EndPoint4,
    USBD_EndPoint5,
    USBD_EndPoint6,
    USBD_EndPoint7,
    USBD_EndPoint8,
    USBD_EndPoint9,
    USBD_EndPoint10,
    USBD_EndPoint11,
    USBD_EndPoint12,
    USBD_EndPoint13,
    USBD_EndPoint14,
    USBD_EndPoint15
};

/* USB Device - Core Events Callback Functions */
__weak void USBD_Configure_Event(void);
__weak void USBD_Interface_Event(void);
__weak void USBD_Feature_Event(void);

/* USB Device - Core Events Callback Pointers */
void (* const USBD_P_Configure_Event)(void) = USBD_Configure_Event;
void (* const USBD_P_Interface_Event)(void) = USBD_Interface_Event;
void (* const USBD_P_Feature_Event)(void) = USBD_Feature_Event;

#ifdef __RTX
const BOOL __rtx = __TRUE;

#if   ((USBD_HID_ENABLE) || (USBD_ADC_ENABLE) || (USBD_CDC_ACM_ENABLE) || (USBD_CLS_ENABLE))
__weak void USBD_RTX_Device(void)
{
    U16 evt;

    for (;;) {
        os_evt_wait_or(0xFFFF, 0xFFFF);         /* Wait for an Event */
        evt = os_evt_get();                     /* Get Event Flags */

        if (evt & USBD_EVT_RESET) {
#if (USBD_CDC_ACM_ENABLE)
            USBD_CDC_ACM_Reset_Event();
#endif
#if (USBD_MSC_ENABLE)
            USBD_MSC_Reset_Event();
#endif    
        }

        if (evt & USBD_EVT_SOF) {
#if (USBD_HID_ENABLE)
            USBD_HID_SOF_Event();
#endif
#if (USBD_ADC_ENABLE)
            USBD_ADC_SOF_Event();
#endif
#if (USBD_CDC_ACM_ENABLE)
            USBD_CDC_ACM_SOF_Event();
#endif
#if (USBD_CLS_ENABLE)
            USBD_CLS_SOF_Event();
#endif
        }
    }
}
#else
__weak void USBD_RTX_Device(void);
#endif

/* USB Device - Device Events Callback Pointer */
void (* const USBD_RTX_P_Device)(void) = USBD_RTX_Device;

/* USB Device Endpoint Events Callback Functions */
extern void USBD_RTX_EndPoint0(void);
#ifndef       USBD_RTX_EndPoint1
__weak void USBD_RTX_EndPoint1(void);
#endif
#ifndef       USBD_RTX_EndPoint2
__weak void USBD_RTX_EndPoint2(void);
#endif
#ifndef       USBD_RTX_EndPoint3
__weak void USBD_RTX_EndPoint3(void);
#endif
#ifndef       USBD_RTX_EndPoint4
__weak void USBD_RTX_EndPoint4(void);
#endif
#ifndef       USBD_RTX_EndPoint5
__weak void USBD_RTX_EndPoint5(void);
#endif
#ifndef       USBD_RTX_EndPoint6
__weak void USBD_RTX_EndPoint6(void);
#endif
#ifndef       USBD_RTX_EndPoint7
__weak void USBD_RTX_EndPoint7(void);
#endif
#ifndef       USBD_RTX_EndPoint8
__weak void USBD_RTX_EndPoint8(void);
#endif
#ifndef       USBD_RTX_EndPoint9
__weak void USBD_RTX_EndPoint9(void);
#endif
#ifndef       USBD_RTX_EndPoint10
__weak void USBD_RTX_EndPoint10(void);
#endif
#ifndef       USBD_RTX_EndPoint11
__weak void USBD_RTX_EndPoint11(void);
#endif
#ifndef       USBD_RTX_EndPoint12
__weak void USBD_RTX_EndPoint12(void);
#endif
#ifndef       USBD_RTX_EndPoint13
__weak void USBD_RTX_EndPoint13(void);
#endif
#ifndef       USBD_RTX_EndPoint14
__weak void USBD_RTX_EndPoint14(void);
#endif
#ifndef       USBD_RTX_EndPoint15
__weak void USBD_RTX_EndPoint15(void);
#endif

#if    (USBD_HID_ENABLE)
__weak void USBD_RTX_Core(void)
{
    U16 evt;

    for (;;) {
        os_evt_wait_or(0xFFFF, 0xFFFF);         /* Wait for an Event */
        evt = os_evt_get();                     /* Get Event Flags */

        if (evt & USBD_EVT_SET_CFG) {
            USBD_HID_Configure_Event();
        }
    }
}
#else
__weak void USBD_RTX_Core(void);
#endif

/* USB Device - Core Events Callback Pointer */
void (* const USBD_RTX_P_Core)(void)  = USBD_RTX_Core;

/* USB Device Endpoint Events Callback Pointers */
void (* const USBD_RTX_P_EP[16])(void) = {
    USBD_RTX_EndPoint0,
    USBD_RTX_EndPoint1,
    USBD_RTX_EndPoint2,
    USBD_RTX_EndPoint3,
    USBD_RTX_EndPoint4,
    USBD_RTX_EndPoint5,
    USBD_RTX_EndPoint6,
    USBD_RTX_EndPoint7,
    USBD_RTX_EndPoint8,
    USBD_RTX_EndPoint9,
    USBD_RTX_EndPoint10,
    USBD_RTX_EndPoint11,
    USBD_RTX_EndPoint12,
    USBD_RTX_EndPoint13,
    USBD_RTX_EndPoint14,
    USBD_RTX_EndPoint15,
};

void usbd_os_evt_set(U16 event_flags, U32 task)
{
    if (task) {
        os_evt_set(event_flags, task);
    }
}
U16  usbd_os_evt_get(void)
{
    return (os_evt_get());
}
U32  usbd_os_evt_wait_or(U16 wait_flags, U16 timeout)
{
    return (os_evt_wait_or(wait_flags, timeout));
}

#else
const BOOL __rtx = __FALSE;

void usbd_os_evt_set(U16 event_flags, U32 task)
{
    
}
U16  usbd_os_evt_get(void)
{
    return (0);
}
U32  usbd_os_evt_wait_or(U16 wait_flags, U16 timeout)
{
    return (0);
}
#endif

#ifdef __RTX

typedef struct {
    void *stack;
    U16 size;
} user_stack_t;

#if USBD_ENABLE && !defined(USBD_RTX_CORE_STACK)
#error "USB core stack must be defined"
#endif
#if USBD_ENABLE && !defined(USBD_RTX_DEVICE_STACK)
#error "USB device stack must be defined"
#endif
#if USBD_ENABLE && !defined(USBD_RTX_ENDPOINT0_STACK)
#error "USB endpoint 0 must be defined"
#endif

#if !defined(USBD_HID_EP_INTIN_STACK)
#define USBD_HID_EP_INTIN_STACK 0
#endif
#if !defined(USBD_HID_EP_INTOUT_STACK)
#define USBD_HID_EP_INTOUT_STACK 0
#endif
#if !defined(USBD_MSC_EP_BULKIN_STACK)
#define USBD_MSC_EP_BULKIN_STACK 0
#endif
#if !defined(USBD_MSC_EP_BULKOUT_STACK)
#define USBD_MSC_EP_BULKOUT_STACK 0
#endif
#if !defined(USBD_ADC_EP_ISOOUT_STACK)
#define USBD_ADC_EP_ISOOUT_STACK 0
#endif
#if !defined(USBD_CDC_ACM_EP_INTIN_STACK)
#define USBD_CDC_ACM_EP_INTIN_STACK 0
#endif
#if !defined(USBD_CDC_ACM_EP_BULKIN_STACK)
#define USBD_CDC_ACM_EP_BULKIN_STACK 0
#endif
#if !defined(USBD_CDC_ACM_EP_BULKOUT_STACK)
#define USBD_CDC_ACM_EP_BULKOUT_STACK 0
#endif

#if USBD_HID_EP_INTIN == 0 && USBD_HID_EP_INTIN_STACK > 0
#error "USBD_HID_EP_INTIN stack unused - must be 0"
#endif
#if USBD_HID_EP_INTOUT == 0 && USBD_HID_EP_INTOUT_STACK > 0
#error "USBD_HID_EP_INTOUT stack unused - must be 0"
#endif
#if USBD_MSC_EP_BULKIN == 0 && USBD_MSC_EP_BULKIN_STACK > 0
#error "USBD_MSC_EP_BULKIN stack unused - must be 0"
#endif
#if USBD_MSC_EP_BULKOUT == 0 && USBD_MSC_EP_BULKOUT_STACK > 0
#error "USBD_MSC_EP_BULKOUT stack unused - must be 0"
#endif
#if USBD_ADC_EP_ISOOUT == 0 && USBD_ADC_EP_ISOOUT_STACK > 0
#error "USBD_ADC_EP_ISOOUT stack unused - must be 0"
#endif
#if USBD_CDC_ACM_EP_INTIN == 0 && USBD_CDC_ACM_EP_INTIN_STACK > 0
#error "USBD_CDC_ACM_EP_INTIN stack unused - must be 0"
#endif
#if USBD_CDC_ACM_EP_BULKIN == 0 && USBD_CDC_ACM_EP_BULKIN_STACK > 0
#error "USBD_CDC_ACM_EP_BULKIN stack unused - must be 0"
#endif
#if USBD_CDC_ACM_EP_BULKOUT == 0 && USBD_CDC_ACM_EP_BULKOUT_STACK > 0
#error "USBD_CDC_ACM_EP_BULKOUT stack unused - must be 0"
#endif

#if USBD_ENABLE
static U64 usbd_core_stack[USBD_RTX_CORE_STACK / 8];
static U64 usbd_device_stack[USBD_RTX_DEVICE_STACK / 8];
static U64 usbd_endpoint0_stack[USBD_RTX_ENDPOINT0_STACK / 8];
#endif

#if (USBD_HID_EP_INTIN_STACK > 0)
static U64 usbd_hid_ep_intin_stack[USBD_HID_EP_INTIN_STACK / 8];
#endif
#if (USBD_HID_EP_INTOUT_STACK > 0)
static U64 usbd_hid_ep_intout_stack[USBD_HID_EP_INTOUT_STACK / 8];
#endif
#if (USBD_MSC_EP_BULKIN_STACK > 0)
static U64 usbd_msc_ep_bulkin_stack[USBD_MSC_EP_BULKIN_STACK / 8];
#endif
#if (USBD_MSC_EP_BULKOUT_STACK > 0)
static U64 usbd_msc_ep_bulkout_stack[USBD_MSC_EP_BULKOUT_STACK / 8];
#endif
#if (USBD_ADC_EP_ISOOUT_STACK > 0)
static U64 usbd_adc_ep_isoout_stack[USBD_ADC_EP_ISOOUT_STACK / 8];
#endif
#if (USBD_CDC_ACM_EP_INTIN_STACK > 0)
static U64 usbd_cdc_acm_ep_intin_stack[USBD_CDC_ACM_EP_INTIN_STACK / 8];
#endif
#if (USBD_CDC_ACM_EP_BULKIN_STACK > 0)
static U64 usbd_cdc_acm_ep_bulkin_stack[USBD_CDC_ACM_EP_BULKIN_STACK / 8];
#endif
#if (USBD_CDC_ACM_EP_BULKOUT_STACK > 0)
static U64 usbd_cdc_acm_ep_bulkout_stack[USBD_CDC_ACM_EP_BULKOUT_STACK / 8];
#endif

// Check HID
#if (USBD_HID_ENABLE && !USBD_HID_EP_INTIN_STACK && USBD_HID_EP_INTIN != USBD_HID_EP_INTOUT)
#error "USBD_HID_EP_INTIN_STACK must be defined"
#endif
#if (USBD_HID_ENABLE && !USBD_HID_EP_INTOUT_STACK && USBD_HID_EP_INTIN != USBD_HID_EP_INTOUT && USBD_HID_EP_INTOUT != 0)
#error "USBD_HID_EP_INTOUT_STACK must be defined"
#endif
#if (USBD_HID_ENABLE && USBD_HID_EP_INTIN_STACK == 0 && USBD_HID_EP_INTOUT_STACK == 0)
#error "HID stack must be defined"
#endif
#if (USBD_HID_EP_INTIN_STACK > 0 && USBD_HID_EP_INTOUT_STACK > 0 && USBD_HID_EP_INTIN == USBD_HID_EP_INTOUT)
#error "Multiple HID stacks defined for same EP"
#endif

// Check MSC
#if (USBD_MSC_ENABLE && !USBD_MSC_EP_BULKIN_STACK && USBD_MSC_EP_BULKIN != USBD_MSC_EP_BULKOUT)
#error "USBD_MSC_EP_BULKIN_STACK must be defined"
#endif
#if (USBD_MSC_ENABLE && !USBD_MSC_EP_BULKOUT_STACK && USBD_MSC_EP_BULKIN != USBD_MSC_EP_BULKOUT)
#error "USBD_MSC_EP_BULKIN_STACK must be defined"
#endif
#if (USBD_MSC_ENABLE && USBD_MSC_EP_BULKIN_STACK == 0 && USBD_MSC_EP_BULKOUT_STACK == 0)
#error "MSC stack must be defined"
#endif
#if (USBD_MSC_EP_BULKIN_STACK > 0 && USBD_MSC_EP_BULKOUT_STACK > 0 && USBD_MSC_EP_BULKIN == USBD_MSC_EP_BULKOUT)
#error "Multiple MSC stacks defined for same EP"
#endif

// Check ADC
#if (USBD_ADC_ENABLE && !USBD_ADC_EP_ISOOUT_STACK)
#error "ADC stack must be defined"
#endif

// Check CDC
#if (USBD_CDC_ACM_ENABLE && !USBD_CDC_ACM_EP_INTIN_STACK)
#error "CDC ACM INTIN stack must be defined"
#endif
#if (USBD_CDC_ACM_ENABLE && !USBD_CDC_ACM_EP_BULKIN_STACK && USBD_CDC_ACM_EP_BULKIN != USBD_CDC_ACM_EP_BULKOUT)
#error "USBD_CDC_ACM_EP_BULKIN must be defined"
#endif
#if (USBD_CDC_ACM_ENABLE && !USBD_CDC_ACM_EP_BULKOUT_STACK && USBD_CDC_ACM_EP_BULKIN != USBD_CDC_ACM_EP_BULKOUT)
#error "USBD_CDC_ACM_EP_BULKOUT must be defined"
#endif
#if (USBD_CDC_ACM_ENABLE && USBD_CDC_ACM_EP_BULKIN_STACK == 0 && USBD_CDC_ACM_EP_BULKOUT_STACK == 0)
#error "CDC BULK stack must be defined"
#endif
#if (USBD_CDC_ACM_EP_BULKIN_STACK > 0 && USBD_CDC_ACM_EP_BULKOUT_STACK > 0 && USBD_CDC_ACM_EP_BULKIN == USBD_CDC_ACM_EP_BULKOUT)
#error "Multiple CDC stacks defined for same EP"
#endif

static const user_stack_t user_stack_list[16] = {
#if USBD_ENABLE
    [0] = {usbd_endpoint0_stack, sizeof(usbd_endpoint0_stack)},
#endif
#if (USBD_HID_EP_INTIN_STACK > 0)
    [USBD_HID_EP_INTIN] = {usbd_hid_ep_intin_stack, sizeof(usbd_hid_ep_intin_stack)},
#endif
#if (USBD_HID_EP_INTOUT_STACK > 0)
    [USBD_HID_EP_INTOUT] = {usbd_hid_ep_intout_stack, sizeof(usbd_hid_ep_intout_stack)},
#endif
#if (USBD_MSC_EP_BULKIN_STACK > 0)
    [USBD_MSC_EP_BULKIN] = {usbd_msc_ep_bulkin_stack, sizeof(usbd_msc_ep_bulkin_stack)},
#endif
#if (USBD_MSC_EP_BULKOUT_STACK > 0)
    [USBD_MSC_EP_BULKOUT] = {usbd_msc_ep_bulkout_stack, sizeof(usbd_msc_ep_bulkout_stack)},
#endif
#if (USBD_ADC_EP_ISOOUT_STACK > 0)
    [USBD_ADC_EP_ISOOUT] = {usbd_adc_ep_isoout_stack, sizeof(usbd_adc_ep_isoout_stack)},
#endif
#if (USBD_CDC_ACM_EP_INTIN_STACK > 0)
    [USBD_CDC_ACM_EP_INTIN] = {usbd_cdc_acm_ep_intin_stack, sizeof(usbd_cdc_acm_ep_intin_stack)},
#endif
#if (USBD_CDC_ACM_EP_BULKIN_STACK > 0)
    [USBD_CDC_ACM_EP_BULKIN] = {usbd_cdc_acm_ep_bulkin_stack, sizeof(usbd_cdc_acm_ep_bulkin_stack)},
#endif
#if (USBD_CDC_ACM_EP_BULKOUT_STACK > 0)
    [USBD_CDC_ACM_EP_BULKOUT] = {usbd_cdc_acm_ep_bulkout_stack, sizeof(usbd_cdc_acm_ep_bulkout_stack)},
#endif
};

#endif /* __RTX */

void USBD_RTX_TaskInit(void)
{
#ifdef __RTX
    /* Initialize memory pools for endpoints */
    U32 i;
    USBD_RTX_DevTask = 0;

    if (USBD_RTX_P_Device) {
        USBD_RTX_DevTask = os_tsk_create_user(USBD_RTX_Device, 3, usbd_device_stack,
                                              sizeof(usbd_device_stack));
    }

    for (i = 0; i <= 15; i++) {
        USBD_RTX_EPTask[i] = 0;

        if (USBD_RTX_P_EP[i]) {
            // Set the control endpoint (endpoint 0) thread to a high priority.
            // According to the USB 2.0 specification, the time to complete standard
            // request with no data must be under 50ms.  If a long caluclation
            // such as a CRC is running on a higher priority thread, USB enumeration
            // can fail because of this timeout.
            uint8_t priority = 0 == i ? 200 : 20;
            USBD_RTX_EPTask[i] = os_tsk_create_user(USBD_RTX_P_EP[i], priority, user_stack_list[i].stack,
                                                    user_stack_list[i].size);
        }
    }

    USBD_RTX_CoreTask = 0;

    if (USBD_RTX_P_Core) {
        USBD_RTX_CoreTask = os_tsk_create_user(USBD_RTX_Core, 2, usbd_core_stack,
                                               sizeof(usbd_core_stack));
    }

#endif
}

/*------------------------------------------------------------------------------
 *      CDC Sizes
 *----------------------------------------------------------------------------*/
#define CDC_HEADER_SIZE                         5
#define CDC_CALL_MANAGEMENT_SIZE                5
#define CDC_ABSTRACT_CONTROL_MANAGEMENT_SIZE    4
#define CDC_UNION_SIZE                          5

/*------------------------------------------------------------------------------
 *      USB Device Descriptors
 *----------------------------------------------------------------------------*/
#define USBD_MSC_DESC_LEN                 (USB_INTERFACE_DESC_SIZE + 2*USB_ENDPOINT_DESC_SIZE)
#define USBD_CDC_ACM_DESC_LEN             (USBD_MULTI_IF * USB_INTERFACE_ASSOC_DESC_SIZE                                                        + \
                                           /* CDC Interface 1 */                                                                                  \
                                           USB_INTERFACE_DESC_SIZE + CDC_HEADER_SIZE + CDC_CALL_MANAGEMENT_SIZE                                 + \
                                           CDC_ABSTRACT_CONTROL_MANAGEMENT_SIZE + CDC_UNION_SIZE + USB_ENDPOINT_DESC_SIZE                       + \
                                           /* CDC Interface 2 */                                                                                  \
                                           USB_INTERFACE_DESC_SIZE + USB_ENDPOINT_DESC_SIZE + USB_ENDPOINT_DESC_SIZE)
#define USBD_HID_DESC_LEN                 (USB_INTERFACE_DESC_SIZE + USB_HID_DESC_SIZE                                                          + \
                                          (USB_ENDPOINT_DESC_SIZE*((USBD_HID_EP_INTIN != 0)+(USBD_HID_EP_INTOUT != 0))))
#define USBD_BULK_DESC_LEN                (USB_INTERFACE_DESC_SIZE + 2*USB_ENDPOINT_DESC_SIZE)

#define USBD_HID_DESC_OFS                 (USB_CONFIGUARTION_DESC_SIZE + USB_INTERFACE_DESC_SIZE                                                + \
                                           USBD_MSC_ENABLE * USBD_MSC_DESC_LEN + USBD_CDC_ACM_ENABLE * USBD_CDC_ACM_DESC_LEN)
  
#define USBD_WTOTALLENGTH_MAX              (USB_CONFIGUARTION_DESC_SIZE +                 \
                                           USBD_CDC_ACM_DESC_LEN * USBD_CDC_ACM_ENABLE + \
                                           USBD_HID_DESC_LEN     * USBD_HID_ENABLE     + \
                                           (USB_INTERFACE_DESC_SIZE) * USBD_WEBUSB_ENABLE + \
                                           USBD_BULK_DESC_LEN     * USBD_BULK_ENABLE + \
                                           USBD_MSC_DESC_LEN     * USBD_MSC_ENABLE)

/*------------------------------------------------------------------------------
  Default HID Report Descriptor
 *----------------------------------------------------------------------------*/

/*   Bit    Input       Output
      0     IN0          OUT0
      1     IN1          OUT1
      2     IN2          OUT2
      3     IN3          OUT3
      4     IN4          OUT4
      5     IN5          OUT5
      6     IN6          OUT6
      7     IN7          OUT7
*/

__weak \
const U8 USBD_HID_ReportDescriptor[] = {
    HID_UsagePageVendor(0x00),
    HID_Usage(0x01),
    HID_Collection(HID_Application),
    HID_LogicalMin(0),                              /* value range: 0 - 0xFF */
    HID_LogicalMaxS(0xFF),
    HID_ReportSize(8),                              /* 8 bits */
#if (USBD_HID_INREPORT_MAX_SZ > 255)
    HID_ReportCountS(USBD_HID_INREPORT_MAX_SZ),
#else
    HID_ReportCount(USBD_HID_INREPORT_MAX_SZ),
#endif
    HID_Usage(0x01),
    HID_Input(HID_Data | HID_Variable | HID_Absolute),
#if (USBD_HID_OUTREPORT_MAX_SZ > 255)
    HID_ReportCountS(USBD_HID_OUTREPORT_MAX_SZ),
#else
    HID_ReportCount(USBD_HID_OUTREPORT_MAX_SZ),
#endif
    HID_Usage(0x01),
    HID_Output(HID_Data | HID_Variable | HID_Absolute),
#if (USBD_HID_FEATREPORT_MAX_SZ > 255)
    HID_ReportCountS(USBD_HID_FEATREPORT_MAX_SZ),
#else
    HID_ReportCount(USBD_HID_FEATREPORT_MAX_SZ),
#endif
    HID_Usage(0x01),
    HID_Feature(HID_Data | HID_Variable | HID_Absolute),
    HID_EndCollection,
};

__weak \
const U16 USBD_HID_ReportDescriptorSize = sizeof(USBD_HID_ReportDescriptor);

__weak \
U16 USBD_HID_DescriptorOffset     = USBD_HID_DESC_OFS;

/* USB Device Standard Descriptor */
__weak \
const U8 USBD_DeviceDescriptor[] = {
    USB_DEVICE_DESC_SIZE,                 /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,           /* bDescriptorType */
#if (USBD_BOS_ENABLE)
    WBVAL(0x0210), /* 2.10 */             /* bcdUSB */
#elif ((USBD_HS_ENABLE) || (USBD_MULTI_IF))
    WBVAL(0x0200), /* 2.00 */             /* bcdUSB */
#else
    WBVAL(0x0110), /* 1.10 */             /* bcdUSB */
#endif
#if (USBD_MULTI_IF)
    USB_DEVICE_CLASS_MISCELLANEOUS,       /* bDeviceClass */
    0x02,                                 /* bDeviceSubClass */
    0x01,                                 /* bDeviceProtocol */
#elif (USBD_CDC_ACM_ENABLE)
    USB_DEVICE_CLASS_COMMUNICATIONS,      /* bDeviceClass CDC*/
    0x00,                                 /* bDeviceSubClass */
    0x00,                                 /* bDeviceProtocol */
#else
    0x00,                                 /* bDeviceClass */
    0x00,                                 /* bDeviceSubClass */
    0x00,                                 /* bDeviceProtocol */
#endif
    USBD_MAX_PACKET0,                     /* bMaxPacketSize0 */
    WBVAL(USBD_DEVDESC_IDVENDOR),         /* idVendor */
    WBVAL(USBD_DEVDESC_IDPRODUCT),        /* idProduct */
    WBVAL(USBD_DEVDESC_BCDDEVICE),        /* bcdDevice */
    0x01,                                 /* iManufacturer */
    0x02,                                 /* iProduct */
    0x03 * USBD_STRDESC_SER_ENABLE,       /* iSerialNumber */
    0x01                                  /* bNumConfigurations: one possible configuration*/
};

#if (USBD_HS_ENABLE)
/* USB Device Qualifier Descriptor (for Full Speed) */
__weak \
const U8 USBD_DeviceQualifier[] = {
    USB_DEVICE_QUALI_SIZE,                /* bLength */
    USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE, /* bDescriptorType */
#if (USBD_BOS_ENABLE)
    WBVAL(0x0210), /* 2.10 */             /* bcdUSB */
#else
    WBVAL(0x0200), /* 2.00 */             /* bcdUSB */
#endif
    0x00,                                 /* bDeviceClass */
    0x00,                                 /* bDeviceSubClass */
    0x00,                                 /* bDeviceProtocol */
    USBD_MAX_PACKET0,                     /* bMaxPacketSize0 */
    0x01,                                 /* bNumConfigurations */
    0x00                                  /* bReserved */
};

/* USB Device Qualifier Descriptor for High Speed */
__weak \
const U8 USBD_DeviceQualifier_HS[] = {
    USB_DEVICE_QUALI_SIZE,                /* bLength */
    USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE, /* bDescriptorType */
#if (USBD_BOS_ENABLE)
    WBVAL(0x0210), /* 2.10 */             /* bcdUSB */
#else
    WBVAL(0x0200), /* 2.00 */             /* bcdUSB */
#endif
    0x00,                                 /* bDeviceClass */
    0x00,                                 /* bDeviceSubClass */
    0x00,                                 /* bDeviceProtocol */
    USBD_MAX_PACKET0,                     /* bMaxPacketSize0 */
    0x01,                                 /* bNumConfigurations */
    0x00                                  /* bReserved */
};
#else
/* USB Device Qualifier Descriptor (for Full Speed) */
__weak \
const U8 USBD_DeviceQualifier[]    = { 0 };

/* USB Device Qualifier Descriptor for High Speed */
__weak \
const U8 USBD_DeviceQualifier_HS[] = { 0 };
#endif

#if (USBD_WINUSB_ENABLE)

#define FUNCTION_SUBSET_LEN                 160
#define DEVICE_INTERFACE_GUIDS_FEATURE_LEN  132

#define USBD_WINUSB_DESC_SET_LEN            (WINUSB_DESCRIPTOR_SET_HEADER_SIZE + USBD_WEBUSB_ENABLE * FUNCTION_SUBSET_LEN + USBD_BULK_ENABLE * FUNCTION_SUBSET_LEN)

U8 USBD_WinUSBDescriptorSetDescriptor[] = {
    WBVAL(WINUSB_DESCRIPTOR_SET_HEADER_SIZE), /* wLength */
    WBVAL(WINUSB_SET_HEADER_DESCRIPTOR_TYPE), /* wDescriptorType */
    0x00, 0x00, 0x03, 0x06, /* >= Win 8.1 */  /* dwWindowsVersion*/
    WBVAL(USBD_WINUSB_DESC_SET_LEN),          /* wDescriptorSetTotalLength */
#if (USBD_WEBUSB_ENABLE)    
    WBVAL(WINUSB_FUNCTION_SUBSET_HEADER_SIZE),// wLength 
    WBVAL(WINUSB_SUBSET_HEADER_FUNCTION_TYPE),// wDescriptorType 
    0,                                        // bFirstInterface USBD_WINUSB_IF_NUM
    0,                                        // bReserved 
    WBVAL(FUNCTION_SUBSET_LEN),               // wSubsetLength 
    WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_SIZE), // wLength 
    WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_TYPE), // wDescriptorType 
    'W', 'I', 'N', 'U', 'S', 'B', 0, 0,       // CompatibleId
    0, 0, 0, 0, 0, 0, 0, 0,                   // SubCompatibleId
    WBVAL(DEVICE_INTERFACE_GUIDS_FEATURE_LEN),// wLength 
    WBVAL(WINUSB_FEATURE_REG_PROPERTY_TYPE),  // wDescriptorType 
    WBVAL(WINUSB_PROP_DATA_TYPE_REG_MULTI_SZ), // wPropertyDataType 
    WBVAL(42), // wPropertyNameLength 
    'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,
    'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,
    'G',0,'U',0,'I',0,'D',0,'s',0,0,0,
    WBVAL(80), // wPropertyDataLength 
    '{',0,
    '9',0,'2',0,'C',0,'E',0,'6',0,'4',0,'6',0,'2',0,'-',0,
    '9',0,'C',0,'7',0,'7',0,'-',0,
    '4',0,'6',0,'F',0,'E',0,'-',0,
    '9',0,'3',0,'3',0,'B',0,'-',
    0,'3',0,'1',0,'C',0,'B',0,'9',0,'C',0,'5',0,'A',0,'A',0,'3',0,'B',0,'9',0,
    '}',0,0,0,0,0,
#endif 
#if (USBD_BULK_ENABLE)    
    WBVAL(WINUSB_FUNCTION_SUBSET_HEADER_SIZE),/* wLength */
    WBVAL(WINUSB_SUBSET_HEADER_FUNCTION_TYPE),/* wDescriptorType */
    0,                                        /* bFirstInterface USBD_BULK_IF_NUM*/
    0,                                        /* bReserved */
    WBVAL(FUNCTION_SUBSET_LEN),               /* wSubsetLength */
    WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_SIZE), /* wLength */
    WBVAL(WINUSB_FEATURE_COMPATIBLE_ID_TYPE), /* wDescriptorType */
    'W', 'I', 'N', 'U', 'S', 'B', 0, 0,       /* CompatibleId*/
    0, 0, 0, 0, 0, 0, 0, 0,                   /* SubCompatibleId*/
    WBVAL(DEVICE_INTERFACE_GUIDS_FEATURE_LEN),/* wLength */
    WBVAL(WINUSB_FEATURE_REG_PROPERTY_TYPE),  /* wDescriptorType */
    WBVAL(WINUSB_PROP_DATA_TYPE_REG_MULTI_SZ), /* wPropertyDataType */
    WBVAL(42), /* wPropertyNameLength */
    'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,
    'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,
    'G',0,'U',0,'I',0,'D',0,'s',0,0,0,
    WBVAL(80), /* wPropertyDataLength */
    '{',0,
    'C',0,'D',0,'B',0,'3',0,'B',0,'5',0,'A',0,'D',0,'-',0,
    '2',0,'9',0,'3',0,'B',0,'-',0,
    '4',0,'6',0,'6',0,'3',0,'-',0,
    'A',0,'A',0,'3',0,'6',0,'-',
    0,'1',0,'A',0,'A',0,'E',0,'4',0,'6',0,'4',0,'6',0,'3',0,'7',0,'7',0,'6',0,
    '}',0,0,0,0,0,
#endif    
};

#else

const U8 USBD_WinUSBDescriptorSetDescriptor[] = { 0 };

BOOL USBD_EndPoint0_Setup_WinUSB_ReqToDevice(void)
{
    return (__FALSE);
}

#endif

#if (USBD_BOS_ENABLE)

#define USBD_NUM_DEV_CAPABILITIES         (USBD_WEBUSB_ENABLE + USBD_WINUSB_ENABLE)

#define USBD_WEBUSB_DESC_LEN              (sizeof(WEBUSB_PLATFORM_CAPABILITY_DESCRIPTOR))

#define USBD_WINUSB_DESC_LEN              (sizeof(WINUSB_PLATFORM_CAPABILITY_DESCRIPTOR))

#define USBD_BOS_WTOTALLENGTH             (USB_BOS_DESC_SIZE +                         \
                                           USBD_WEBUSB_DESC_LEN * USBD_WEBUSB_ENABLE + \
                                           USBD_WINUSB_DESC_LEN * USBD_WINUSB_ENABLE)

__weak \
const U8 USBD_BinaryObjectStoreDescriptor[] = {
	USB_BOS_DESC_SIZE,                      /* bLength */
	USB_BINARY_OBJECT_STORE_DESCRIPTOR_TYPE,/* bDescriptorType */
	WBVAL(USBD_BOS_WTOTALLENGTH),           /* wTotalLength */
	USBD_NUM_DEV_CAPABILITIES,              /* bNumDeviceCaps */
#if (USBD_WEBUSB_ENABLE)
	USBD_WEBUSB_DESC_LEN,                   /* bLength */
	USB_DEVICE_CAPABILITY_DESCRIPTOR_TYPE,  /* bDescriptorType */
	USB_DEVICE_CAPABILITY_PLATFORM,         /* bDevCapabilityType */
	0x00,                                   /* bReserved */
	0x38, 0xB6, 0x08, 0x34,                 /* PlatformCapabilityUUID */
	0xA9, 0x09, 0xA0, 0x47,
	0x8B, 0xFD, 0xA0, 0x76,
	0x88, 0x15, 0xB6, 0x65,
	WBVAL(0x0100), /* 1.00 */               /* bcdVersion */
	USBD_WEBUSB_VENDOR_CODE,                /* bVendorCode */
	0,                                      /* iLandingPage */
#endif
#if (USBD_WINUSB_ENABLE)
	USBD_WINUSB_DESC_LEN,                   /* bLength */
	USB_DEVICE_CAPABILITY_DESCRIPTOR_TYPE,  /* bDescriptorType */
	USB_DEVICE_CAPABILITY_PLATFORM,         /* bDevCapabilityType */
	0x00,                                   /* bReserved */
	0xDF, 0x60, 0xDD, 0xD8,                 /* PlatformCapabilityUUID */
	0x89, 0x45, 0xC7, 0x4C,
	0x9C, 0xD2, 0x65, 0x9D,
	0x9E, 0x64, 0x8A, 0x9F,
	0x00, 0x00, 0x03, 0x06, /* >= Win 8.1 *//* dwWindowsVersion*/
	WBVAL(USBD_WINUSB_DESC_SET_LEN),        /* wDescriptorSetTotalLength */
	USBD_WINUSB_VENDOR_CODE,                /* bVendorCode */
	0,                                      /* bAltEnumCode */
#endif
};

#else
__weak \
const U8 USBD_BinaryObjectStoreDescriptor[] = { 0 };

#endif

#define HID_DESC                                                                                            \
  /* Interface, Alternate Setting 0, HID Class */                                                           \
  USB_INTERFACE_DESC_SIZE,              /* bLength */                                                       \
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */                                               \
  0x00,                                 /* bInterfaceNumber USBD_HID_IF_NUM*/                               \
  0x00,                                 /* bAlternateSetting */                                             \
  0x00+(USBD_HID_EP_INTIN != 0)+(USBD_HID_EP_INTOUT != 0), /*0x01+(USBD_HID_EP_INTOUT != 0),*/       /* bNumEndpoints */                                                 \
  USB_DEVICE_CLASS_HUMAN_INTERFACE,     /* bInterfaceClass */                                               \
  HID_SUBCLASS_NONE,                    /* bInterfaceSubClass */                                            \
  HID_PROTOCOL_NONE,                    /* bInterfaceProtocol */                                            \
  USBD_HID_IF_STR_NUM,                  /* iInterface */                                                    \
                                                                                                            \
/* HID Class Descriptor */                                                                                  \
  USB_HID_DESC_SIZE,                    /* bLength */                                                       \
  HID_HID_DESCRIPTOR_TYPE,              /* bDescriptorType */                                               \
  WBVAL(0x0100), /* 1.00 */             /* bcdHID */                                                        \
  0x00,                                 /* bCountryCode */                                                  \
  0x01,                                 /* bNumDescriptors */                                               \
  HID_REPORT_DESCRIPTOR_TYPE,           /* bDescriptorType */                                               \
  WBVAL(USB_HID_REPORT_DESC_SIZE),      /* wDescriptorLength */

#define VENDOR_DESC_IAD(first,num_of_ifs)  /* Vendor: Interface Association Descriptor */                     \
  USB_INTERFACE_ASSOC_DESC_SIZE,        /* bLength */                                                       \
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,  /* bDescriptorType */                                         \
 (first),                               /* bFirstInterface */                                               \
 (num_of_ifs),                          /* bInterfaceCount */                                               \
  USB_DEVICE_CLASS_VENDOR_SPECIFIC,    /* bFunctionClass     */                       \
  0x00,           /* bFunctionSubclass */                    \
  0x00,                                 /* bFunctionProtocol */               \
  0x00,             /* iFunction */

#define WEBUSB_DESC                                                                                            \
/* Interface, Alternate Setting 0, VENDOR_SPECIFIC Class */                                                           \
  USB_INTERFACE_DESC_SIZE,              /* bLength */                                                       \
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */                                               \
  0x00,                                 /* bInterfaceNumber USBD_WEBUSB_IF_NUM */                           \
  0x00,                                 /* bAlternateSetting */                                             \
  0x00,                                 /* bNumEndpoints */                                                 \
  USB_DEVICE_CLASS_VENDOR_SPECIFIC,     /* bInterfaceClass */                                               \
  USB_DEVICE_CLASS_HUMAN_INTERFACE,     /* bInterfaceSubClass */                                            \
  HID_PROTOCOL_NONE,                    /* bInterfaceProtocol */                                            \
  USBD_WEBUSB_IF_STR_NUM,               /* iInterface */                                                    \

#define HID_EP_IN                       /* HID Endpoint for Low-speed/Full-speed */                         \
/* Endpoint, HID Interrupt In */                                                                            \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_HID_EP_INTIN),   /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes */                                                  \
  WBVAL(USBD_HID_WMAXPACKETSIZE),       /* wMaxPacketSize */                                                \
  USBD_HID_BINTERVAL,                   /* bInterval */

#define HID_EP_OUT                          /* HID Endpoint for Low-speed/Full-speed */                         \
/* Endpoint, HID Interrupt Out */                                                                           \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_HID_EP_INTOUT), /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes */                                                  \
  WBVAL(USBD_HID_WMAXPACKETSIZE),       /* wMaxPacketSize */                                                \
  USBD_HID_BINTERVAL,                   /* bInterval */

#define HID_EP_INOUT                    /* HID Endpoint for Low-speed/Full-speed */                         \
/* Endpoint, HID Interrupt In */                                                                            \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_HID_EP_INTIN),   /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes */                                                  \
  WBVAL(USBD_HID_WMAXPACKETSIZE),       /* wMaxPacketSize */                                                \
  USBD_HID_BINTERVAL,                   /* bInterval */                                                     \
                                                                                                            \
/* Endpoint, HID Interrupt Out */                                                                           \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_HID_EP_INTOUT), /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes */                                                  \
  WBVAL(USBD_HID_WMAXPACKETSIZE),       /* wMaxPacketSize */                                                \
  USBD_HID_BINTERVAL,                   /* bInterval */

#define HID_EP_IN_HS                       /* HID Endpoint for High-speed */                                   \
/* Endpoint, HID Interrupt In */                                                                            \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_HID_EP_INTIN),   /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes */                                                  \
  WBVAL(USBD_HID_HS_WMAXPACKETSIZE),    /* wMaxPacketSize */                                                \
  USBD_HID_HS_BINTERVAL,                /* bInterval */

#define HID_EP_OUT_HS                       /* HID Endpoint for High-speed */                                   \
/* Endpoint, HID Interrupt Out */                                                                           \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_HID_EP_INTOUT), /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes */                                                  \
  WBVAL(USBD_HID_HS_WMAXPACKETSIZE),    /* wMaxPacketSize */                                                \
  USBD_HID_HS_BINTERVAL,                /* bInterval */

#define HID_EP_INOUT_HS                 /* HID Endpoint for High-speed */                                   \
/* Endpoint, HID Interrupt In */                                                                            \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_HID_EP_INTIN),   /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes */                                                  \
  WBVAL(USBD_HID_HS_WMAXPACKETSIZE),    /* wMaxPacketSize */                                                \
  USBD_HID_HS_BINTERVAL,                /* bInterval */                                                     \
                                                                                                            \
/* Endpoint, HID Interrupt Out */                                                                           \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_HID_EP_INTOUT), /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes */                                                  \
  WBVAL(USBD_HID_HS_WMAXPACKETSIZE),    /* wMaxPacketSize */                                                \
  USBD_HID_HS_BINTERVAL,                /* bInterval */

#define MSC_DESC                                                                                            \
/* Interface, Alternate Setting 0, MSC Class */                                                             \
  USB_INTERFACE_DESC_SIZE,              /* bLength */                                                       \
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */                                               \
  0x00,                                 /* bInterfaceNumber USBD_MSC_IF_NUM*/                               \
  0x00,                                 /* bAlternateSetting */                                             \
  0x02,                                 /* bNumEndpoints */                                                 \
  USB_DEVICE_CLASS_STORAGE,             /* bInterfaceClass */                                               \
  MSC_SUBCLASS_SCSI,                    /* bInterfaceSubClass */                                            \
  MSC_PROTOCOL_BULK_ONLY,               /* bInterfaceProtocol */                                            \
  USBD_MSC_IF_STR_NUM,                  /* iInterface */

#define MSC_EP                          /* MSC Endpoints for Low-speed/Full-speed */                        \
/* Endpoint, EP Bulk IN */                                                                                  \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_MSC_EP_BULKIN),  /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_MSC_WMAXPACKETSIZE),       /* wMaxPacketSize */                                                \
  0x00,                                 /* bInterval: ignore for Bulk transfer */                           \
                                                                                                            \
/* Endpoint, EP Bulk OUT */                                                                                 \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_MSC_EP_BULKOUT),/* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_MSC_WMAXPACKETSIZE),       /* wMaxPacketSize */                                                \
  0x00,                                 /* bInterval: ignore for Bulk transfer */

#define MSC_EP_HS                       /* MSC Endpoints for High-speed */                                  \
/* Endpoint, EP Bulk IN */                                                                                  \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_MSC_EP_BULKIN),  /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_MSC_HS_WMAXPACKETSIZE),    /* wMaxPacketSize */                                                \
  USBD_MSC_HS_BINTERVAL,                /* bInterval */                                                     \
                                                                                                            \
/* Endpoint, EP Bulk OUT */                                                                                 \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_MSC_EP_BULKOUT),/* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_MSC_HS_WMAXPACKETSIZE),    /* wMaxPacketSize */                                                \
  USBD_MSC_HS_BINTERVAL,                /* bInterval */
  
#define BULK_DESC                                                                                            \
/* Interface, Alternate Setting 0, MSC Class */                                                             \
  USB_INTERFACE_DESC_SIZE,              /* bLength */                                                       \
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */                                               \
  0x00,                                 /* bInterfaceNumber USBD_BULK_IF_NUM*/                             \
  0x00,                                 /* bAlternateSetting */                                             \
  0x02,                                 /* bNumEndpoints */                                                 \
  USB_DEVICE_CLASS_VENDOR_SPECIFIC,     /* bInterfaceClass */                                               \
  0x00,                                 /* bInterfaceSubClass */                                            \
  0x00,                                 /* bInterfaceProtocol */                                            \
  USBD_BULK_IF_STR_NUM,                  /* iInterface */

#define BULK_EP                          /* MSC Endpoints for Low-speed/Full-speed */                        \
/* Endpoint, EP Bulk OUT */                                                                                 \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_BULK_EP_BULKOUT),/* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_BULK_WMAXPACKETSIZE),       /* wMaxPacketSize */                                                \
  0x00,                                 /* bInterval: ignore for Bulk transfer */                           \
                                                                                                            \
/* Endpoint, EP Bulk IN */                                                                                  \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_BULK_EP_BULKIN),  /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_BULK_WMAXPACKETSIZE),       /* wMaxPacketSize */                                                \
  0x00,                                 /* bInterval: ignore for Bulk transfer */                           

#define BULK_EP_HS                          /* MSC Endpoints for Low-speed/Full-speed */                        \
/* Endpoint, EP Bulk OUT */                                                                                  \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_BULK_EP_BULKOUT),  /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_BULK_HS_WMAXPACKETSIZE),       /* wMaxPacketSize */                                                \
  0x00,                                 /* bInterval: ignore for Bulk transfer */                           \
                                                                                                            \
/* Endpoint, EP Bulk IN */                                                                                 \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_BULK_EP_BULKIN),/* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_BULK_HS_WMAXPACKETSIZE),       /* wMaxPacketSize */                                                \
  0x00,                                 /* bInterval: ignore for Bulk transfer */                           

#define ADC_DESC_IAD(first,num_of_ifs)  /* ADC: Interface Association Descriptor */                         \
  USB_INTERFACE_ASSOC_DESC_SIZE,        /* bLength */                                                       \
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,  /* bDescriptorType */                                         \
 (first),                               /* bFirstInterface */                                               \
 (num_of_ifs),                          /* bInterfaceCount */                                               \
  USB_DEVICE_CLASS_AUDIO,               /* bFunctionClass */                                                \
  AUDIO_SUBCLASS_AUDIOCONTROL,          /* bFunctionSubclass */                                             \
  AUDIO_PROTOCOL_UNDEFINED,             /* bFunctionProtocol */                                             \
  USBD_ADC_CIF_STR_NUM,                 /* iFunction */                                                     \

#define ADC_DESC                                                                                            \
/* Interface, Alternate Setting 0, Audio Control */                                                         \
  USB_INTERFACE_DESC_SIZE,              /* bLength */                                                       \
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */                                               \
  USBD_ADC_CIF_NUM,                     /* bInterfaceNumber */                                              \
  0x00,                                 /* bAlternateSetting */                                             \
  0x00,                                 /* bNumEndpoints */                                                 \
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */                                               \
  AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */                                            \
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */                                            \
  USBD_ADC_CIF_STR_NUM,                 /* iInterface */                                                    \
/* Audio Control Interface */                                                                               \
  AUDIO_CONTROL_INTERFACE_DESC_SZ(1),   /* bLength */                                                       \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */                                               \
  AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */                                            \
  WBVAL(0x0100), /* 1.00 */             /* bcdADC */                                                        \
  WBVAL(                                /* wTotalLength */                                                  \
    AUDIO_CONTROL_INTERFACE_DESC_SZ(1) +                                                                    \
    AUDIO_INPUT_TERMINAL_DESC_SIZE     +                                                                    \
    AUDIO_FEATURE_UNIT_DESC_SZ(1,1)    +                                                                    \
    AUDIO_OUTPUT_TERMINAL_DESC_SIZE                                                                         \
  ),                                                                                                        \
  0x01,                                 /* bInCollection */                                                 \
  0x01,                                 /* baInterfaceNr */                                                 \
                                                                                                            \
/* Audio Input Terminal */                                                                                  \
  AUDIO_INPUT_TERMINAL_DESC_SIZE,       /* bLength */                                                       \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */                                               \
  AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */                                            \
  0x01,                                 /* bTerminalID */                                                   \
  WBVAL(AUDIO_TERMINAL_USB_STREAMING),  /* wTerminalType */                                                 \
  0x00,                                 /* bAssocTerminal */                                                \
  0x01,                                 /* bNrChannels */                                                   \
  WBVAL(AUDIO_CHANNEL_M),               /* wChannelConfig */                                                \
  0x00,                                 /* iChannelNames */                                                 \
  0x00,                                 /* iTerminal */                                                     \
                                                                                                            \
/* Audio Feature Unit */                                                                                    \
  AUDIO_FEATURE_UNIT_DESC_SZ(1,1),      /* bLength */                                                       \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */                                               \
  AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype */                                            \
  0x02,                                 /* bUnitID */                                                       \
  0x01,                                 /* bSourceID */                                                     \
  0x01,                                 /* bControlSize */                                                  \
  AUDIO_CONTROL_MUTE |                                                                                      \
  AUDIO_CONTROL_VOLUME,                 /* bmaControls(0) */                                                \
  0x00,                                 /* bmaControls(1) */                                                \
  0x00,                                 /* iTerminal */                                                     \
                                                                                                            \
/* Audio Output Terminal */                                                                                 \
  AUDIO_OUTPUT_TERMINAL_DESC_SIZE,      /* bLength */                                                       \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */                                               \
  AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */                                            \
  0x03,                                 /* bTerminalID */                                                   \
  WBVAL(AUDIO_TERMINAL_SPEAKER),        /* wTerminalType */                                                 \
  0x00,                                 /* bAssocTerminal */                                                \
  0x02,                                 /* bSourceID */                                                     \
  0x00,                                 /* iTerminal */                                                     \
                                                                                                            \
/* Interface, Alternate Setting 0, Audio Streaming - Zero Bandwith */                                       \
  USB_INTERFACE_DESC_SIZE,              /* bLength */                                                       \
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */                                               \
  USBD_ADC_SIF1_NUM,                    /* bInterfaceNumber */                                              \
  0x00,                                 /* bAlternateSetting */                                             \
  0x00,                                 /* bNumEndpoints */                                                 \
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */                                               \
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */                                            \
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */                                            \
  USBD_ADC_SIF1_STR_NUM,                /* iInterface */                                                    \
                                                                                                            \
/* Interface, Alternate Setting 1, Audio Streaming - Operational */                                         \
  USB_INTERFACE_DESC_SIZE,              /* bLength */                                                       \
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */                                               \
  USBD_ADC_SIF1_NUM,                    /* bInterfaceNumber */                                              \
  0x01,                                 /* bAlternateSetting */                                             \
  0x01,                                 /* bNumEndpoints */                                                 \
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */                                               \
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */                                            \
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */                                            \
  USBD_ADC_SIF2_STR_NUM,                /* iInterface */                                                    \
                                                                                                            \
/* Audio Streaming Interface */                                                                             \
  AUDIO_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */                                                       \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */                                               \
  AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */                                            \
  0x01,                                 /* bTerminalLink */                                                 \
  0x01,                                 /* bDelay */                                                        \
  WBVAL(AUDIO_FORMAT_PCM),              /* wFormatTag */                                                    \
                                                                                                            \
/* Audio Type I Format */                                                                                   \
  AUDIO_FORMAT_TYPE_I_DESC_SZ(1),       /* bLength */                                                       \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */                                               \
  AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */                                            \
  AUDIO_FORMAT_TYPE_I,                  /* bFormatType */                                                   \
  0x01,                                 /* bNrChannels */                                                   \
  USBD_ADC_BSUBFRAMESIZE,               /* bSubFrameSize */                                                 \
  USBD_ADC_BBITRESOLUTION,              /* bBitResolution */                                                \
  0x01,                                 /* bSamFreqType */                                                  \
  B3VAL(USBD_ADC_TSAMFREQ),             /* tSamFreq */

#define ADC_EP                          /* ADC Endpoints for Low-speed/Full-speed */                        \
/* Endpoint, EP ISO OUT - Standard Descriptor */                                                            \
  AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_ADC_EP_ISOOUT), /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_ISOCHRONOUS,        /* bmAttributes */                                                  \
  WBVAL(USBD_ADC_WMAXPACKETSIZE),       /* wMaxPacketSize */                                                \
  USBD_ADC_BINTERVAL,                   /* bInterval */                                                     \
  0x00,                                 /* bRefresh */                                                      \
  0x00,                                 /* bSynchAddress */                                                 \
                                                                                                            \
/* Endpoint - Audio Streaming */                                                                            \
  AUDIO_STREAMING_ENDPOINT_DESC_SIZE,   /* bLength */                                                       \
  AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */                                               \
  AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */                                                   \
  0x00,                                 /* bmAttributes */                                                  \
  0x00,                                 /* bLockDelayUnits */                                               \
  WBVAL(0x0000),                        /* wLockDelay */

#define ADC_EP_HS                       /* ADC Endpoints for High-speed */                                  \
/* Endpoint, EP ISO OUT - Standard Descriptor */                                                            \
  AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_ADC_EP_ISOOUT), /* bEndpointAddress */                                              \
  USB_ENDPOINT_TYPE_ISOCHRONOUS,        /* bmAttributes */                                                  \
  WBVAL(USBD_ADC_HS_WMAXPACKETSIZE),    /* wMaxPacketSize */                                                \
  USBD_ADC_BINTERVAL,                   /* bInterval */                                                     \
  0x00,                                 /* bRefresh */                                                      \
  0x00,                                 /* bSynchAddress */                                                 \
                                                                                                            \
/* Endpoint - Audio Streaming */                                                                            \
  AUDIO_STREAMING_ENDPOINT_DESC_SIZE,   /* bLength */                                                       \
  AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */                                               \
  AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */                                                   \
  0x00,                                 /* bmAttributes */                                                  \
  0x00,                                 /* bLockDelayUnits */                                               \
  WBVAL(0x0000),                        /* wLockDelay */

#define CDC_ACM_DESC_IAD(first,num_of_ifs)  /* CDC: Interface Association Descriptor */                     \
  USB_INTERFACE_ASSOC_DESC_SIZE,        /* bLength */                                                       \
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR_TYPE,  /* bDescriptorType */                                         \
 (first),                               /* bFirstInterface */                                               \
 (num_of_ifs),                          /* bInterfaceCount */                                               \
  CDC_COMMUNICATION_INTERFACE_CLASS,    /* bFunctionClass    (Communication Class) */                       \
  CDC_ABSTRACT_CONTROL_MODEL,           /* bFunctionSubclass (Abstract Control Model) */                    \
  0x01,                                 /* bFunctionProtocol (V.25ter, Common AT commands) */               \
  USBD_CDC_ACM_CIF_STR_NUM,             /* iFunction */                                                     \

#define CDC_ACM_DESC_IF0                                                                                        \
/* Interface, Alternate Setting 0, CDC Class */                                                             \
  USB_INTERFACE_DESC_SIZE,              /* bLength */                                                       \
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */                                               \
  0x00,                                 /* bInterfaceNumber: Number of Interface USBD_CDC_ACM_CIF_NUM*/    \
  0x00,                                 /* bAlternateSetting: Alternate setting */                          \
  0x01,                                 /* bNumEndpoints: One endpoint used */                              \
  CDC_COMMUNICATION_INTERFACE_CLASS,    /* bInterfaceClass: Communication Interface Class */                \
  CDC_ABSTRACT_CONTROL_MODEL,           /* bInterfaceSubClass: Abstract Control Model */                    \
  0x01,                                 /* bInterfaceProtocol: no protocol used */                          \
  USBD_CDC_ACM_CIF_STR_NUM,             /* iInterface: */                                                   \
                                                                                                            \
/* Header Functional Descriptor */                                                                          \
  CDC_HEADER_SIZE,                      /* bLength: Endpoint Descriptor size */                             \
  CDC_CS_INTERFACE,                     /* bDescriptorType: CS_INTERFACE */                                 \
  CDC_HEADER,                           /* bDescriptorSubtype: Header Func Desc */                          \
  WBVAL(CDC_V1_10), /* 1.10 */          /* bcdCDC */                                                        \
/* Call Management Functional Descriptor */                                                                 \
  CDC_CALL_MANAGEMENT_SIZE,             /* bFunctionLength */                                               \
  CDC_CS_INTERFACE,                     /* bDescriptorType: CS_INTERFACE */                                 \
  CDC_CALL_MANAGEMENT,                  /* bDescriptorSubtype: Call Management Func Desc */                 \
  0x03,                                 /* bmCapabilities: device handles call management */                \
  0x02,                                 /* bDataInterface: CDC data IF ID */                                \
/* Abstract Control Management Functional Descriptor */                                                     \
  CDC_ABSTRACT_CONTROL_MANAGEMENT_SIZE, /* bFunctionLength */                                               \
  CDC_CS_INTERFACE,                     /* bDescriptorType: CS_INTERFACE */                                 \
  CDC_ABSTRACT_CONTROL_MANAGEMENT,      /* bDescriptorSubtype: Abstract Control Management desc */          \
  0x06,                                 /* bmCapabilities: SET_LINE_CODING, GET_LINE_CODING, SET_CONTROL_LINE_STATE supported */ \
/* Union Functional Descriptor */                                                                           \
  CDC_UNION_SIZE,                       /* bFunctionLength */                                               \
  CDC_CS_INTERFACE,                     /* bDescriptorType: CS_INTERFACE */                                 \
  CDC_UNION,                            /* bDescriptorSubtype: Union func desc */                           \
  0x00,                                 /* bMasterInterface: Communication class interface is master USBD_CDC_ACM_CIF_NUM*/ \
  0x00,                                 /* bSlaveInterface0: Data class interface is slave 0 USBD_CDC_ACM_DIF_NUM*/

#define CDC_ACM_EP_IF0                  /* CDC Endpoints for Interface 0 for Low-speed/Full-speed */        \
/* Endpoint, EP Interrupt IN */         /* event notification (optional) */                                 \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_CDC_ACM_EP_INTIN),/* bEndpointAddress */                                             \
  USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes */                                                  \
  WBVAL(USBD_CDC_ACM_WMAXPACKETSIZE),   /* wMaxPacketSize */                                                \
  USBD_CDC_ACM_BINTERVAL,               /* bInterval */

#define CDC_ACM_EP_IF0_HS               /* CDC Endpoints for Interface 0 for High-speed */                  \
/* Endpoint, EP Interrupt IN */         /* event notification (optional) */                                 \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_CDC_ACM_EP_INTIN),/* bEndpointAddress */                                             \
  USB_ENDPOINT_TYPE_INTERRUPT,          /* bmAttributes */                                                  \
  WBVAL(USBD_CDC_ACM_HS_WMAXPACKETSIZE),/* wMaxPacketSize */                                                \
  USBD_CDC_ACM_HS_BINTERVAL,            /* bInterval */

#define CDC_ACM_DESC_IF1                                                                                        \
/* Interface, Alternate Setting 0, Data class interface descriptor*/                                        \
  USB_INTERFACE_DESC_SIZE,              /* bLength */                                                       \
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */                                               \
  0x00,                                 /* bInterfaceNumber: Number of Interface USBD_CDC_ACM_DIF_NUM*/      \
  0x00,                                 /* bAlternateSetting: no alternate setting */                       \
  0x02,                                 /* bNumEndpoints: two endpoints used */                             \
  CDC_DATA_INTERFACE_CLASS,             /* bInterfaceClass: Data Interface Class */                         \
  0x00,                                 /* bInterfaceSubClass: no subclass available */                     \
  0x00,                                 /* bInterfaceProtocol: no protocol used */                          \
  USBD_CDC_ACM_DIF_STR_NUM,             /* iInterface */

#define CDC_ACM_EP_IF1                  /* CDC Endpoints for Interface 1 for Low-speed/Full-speed */        \
/* Endpoint, EP Bulk OUT */                                                                                 \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_CDC_ACM_EP_BULKOUT),/* bEndpointAddress */                                          \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_CDC_ACM_WMAXPACKETSIZE1),  /* wMaxPacketSize */                                                \
  0x00,                                 /* bInterval: ignore for Bulk transfer */                           \
                                                                                                            \
/* Endpoint, EP Bulk IN */                                                                                  \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_CDC_ACM_EP_BULKIN),/* bEndpointAddress */                                            \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_CDC_ACM_WMAXPACKETSIZE1),  /* wMaxPacketSize */                                                \
  0x00,                                 /* bInterval: ignore for Bulk transfer */

#define CDC_ACM_EP_IF1_HS               /* CDC Endpoints for Interface 1 for High-speed */                  \
/* Endpoint, EP Bulk OUT */                                                                                 \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_OUT(USBD_CDC_ACM_EP_BULKOUT),/* bEndpointAddress */                                          \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_CDC_ACM_HS_WMAXPACKETSIZE1),/* wMaxPacketSize */                                               \
  USBD_CDC_ACM_HS_BINTERVAL1,           /* bInterval */                                                     \
                                                                                                            \
/* Endpoint, EP Bulk IN */                                                                                  \
  USB_ENDPOINT_DESC_SIZE,               /* bLength */                                                       \
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */                                               \
  USB_ENDPOINT_IN(USBD_CDC_ACM_EP_BULKIN),/* bEndpointAddress */                                            \
  USB_ENDPOINT_TYPE_BULK,               /* bmAttributes */                                                  \
  WBVAL(USBD_CDC_ACM_HS_WMAXPACKETSIZE1),/* wMaxPacketSize */                                               \
  USBD_CDC_ACM_HS_BINTERVAL1,           /* bInterval */

/* USB Device Configuration Descriptor (for Full Speed) */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor) */
__weak \
U8 USBD_ConfigDescriptor[200] = { 0 };

#if (USBD_HS_ENABLE == 0)               /* If High-speed not enabled, declare dummy descriptors for High-speed */
__weak \
U8 USBD_ConfigDescriptor_HS[] = { 0 };
#else
/* USB Device Configuration Descriptor (for High Speed) */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor) */
__weak \
U8 USBD_ConfigDescriptor_HS[200] = { 0 };

#endif

/* USB Device Create String Descriptor */
#define USBD_STR_DEF(n)                 \
  struct {                              \
    U8  len;                            \
    U8  type;                           \
    U16 str[sizeof(USBD_##n)/2-1];      \
  } desc##n

#define USBD_STR_VAL(n)                  \
 { sizeof(USBD_##n), USB_STRING_DESCRIPTOR_TYPE, USBD_##n }

__weak \
const struct {
    struct {
        U8  len;
        U8  type;
        U16 langid;
    } desc_langid;
    USBD_STR_DEF(STRDESC_MAN);
    USBD_STR_DEF(STRDESC_PROD);
#if  (USBD_STRDESC_SER_ENABLE)
    USBD_STR_DEF(STRDESC_SER);
#endif
#if (USBD_ADC_ENABLE)
    USBD_STR_DEF(ADC_CIF_STRDESC);
    USBD_STR_DEF(ADC_SIF1_STRDESC);
    USBD_STR_DEF(ADC_SIF2_STRDESC);
#endif
#if (USBD_CDC_ACM_ENABLE)
    USBD_STR_DEF(CDC_ACM_CIF_STRDESC);
    USBD_STR_DEF(CDC_ACM_DIF_STRDESC);
#endif
#if (USBD_HID_ENABLE)
    USBD_STR_DEF(HID_STRDESC);
#endif
#if (USBD_WEBUSB_ENABLE)
    USBD_STR_DEF(WEBUSB_STRDESC);
#endif
#if (USBD_MSC_ENABLE)
    USBD_STR_DEF(MSC_STRDESC);
#endif
#if (USBD_BULK_ENABLE)
    USBD_STR_DEF(BULK_STRDESC);
#endif
} USBD_StringDescriptor
= {
    { 4, USB_STRING_DESCRIPTOR_TYPE, USBD_STRDESC_LANGID },
    USBD_STR_VAL(STRDESC_MAN),
    USBD_STR_VAL(STRDESC_PROD),
#if (USBD_STRDESC_SER_ENABLE)
    USBD_STR_VAL(STRDESC_SER),
#endif
#if (USBD_ADC_ENABLE)
    USBD_STR_VAL(ADC_CIF_STRDESC),
    USBD_STR_VAL(ADC_SIF1_STRDESC),
    USBD_STR_VAL(ADC_SIF2_STRDESC),
#endif
#if (USBD_CDC_ACM_ENABLE)
    USBD_STR_VAL(CDC_ACM_CIF_STRDESC),
    USBD_STR_VAL(CDC_ACM_DIF_STRDESC),
#endif
#if (USBD_HID_ENABLE)
    USBD_STR_VAL(HID_STRDESC),
#endif
#if (USBD_WEBUSB_ENABLE)
    USBD_STR_VAL(WEBUSB_STRDESC),
#endif
#if (USBD_MSC_ENABLE)
    USBD_STR_VAL(MSC_STRDESC),
#endif
#if (USBD_BULK_ENABLE)
    USBD_STR_VAL(BULK_STRDESC),
#endif
};

#if (USBD_WEBUSB_ENABLE)

#define WEBUSB_NUM_FUNCTIONS              (1)

#define WEBUSB_WTOTALLENGTH               (WEBUSB_DESCRIPTOR_SET_HEADER_SIZE +      \
                                           WEBUSB_CONFIGURATION_SUBSET_HEADER_SIZE +\
                                           (WEBUSB_NUM_FUNCTIONS * (WEBUSB_FUNCTION_SUBSET_HEADER_SIZE + 1)))

/* WebUSB Create URL Descriptor */
#define WEBUSB_URL_DEF(n)       \
  struct {                      \
    U8  bLength;                \
    U8  bDescriptorType;        \
    U8  bScheme;                \
    U8  URL[sizeof(USBD_##n)+3];\
  } url##n

#define WEBUSB_HTTP_URL_VAL(n)  \
{                               \
    (sizeof(USBD_##n) + 3) + 3, \
    WEBUSB_URL_TYPE,            \
    WEBUSB_URL_SCHEME_HTTP,     \
    USBD_##n                    \
}

#define WEBUSB_HTTPS_URL_VAL(n) \
{                               \
    (sizeof(USBD_##n) + 3) + 3, \
    WEBUSB_URL_TYPE,            \
    WEBUSB_URL_SCHEME_HTTPS,    \
    USBD_##n                    \
}

__weak \
struct {
    WEBUSB_URL_DEF(WEBUSB_LANDING_URL);
    WEBUSB_URL_DEF(WEBUSB_ORIGIN_URL);
} USBD_WebUSBURLDescriptor
= {
    WEBUSB_HTTPS_URL_VAL(WEBUSB_LANDING_URL),
    WEBUSB_HTTPS_URL_VAL(WEBUSB_ORIGIN_URL),
};

#else

U8 USBD_WebUSBURLDescriptor[] = { 0 };

BOOL USBD_EndPoint0_Setup_WebUSB_ReqToDevice(void)
{
    return (__FALSE);
}

#endif  //USBD_WEBUSB_ENABLE


extern uint8_t flash_algo_valid(void);

static U16 start_desc_fill(U8 * config_desc, U8 * config_desc_hs, U8 if_num) {
    U8 * pD = 0;
    const U8 start_desc[] = { 
        /* Configuration 1 */
        USB_CONFIGUARTION_DESC_SIZE,                // bLength 
        USB_CONFIGURATION_DESCRIPTOR_TYPE,          // bDescriptorType 
        WBVAL(USBD_WTOTALLENGTH_MAX),               // wTotalLength 
        USBD_IF_NUM_MAX,                            // bNumInterfaces 
        0x01,                                       // bConfigurationValue: 0x01 is used to select this configuration 
        0x00,                                       // iConfiguration: no string to describe this configuration 
        USBD_CFGDESC_BMATTRIBUTES |                 // bmAttributes 
        (USBD_POWER << 6),
        USBD_CFGDESC_BMAXPOWER                      // bMaxPower, device power consumption 
    };
    pD = config_desc;
    memcpy(pD, start_desc, sizeof(start_desc));
    
#if (USBD_HS_ENABLE == 1)
    pD = config_desc_hs;
    memcpy(pD, start_desc, sizeof(start_desc));
#endif
    
    return sizeof(start_desc);
}

static U16 hid_desc_fill(U8 * config_desc, U8 * config_desc_hs, U8 if_num) {
    U8 * pD = 0;
    const U8 hid_desc[] = {
        HID_DESC
    #if ((USBD_HID_EP_INTOUT != 0) && (USBD_HID_EP_INTIN != 0))
        HID_EP_INOUT
    #elif (USBD_HID_EP_INTIN != 0)
        HID_EP_IN
    #elif (USBD_HID_EP_INTOUT != 0)
        HID_EP_OUT
    #endif
    };
    pD = config_desc;
    memcpy(pD, hid_desc, sizeof(hid_desc));
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num;
#if (USBD_HS_ENABLE == 1)  
    const U8 hid_desc_hs[] = {
        HID_DESC
    #if ((USBD_HID_EP_INTOUT != 0) && (USBD_HID_EP_INTIN != 0))
        HID_EP_INOUT_HS
    #elif (USBD_HID_EP_INTIN != 0) //#else
        HID_EP_IN_HS
    #elif (USBD_HID_EP_INTOUT != 0)
        HID_EP_OUT_HS
    #endif
    };
    pD = config_desc_hs;
    memcpy(pD, hid_desc_hs, sizeof(hid_desc_hs));
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num;
#endif    
    return sizeof(hid_desc);
}

static U16 acm_cdc_desc_fill(U8 * config_desc, U8 * config_desc_hs, U8 if_num) {
    U8 * pD = 0;
    const U8 cdc_desc[] = {
    #if (USBD_MULTI_IF)
        CDC_ACM_DESC_IAD(0, 2)
    #endif
        CDC_ACM_DESC_IF0
        CDC_ACM_EP_IF0
        CDC_ACM_DESC_IF1
        CDC_ACM_EP_IF1
    };
    pD = config_desc;
    memcpy(pD, cdc_desc, sizeof(cdc_desc));

#if (USBD_MULTI_IF)
    ((USB_INTERFACE_ASSOCIATION_DESCRIPTOR *)pD)->bFirstInterface = if_num;
    pD += USB_INTERFACE_ASSOC_DESC_SIZE;
#endif    
    
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num;
    pD += USB_INTERFACE_DESC_SIZE + CDC_HEADER_SIZE + CDC_CALL_MANAGEMENT_SIZE + CDC_ABSTRACT_CONTROL_MANAGEMENT_SIZE;
    ((UNION_FUNCTIONAL_DESCRIPTOR*)pD)->bMasterInterface = if_num;
    ((UNION_FUNCTIONAL_DESCRIPTOR*)pD)->bSlaveInterface0 = if_num + 1;
    pD += CDC_UNION_SIZE + USB_ENDPOINT_DESC_SIZE;
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num + 1;

#if (USBD_HS_ENABLE == 1)    
    const U8 cdc_desc_hs[] = {
    #if (USBD_MULTI_IF)
        CDC_ACM_DESC_IAD(0, 2)
    #endif
        CDC_ACM_DESC_IF0
        CDC_ACM_EP_IF0_HS
        CDC_ACM_DESC_IF1
        CDC_ACM_EP_IF1_HS
    };
     pD = config_desc_hs;
    memcpy(pD, cdc_desc_hs, sizeof(cdc_desc_hs));
    
#if (USBD_MULTI_IF)
    ((USB_INTERFACE_ASSOCIATION_DESCRIPTOR *)pD)->bFirstInterface = if_num;
    pD += USB_INTERFACE_ASSOC_DESC_SIZE;
#endif    
    
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num;
    pD += USB_INTERFACE_DESC_SIZE + CDC_HEADER_SIZE + CDC_CALL_MANAGEMENT_SIZE + CDC_ABSTRACT_CONTROL_MANAGEMENT_SIZE;
    ((UNION_FUNCTIONAL_DESCRIPTOR*)pD)->bMasterInterface = if_num;
    ((UNION_FUNCTIONAL_DESCRIPTOR*)pD)->bSlaveInterface0 = if_num + 1;
    pD += CDC_UNION_SIZE + USB_ENDPOINT_DESC_SIZE;
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num +1 ;
#endif  //(USBD_HS_ENABLE == 1)    
    return sizeof(cdc_desc);
}

static U16 msc_desc_fill(U8 * config_desc, U8 * config_desc_hs, U8 if_num) {
    U8 * pD = 0;
    const U8 msc_desc[] = { 
        MSC_DESC
        MSC_EP
    };
    pD = config_desc;
    memcpy(pD, msc_desc, sizeof(msc_desc));
    
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num;

#if (USBD_HS_ENABLE == 1)     
    const U8 msc_desc_hs[] = { 
        MSC_DESC
        MSC_EP_HS
    };
    pD = config_desc_hs;
    memcpy(pD, msc_desc_hs, sizeof(msc_desc_hs));
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num;
#endif
    
    return sizeof(msc_desc);
}

#if (USBD_WEBUSB_ENABLE)
static U16 webusb_desc_fill(U8 * config_desc, U8 * config_desc_hs, U8 if_num) {
    U8 * pD = 0;
    const U8 webusb_desc[] = {
        WEBUSB_DESC
    };
    pD = config_desc;
    memcpy(pD, webusb_desc, sizeof(webusb_desc));
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num;
    
#if (USBD_HS_ENABLE == 1)
    pD = config_desc_hs;
    memcpy(pD, webusb_desc, sizeof(webusb_desc));
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num;
#endif
    
#if (USBD_WINUSB_ENABLE)
    pD = USBD_WinUSBDescriptorSetDescriptor + WINUSB_DESCRIPTOR_SET_HEADER_SIZE;
    ((WINUSB_FUNCTION_SUBSET_HEADER*)pD)->bFirstInterface = if_num;
#else
#error "WEBUSB requires WINUSB!"
#endif
    
    return sizeof(webusb_desc); 
}
#endif

#if (USBD_BULK_ENABLE)
static U16 bulk_desc_fill(U8 * config_desc, U8 * config_desc_hs, U8 if_num) {
    U8 * pD = 0;
    const U8 bulk_desc[] = { 
        BULK_DESC
        BULK_EP
    };
    pD = config_desc;
    memcpy(pD, bulk_desc, sizeof(bulk_desc));
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num;
#if (USBD_HS_ENABLE == 1)   
    const U8 bulk_desc_hs[] = { 
        BULK_DESC
        BULK_EP_HS
    };
     pD = config_desc_hs;
    memcpy(pD, bulk_desc_hs, sizeof(bulk_desc_hs));
    ((USB_INTERFACE_DESCRIPTOR *)pD)->bInterfaceNumber = if_num;
#endif

#if (USBD_WINUSB_ENABLE)
    pD = USBD_WinUSBDescriptorSetDescriptor + WINUSB_DESCRIPTOR_SET_HEADER_SIZE;
#if (USBD_WEBUSB_ENABLE)
    pD += WINUSB_FUNCTION_SUBSET_HEADER_SIZE + WINUSB_FEATURE_COMPATIBLE_ID_SIZE + DEVICE_INTERFACE_GUIDS_FEATURE_LEN;
#endif
    ((WINUSB_FUNCTION_SUBSET_HEADER*)pD)->bFirstInterface = if_num;
#else
#error "BULK interfaces requires WINUSB!"
#endif
    
    return sizeof(bulk_desc);
}
#endif

void usbd_class_init(void)
{   
    U8  if_num = 0;
    U16 desc_ptr = 0;
    
    desc_ptr += start_desc_fill(&USBD_ConfigDescriptor[desc_ptr], &USBD_ConfigDescriptor_HS[desc_ptr], if_num);    
    
#if (USBD_ADC_ENABLE)
    usbd_adc_init();
#endif


#if (USBD_MSC_ENABLE)        
    
#if !(defined(DAPLINK_BL)) &&  defined(DRAG_N_DROP_SUPPORT)
    //change descriptors here
    if (config_ram_get_disable_msd() == 1 || flash_algo_valid()==0 ){
        usbd_if_num -= USBD_MSC_ENABLE;
        USB_CONFIGURATION_DESCRIPTOR * usb_conf_desc = (USB_CONFIGURATION_DESCRIPTOR *)USBD_ConfigDescriptor;
        usb_conf_desc->bNumInterfaces = usbd_if_num;
        U16 usb_wtotal_len = USBD_WTOTALLENGTH_MAX - (USBD_MSC_DESC_LEN     * USBD_MSC_ENABLE);
        usb_conf_desc->wTotalLength = usb_wtotal_len;
        USBD_ConfigDescriptor[usb_wtotal_len] = 0;
        USBD_HID_DescriptorOffset -= USBD_MSC_ENABLE * USBD_MSC_DESC_LEN;
#if (USBD_HS_ENABLE == 1)
        usb_conf_desc = (USB_CONFIGURATION_DESCRIPTOR *)USBD_ConfigDescriptor_HS;
        usb_conf_desc->bNumInterfaces = usbd_if_num;
        usb_conf_desc->wTotalLength = usb_wtotal_len;
        USBD_ConfigDescriptor_HS[usb_wtotal_len] = 0;
#endif         
    } else
#endif
    {
    usbd_msc_if_num = if_num++;
    desc_ptr += msc_desc_fill(&USBD_ConfigDescriptor[desc_ptr], &USBD_ConfigDescriptor_HS[desc_ptr], usbd_msc_if_num);
    usbd_msc_init();

    }
#endif //#if (USBD_MSC_ENABLE)  

#if (USBD_CDC_ACM_ENABLE)
    usbd_cdc_acm_cif_num = if_num++;
    usbd_cdc_acm_dif_num = if_num++;
    desc_ptr += acm_cdc_desc_fill(&USBD_ConfigDescriptor[desc_ptr], &USBD_ConfigDescriptor_HS[desc_ptr], usbd_cdc_acm_cif_num);
    USBD_CDC_ACM_Initialize();
#endif

#if (USBD_HID_ENABLE) 
    usbd_hid_if_num = if_num++;
    desc_ptr += hid_desc_fill(&USBD_ConfigDescriptor[desc_ptr], &USBD_ConfigDescriptor_HS[desc_ptr], usbd_hid_if_num);
    usbd_hid_init();
#endif

#if (USBD_WEBUSB_ENABLE)
    usbd_webusb_if_num = if_num++;   
    desc_ptr += webusb_desc_fill(&USBD_ConfigDescriptor[desc_ptr], &USBD_ConfigDescriptor_HS[desc_ptr], usbd_webusb_if_num);
#endif

#if (USBD_BULK_ENABLE)
    usbd_bulk_if_num = if_num++;  
    desc_ptr += bulk_desc_fill(&USBD_ConfigDescriptor[desc_ptr], &USBD_ConfigDescriptor_HS[desc_ptr], usbd_bulk_if_num);
    usbd_bulk_init();
#endif

#if (USBD_CLS_ENABLE)
    usbd_cls_init();
#endif

}

#endif  //USBD_ENABLE

#endif  /* __USB_CONFIG__ */
