/**
 * @file    usbd_hid.c
 * @brief   Human Interface Device driver
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
#include "usb_for_lib.h"


U8 USBD_HID_Protocol;

BOOL DataOutAsyncReq;
U32 DataOutUpdateReqMask;
U8 *ptrDataOut;
volatile U16 DataOutToSendLen;
U16 DataOutSentLen;
BOOL DataOutEndWithShortPacket;

U8 *ptrDataIn;
U16 DataInReceMax;
U16 DataInReceLen;

U8 *ptrDataFeat;
U16 DataFeatReceLen;


/* Dummy Weak Functions that need to be provided by user */
__weak void usbd_hid_init(void)
{

}
__weak int usbd_hid_get_report(U8 rtype, U8 rid, U8 *buf, U8 req)
{
    return (0);
};
__weak void usbd_hid_set_report(U8  rtype, U8 rid, U8 *buf, int len, U8 req)
{

}
__weak U8 usbd_hid_get_protocol(void)
{
    return (0);
};
__weak void usbd_hid_set_protocol(U8  protocol)
{

};


/*
 *  USB Device HID Get Report Request Callback
 *   Called automatically on USB Device HID Get Report Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USBD_HID_GetReport(void)
{
    U8 *ptr_buf = 0;

    /* Report Type   = USBD_SetupPacket.wValueH */
    /* Report ID     = USBD_SetupPacket.wValueL */
    /* Report Length = USBD_SetupPacket.wLength */
    switch (USBD_SetupPacket.wValueH) {
        case HID_REPORT_INPUT:
            ptr_buf  = &USBD_HID_InReport[1];
            break;

        case HID_REPORT_OUTPUT:
            return (__FALSE);        /* Not Supported */

        case HID_REPORT_FEATURE:
            ptr_buf  = &USBD_HID_FeatReport[1];
            break;
    }

    usbd_hid_get_report(USBD_SetupPacket.wValueH, USBD_SetupPacket.wValueL, ptr_buf, USBD_HID_REQ_EP_CTRL);
    return (__TRUE);
}


/*
 *  USB Device HID Set Report Request Callback
 *   Called automatically on USB Device HID Set Report Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USBD_HID_SetReport(void)
{
    U8 *ptr_buf = 0;

    /* Report Type   = USBD_SetupPacket.wValueH */
    /* Report ID     = USBD_SetupPacket.wValueL */
    /* Report Length = USBD_SetupPacket.wLength */
    switch (USBD_SetupPacket.wValueH) {
        case HID_REPORT_INPUT:
            return (__FALSE);        /* Not Supported */

        case HID_REPORT_OUTPUT:
            ptr_buf  = &USBD_HID_OutReport[1];
            break;

        case HID_REPORT_FEATURE:
            ptr_buf  = &USBD_HID_FeatReport[1];
            break;
    }

    usbd_hid_set_report(USBD_SetupPacket.wValueH, USBD_SetupPacket.wValueL, ptr_buf, USBD_SetupPacket.wLength, USBD_HID_REQ_EP_CTRL);
    return (__TRUE);
}


/*
 *  USB Device HID Get Idle Request Callback
 *   Called automatically on USB Device HID Get Idle Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USBD_HID_GetIdle(void)
{
    USBD_EP0Buf[0] = USBD_HID_IdleSet[USBD_SetupPacket.wValueL];
    return (__TRUE);
}


/*
 *  USB Device HID Set Idle Request Callback
 *   Called automatically on USB Device HID Set Idle Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USBD_HID_SetIdle(void)
{
    U8 i;

    if (USBD_SetupPacket.wValueL) {       /* If  > 0 Report ID specified        */
        USBD_HID_IdleSet[USBD_SetupPacket.wValueL - 1] = USBD_SetupPacket.wValueH;
    } else {                              /* If == 0 all reports                */
        for (i = 0; i < usbd_hid_inreport_num; i++) {
            USBD_HID_IdleSet[i] = USBD_SetupPacket.wValueH;
        }
    }

    return (__TRUE);
}


/*
 *  USB Device HID Get Protocol Request Callback
 *   Called automatically on USB Device HID Get Protocol Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USBD_HID_GetProtocol(void)
{
    USBD_EP0Buf[0] = usbd_hid_get_protocol();
    return (__TRUE);
}


/*
 *  USB Device HID Set Protocol Request Callback
 *   Called automatically on USB Device HID Set Protocol Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USBD_HID_SetProtocol(void)
{
    usbd_hid_set_protocol(USBD_SetupPacket.wValueL);
    return (__TRUE);
}


/*
 *  USB Device HID Interrupt In Endpoint Event Callback
 *    Parameters:      event: not used (just for compatibility)
 *    Return Value:    None
 */

void USBD_HID_EP_INTIN_Event(U32 event)
{
    U8  i;
    U16 bytes_to_send;

    /* Check if sending is finished                                             */
    if ((DataOutSentLen >= DataOutToSendLen) &&
            !DataOutEndWithShortPacket) {     /* If all sent and short packet also  */
        ptrDataOut          = NULL;
        DataOutSentLen      = 0;
        DataOutToSendLen    = usbd_hid_get_report(HID_REPORT_INPUT, USBD_HID_InReport[0], &USBD_HID_InReport[1], USBD_HID_REQ_EP_INT);

        if (DataOutToSendLen) {             /* If new send should be started      */
            ptrDataOut        = USBD_HID_InReport;

            if (usbd_hid_inreport_num <= 1) { /* If only in 1 report skip ReportID  */
                ptrDataOut++;
            } else {                          /* If more in reports, send ReportID  */
                DataOutToSendLen++;
            }
        }
    }

    /* Check if new data out sending should be started                          */
    if (!DataOutToSendLen) {              /* If send not in progress            */
        if (DataOutAsyncReq) {              /* If asynchronous send requested     */
            DataOutAsyncReq = __FALSE;
        } else if (DataOutUpdateReqMask) {  /* If update requested                */
            if (usbd_hid_inreport_num <= 1) { /* If only one in report in system    */
                if (DataOutUpdateReqMask) {
                    USBD_HID_InReport[0]  = 0;    /* ReportID = 0                       */
                    DataOutSentLen        = 0;
                    DataOutToSendLen      = usbd_hid_get_report(HID_REPORT_INPUT, 0, &USBD_HID_InReport[1], USBD_HID_REQ_PERIOD_UPDATE);

                    if (DataOutToSendLen) {
                        ptrDataOut          = &USBD_HID_InReport[1];
                    }

                    DataOutUpdateReqMask  = 0;
                }
            } else {                          /* If multiple reports in system      */
                for (i = USBD_HID_InReport[0]; ; i++) {
                    if (i >= 32) {
                        i = 0;
                    }

                    if (DataOutUpdateReqMask & (1 << i)) {
                        USBD_HID_InReport[0] = i + 1; /* ReportID                           */
                        DataOutSentLen      = 0;
                        DataOutToSendLen    = usbd_hid_get_report(HID_REPORT_INPUT, i + 1, &USBD_HID_InReport[1], USBD_HID_REQ_PERIOD_UPDATE);

                        if (DataOutToSendLen) {
                            ptrDataOut        = USBD_HID_InReport;
                            DataOutToSendLen++;
                        }

                        DataOutUpdateReqMask &= ~(1 << i);
                        break;
                    }
                }
            }
        }
    }

    /* Check if data needs to be sent                                           */
    if (DataOutToSendLen ||
            DataOutEndWithShortPacket) {     /* If sending is in progress          */
        bytes_to_send = DataOutToSendLen - DataOutSentLen;

        if (bytes_to_send > usbd_hid_maxpacketsize[USBD_HighSpeed]) {
            bytes_to_send = usbd_hid_maxpacketsize[USBD_HighSpeed];
        }

        if (usbd_hid_ep_intin != 0) { //control ep does the sending to host
            USBD_WriteEP(usbd_hid_ep_intin | 0x80, ptrDataOut, bytes_to_send);
        }

        ptrDataOut     += bytes_to_send;
        DataOutSentLen += bytes_to_send;

        if ((DataOutSentLen < usbd_hid_inreport_max_sz) &&
                (bytes_to_send == usbd_hid_maxpacketsize[USBD_HighSpeed])) {
            /* If short packet should be sent also*/
            DataOutEndWithShortPacket = __TRUE;
        } else {
            DataOutEndWithShortPacket = __FALSE;
        }
    }
}


/*
 *  USB Device HID Interrupt Out Endpoint Event Callback
 *    Parameters:      event: not used (just for compatibility)
 *    Return Value:    None
 */

void USBD_HID_EP_INTOUT_Event(U32 event)
{
    U16 bytes_rece;

    if (!DataInReceLen) {                 /* Check if new reception             */
        ptrDataIn     = USBD_HID_OutReport;
        DataInReceMax = usbd_hid_outreport_max_sz;
        DataInReceLen = 0;
    }

    bytes_rece      = USBD_ReadEP(usbd_hid_ep_intout, ptrDataIn, DataInReceMax - DataInReceLen);
    ptrDataIn      += bytes_rece;
    DataInReceLen  += bytes_rece;

    if (!bytes_rece ||
            (DataInReceLen >= usbd_hid_outreport_max_sz) ||
            (bytes_rece    <  usbd_hid_maxpacketsize[USBD_HighSpeed])) {
        if (usbd_hid_outreport_num <= 1) {  /* If only one out report in system   */
            usbd_hid_set_report(HID_REPORT_OUTPUT,                    0 ,  USBD_HID_OutReport   , DataInReceLen,   USBD_HID_REQ_EP_INT);
        } else {
            usbd_hid_set_report(HID_REPORT_OUTPUT, USBD_HID_OutReport[0], &USBD_HID_OutReport[1], DataInReceLen - 1, USBD_HID_REQ_EP_INT);
        }

        DataInReceLen = 0;
    }
}


/*
 *  USB Device HID Configure Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_HID_Configure_Event(void)
{
    /* Reset all variables after connect event */
    USBD_HID_Protocol         = 0;
    DataOutAsyncReq           = __FALSE;
    DataOutUpdateReqMask      = __FALSE;
    ptrDataOut                = NULL;
    DataOutToSendLen          = 0;
    DataOutSentLen            = 0;
    DataOutEndWithShortPacket = __FALSE;
    ptrDataIn                 = NULL;
    DataInReceMax             = 0;
    DataInReceLen             = 0;
    ptrDataFeat               = NULL;
    DataFeatReceLen           = 0;
}


/*
 *  USB Device HID Interrupt In/Out Endpoint Event Callback
 *    Parameters:      event: USB Device Event
 *                       USBD_EVT_IN:  Input Event
 *                       USBD_EVT_OUT: Output Event
 *    Return Value:    None
 */

void USBD_HID_EP_INT_Event(U32 event)
{
    if (event & USBD_EVT_IN) {
        USBD_HID_EP_INTIN_Event(event);
    }

    if (event & USBD_EVT_OUT) {
        USBD_HID_EP_INTOUT_Event(event);
    }
}


/*
 *  USB Device HID SOF Handler (handles report timings: polling and idle times)
 *   Called automatically on USB Device Start of Frame
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_HID_SOF_Event(void)
{
    static U8   cnt_for_4ms = 0;
    U8   i;
    BOOL tick_4ms, do_polling, polling_reload, idle_reload;

    if (USBD_Configuration) {
        tick_4ms = __FALSE;

        if (cnt_for_4ms++ >= ((4 << (3 * USBD_HighSpeed))) - 1) {
            cnt_for_4ms = 0;
            tick_4ms    = __TRUE;
        }

        polling_reload = __FALSE;

        if (USBD_HID_PollingCnt < 255) {
            USBD_HID_PollingCnt++;
        }

        if (USBD_HID_PollingCnt == usbd_hid_interval[USBD_HighSpeed]) {
            polling_reload = __TRUE;          /* If polling interval expired        */
        }

        for (i = 0; i < usbd_hid_inreport_num; i++) {
            idle_reload = __FALSE;

            if (tick_4ms) {
                if (USBD_HID_IdleCnt[i] < 255) {
                    USBD_HID_IdleCnt[i]++;
                }

                if (USBD_HID_IdleReload[i]) {
                    if (USBD_HID_IdleCnt[i] >= USBD_HID_IdleReload[i]) {
                        idle_reload = __TRUE;       /* If idle period expired             */
                    }
                }
            }

            do_polling = (usbd_hid_interval[USBD_HighSpeed] > ((U16)(USBD_HID_IdleReload[i]) << (2 << (3 * USBD_HighSpeed)))) && (USBD_HID_IdleReload[i] != 0);

            if (polling_reload) {
                if (do_polling) {
                    /* If polling is longer than idle     */
                    DataOutUpdateReqMask |= (1 << i);
                }
            }

            if (USBD_HID_IdleReload[i] != USBD_HID_IdleSet[i]) {
                if (USBD_HID_IdleCnt[i] >= USBD_HID_IdleSet[i]) {
                    DataOutUpdateReqMask |= (1 << i);
                    cnt_for_4ms = 0;
                }

                USBD_HID_IdleReload[i] = USBD_HID_IdleSet[i];
            }

            if (idle_reload) {
                if (!do_polling) {
                    DataOutUpdateReqMask |= (1 << i);
                }

                USBD_HID_IdleCnt[i] = 0;
            }
        }

        if (polling_reload) {
            USBD_HID_PollingCnt = 0;
        }

        if (DataOutUpdateReqMask && !DataOutToSendLen) {        /* If pending     */
            /* refresh request and no active data */
            /* out then start data out            */
            USBD_HID_EP_INTIN_Event(0);
        }
    }
}


#ifdef __RTX                            /* RTX task for handling events */

/*
 *  USB Device HID Interrupt In Endpoint Event Handler Task
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_RTX_HID_EP_INTIN_Event(void)
{
    for (;;) {
        usbd_os_evt_wait_or(0xFFFF, 0xFFFF);

        if (usbd_os_evt_get() & USBD_EVT_IN) {
            USBD_HID_EP_INTIN_Event(0);
        }
    }
}


/*
 *  USB Device HID Interrupt Out Endpoint Event Handler Task
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_RTX_HID_EP_INTOUT_Event(void)
{
    for (;;) {
        usbd_os_evt_wait_or(0xFFFF, 0xFFFF);

        if (usbd_os_evt_get() & USBD_EVT_OUT) {
            USBD_HID_EP_INTOUT_Event(0);
        }
    }
}


/*
 *  USB Device HID Interrupt In/Out Endpoint Event Handler Task
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_RTX_HID_EP_INT_Event(void)
{
    for (;;) {
        usbd_os_evt_wait_or(0xFFFF, 0xFFFF);
        USBD_HID_EP_INT_Event(usbd_os_evt_get());
    }
}
#endif


/*
 *  USB Device HID Get Report Trigger (asynchronous Get_Report request)
 *    Parameters:      rid: Report ID
 *                     buf: Pointer to data buffer
 *                     len: Number of bytes to be sent
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL usbd_hid_get_report_trigger(U8 rid, U8 *buf, int len)
{
    if (len > usbd_hid_inreport_max_sz) {
        return (__FALSE);
    }

    if (USBD_Configuration) {
        DataOutAsyncReq    = __TRUE;        /* Asynchronous data out request      */

        while (DataOutToSendLen) {
            if (!USBD_Configuration) {        /* If device not configured reject rq */
                DataOutAsyncReq    = __FALSE;   /* Asynchronous data out request      */
                ptrDataOut         = NULL;
                DataOutSentLen     = 0;
                DataOutToSendLen   = 0;
                return (__FALSE);
            }
        }

        USBD_HID_InReport[0]   = rid;
        memcpy(&USBD_HID_InReport[1], buf, len);
        ptrDataOut             = USBD_HID_InReport;
        DataOutSentLen         = 0;
        DataOutToSendLen       = len;

        if (usbd_hid_inreport_num <= 1) {   /* If only 1 in report skip ReportID  */
            ptrDataOut ++;
        } else {                            /* If more in reports, send ReportID  */
            DataOutToSendLen ++;
        }

        USBD_HID_EP_INTIN_Event(0);
        USBD_HID_IdleCnt[rid]      = 0;
        return (__TRUE);
    }

    return (__FALSE);
}
