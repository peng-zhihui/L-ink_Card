/**
 * @file    usbd_cdc_acm.c
 * @brief   Communication Device Class driver
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


/* Module global variables                                                    */

/** \ingroup  USBD_CDC_ACM_global_variables
    \defgroup USBD_CDC_ACM_GLOBAL_VAR  Global Variables (GLOBAL_VAR)
    \brief      Global variables used in USBD CDC ACM module
 */
int32_t data_send_access;              /*!< Flag active while send data (in the send intermediate buffer) is being accessed */
int32_t data_send_active;              /*!< Flag active while data is being sent */
int32_t data_send_zlp;                 /*!< Flag active when ZLP needs to be sent */
int32_t data_to_send_wr;               /*!< Number of bytes written to the send intermediate buffer */
int32_t data_to_send_rd;               /*!< Number of bytes read from the send intermediate buffer */
uint8_t *ptr_data_to_send;             /*!< Pointer to the send intermediate buffer to the data to be sent */
uint8_t *ptr_data_sent;                /*!< Pointer to the send intermediate buffer to the data already sent */

int32_t data_read_access;              /*!< Flag active while read data (in the receive intermediate buffer) is being accessed */
int32_t data_receive_int_access;       /*!< Flag active while read data (in the receive intermediate buffer) is being accessed from the IRQ function*/
int32_t data_received_pending_pckts;   /*!< Number of packets received but not handled (pending) */
int32_t data_no_space_for_receive;     /*!< Flag active while there is no more space for reception */
uint8_t *ptr_data_received;            /*!< Pointer to the receive intermediate buffer to the received unread data */
uint8_t *ptr_data_read;                /*!< Pointer to the receive intermediate buffer to the received read data */

uint16_t control_line_state;           /*!< Control line state settings bitmap (0. bit - DTR state, 1. bit - RTS state) */

CDC_LINE_CODING line_coding;           /*!< Communication settings */

/* end of group USBD_CDC_ACM_GLOBAL_VAR */


/* Functions that should be provided by user to use standard Virtual COM port
   functionality                                                              */
__weak int32_t USBD_CDC_ACM_PortInitialize(void)
{
    return (0);
}
__weak int32_t USBD_CDC_ACM_PortUninitialize(void)
{
    return (0);
}
__weak int32_t USBD_CDC_ACM_PortReset(void)
{
    return (0);
}
__weak int32_t USBD_CDC_ACM_PortSetLineCoding(CDC_LINE_CODING *line_coding)
{
    return (0);
}
__weak int32_t USBD_CDC_ACM_PortGetLineCoding(CDC_LINE_CODING *line_coding)
{
    return (0);
}
__weak int32_t USBD_CDC_ACM_PortSetControlLineState(uint16_t ctrl_bmp)
{
    return (0);
}

/* Functions that can be used by user to use standard Virtual COM port
   functionality                                                              */
int32_t USBD_CDC_ACM_DataSend(const uint8_t *buf, int32_t len);
int32_t USBD_CDC_ACM_PutChar(const uint8_t  ch);
int32_t USBD_CDC_ACM_DataRead(uint8_t *buf, int32_t len);
int32_t USBD_CDC_ACM_GetChar(void);
__weak int32_t USBD_CDC_ACM_DataReceived(int32_t len)
{
    return (0);
}
int32_t USBD_CDC_ACM_DataAvailable(void);
int32_t USBD_CDC_ACM_Notify(uint16_t stat);

/* Functions handling CDC ACM requests (can be overridden to provide custom
   handling of CDC ACM requests)                                              */
__weak int32_t USBD_CDC_ACM_SendEncapsulatedCommand(void)
{
    return (0);
}
__weak int32_t USBD_CDC_ACM_GetEncapsulatedResponse(void)
{
    return (0);
}
__weak int32_t USBD_CDC_ACM_SetCommFeature(uint16_t feat)
{
    return (0);
}
__weak int32_t USBD_CDC_ACM_GetCommFeature(uint16_t feat)
{
    return (0);
}
__weak int32_t USBD_CDC_ACM_ClearCommFeature(uint16_t feat)
{
    return (0);
}
__weak int32_t USBD_CDC_ACM_SendBreak(uint16_t dur)
{
    return (0);
}


/* Local function prototypes                                                  */
static void USBD_CDC_ACM_EP_BULKOUT_HandleData(void);
static void USBD_CDC_ACM_EP_BULKIN_HandleData(void);


/*----------------- USB CDC ACM class handling functions ---------------------*/

/** \brief  Initialization of the USB CDC class (ACM)

    The function calls USBD_CDC_ACM_PortInitialize function which
    initializes Virtual COM Port.

    \return             0        Function failed.
    \return             1        Function succeeded.
 */

__weak int32_t USBD_CDC_ACM_Initialize(void)
{
    data_send_access            = 0;
    data_send_active            = 0;
    data_send_zlp               = 0;
    data_to_send_wr             = 0;
    data_to_send_rd             = 0;
    ptr_data_to_send            = USBD_CDC_ACM_SendBuf;
    ptr_data_sent               = USBD_CDC_ACM_SendBuf;
    data_read_access            = 0;
    data_receive_int_access     = 0;
    data_received_pending_pckts = 0;
    data_no_space_for_receive   = 0;
    ptr_data_received           = USBD_CDC_ACM_ReceiveBuf;
    ptr_data_read               = USBD_CDC_ACM_ReceiveBuf;
    control_line_state          = 0;
    line_coding.dwDTERate       = 9600;
    line_coding.bCharFormat     = 0;
    line_coding.bParityType     = 0;
    line_coding.bDataBits       = 8;
    return (USBD_CDC_ACM_PortInitialize());
}


/** \brief  Uninitialization of the USB CDC class (ACM)

    The function calls USBD_CDC_ACM_PortUninitialize function which
    uninitializes Virtual COM Port.

    \return             0        Function failed.
    \return             1        Function succeeded.
 */

__weak int32_t USBD_CDC_ACM_Uninitialization(void)
{
    return (USBD_CDC_ACM_PortUninitialize());
}


/** \brief  Reset of the USB CDC class (ACM) variables and states

    The function resets class variables and states, it calls
    USBD_CDC_ACM_PortReset function which resets Virtual COM Port variables
    and states and calls USBD_CDC_ACM_PortSetLineCoding function with
    default parameters to set default communication settings for the
    Virtual COM Port.

    \return             0        Function failed.
    \return             1        Function succeeded.
 */

__weak int32_t USBD_CDC_ACM_Reset(void)
{
    data_send_access            = 0;
    data_send_active            = 0;
    data_send_zlp               = 0;
    data_to_send_wr             = 0;
    data_to_send_rd             = 0;
    ptr_data_to_send            = USBD_CDC_ACM_SendBuf;
    ptr_data_sent               = USBD_CDC_ACM_SendBuf;
    data_read_access            = 0;
    data_receive_int_access     = 0;
    data_received_pending_pckts = 0;
    data_no_space_for_receive   = 0;
    ptr_data_received           = USBD_CDC_ACM_ReceiveBuf;
    ptr_data_read               = USBD_CDC_ACM_ReceiveBuf;
    control_line_state          = 0;
    USBD_CDC_ACM_PortReset();
    line_coding.dwDTERate       = 9600;
    line_coding.bCharFormat     = 0;
    line_coding.bParityType     = 0;
    line_coding.bDataBits       = 8;
    return (USBD_CDC_ACM_PortSetLineCoding(&line_coding));
}


/** \brief  Sets Line Coding for the USB CDC ACM Virtual COM Port

    The function is a callback function that forwards USB CDC ACM request
    to set communication settings to the Virtual COM Port.

    \return             0        Function failed.
    \return             1        Function succeeded.
 */

__weak int32_t USBD_CDC_ACM_SetLineCoding(void)
{
    line_coding.dwDTERate   = (USBD_EP0Buf[0] <<  0) |
                              (USBD_EP0Buf[1] <<  8) |
                              (USBD_EP0Buf[2] << 16) |
                              (USBD_EP0Buf[3] << 24) ;
    line_coding.bCharFormat =  USBD_EP0Buf[4];
    line_coding.bParityType =  USBD_EP0Buf[5];
    line_coding.bDataBits   =  USBD_EP0Buf[6];
    return (USBD_CDC_ACM_PortSetLineCoding(&line_coding));
}


/** \brief  Gets Line Coding from the USB CDC ACM Virtual COM Port

    The function is a callback function that forwards USB CDC ACM request
    to get communication settings from the Virtual COM Port.

    \return             0        Function failed.
    \return             1        Function succeeded.
 */

__weak int32_t USBD_CDC_ACM_GetLineCoding(void)
{
    if (USBD_CDC_ACM_PortGetLineCoding(&line_coding)) {
        USBD_EP0Buf[0] = (line_coding.dwDTERate >>  0) & 0xFF;
        USBD_EP0Buf[1] = (line_coding.dwDTERate >>  8) & 0xFF;
        USBD_EP0Buf[2] = (line_coding.dwDTERate >> 16) & 0xFF;
        USBD_EP0Buf[3] = (line_coding.dwDTERate >> 24) & 0xFF;
        USBD_EP0Buf[4] =  line_coding.bCharFormat;
        USBD_EP0Buf[5] =  line_coding.bParityType;
        USBD_EP0Buf[6] =  line_coding.bDataBits;
        return (1);
    }

    return (0);
}


/** \brief  Sets Control Line State for the USB CDC ACM Virtual COM Port

    The function is a callback function that forwards USB CDC ACM request
    to set desired control line state to the Virtual COM Port.

    \param [in]         ctrl_bmp Control line settings bitmap (
                          0. bit - DTR state,
                          1. bit - RTS state).
    \return             0        Function failed.
    \return             1        Function succeeded.
 */

__weak int32_t USBD_CDC_ACM_SetControlLineState(uint16_t ctrl_bmp)
{
    control_line_state = ctrl_bmp;
    return (USBD_CDC_ACM_PortSetControlLineState(ctrl_bmp));
}


/*----------------- USB CDC ACM user API functions ---------------------------*/

/** \brief Number of free bytes in the Send buffer
*/
int32_t USBD_CDC_ACM_DataFree(void)
{
    return ((int32_t)usbd_cdc_acm_sendbuf_sz) - (data_to_send_wr - data_to_send_rd);
}

/** \brief  Sends data over the USB CDC ACM Virtual COM Port

    The function puts requested data to the send intermediate buffer and
    prepares it for sending over the Virtual COM Port.

    \param [in]         buf      Buffer containing data to be sent.
    \param [in]         len      Maximum number of bytes to be sent.
    \return                      Number of bytes accepted to be sent.
 */

int32_t USBD_CDC_ACM_DataSend(const uint8_t *buf, int32_t len)
{
    int32_t  len_data, len_available, len_before_wrap;
    uint8_t *buf_loc;
    buf_loc       = (uint8_t *)buf;       /* Pointer to buf                     */
    len_data      = data_to_send_wr - data_to_send_rd;  /* Num of data in buffer*/
    len_available = ((int32_t)usbd_cdc_acm_sendbuf_sz) - len_data;  /* Num of
                                           bytes of space available           */

    if (len_available <= 0) {             /* If no space for data to send       */
        return (0);
    }

    if (len > len_available)              /* If more data requested for sending
                                           then available space               */
    {
        len = len_available;    /* Correct to maximum available       */
    }

    len_before_wrap = 0;                  /* Circular buffer size before wrap   */

    if ((ptr_data_to_send >= ptr_data_sent) && /* If wrap is possible to happen */
            ((ptr_data_to_send + len) >= (USBD_CDC_ACM_SendBuf + usbd_cdc_acm_sendbuf_sz))) {
        /* If data wraps around end of buffer */
        len_before_wrap   = USBD_CDC_ACM_SendBuf + usbd_cdc_acm_sendbuf_sz - ptr_data_to_send;
        memcpy(ptr_data_to_send, buf_loc, len_before_wrap); /* Copy data till end */
        buf_loc          += len_before_wrap;            /* Increment buf pointer  */
        len              -= len_before_wrap;            /* Decrement bytes to send*/
        ptr_data_to_send  = USBD_CDC_ACM_SendBuf;       /* Wrap send buffer
                                                       pointer to beginning of
                                                       the send buffer        */
    }

    if (len) {                            /* If there are bytes to send         */
        memcpy(ptr_data_to_send, buf_loc, len);   /* Copy data to send buffer     */
        ptr_data_to_send += len;            /* Correct position of write pointer  */
    }

    len += len_before_wrap;               /* Total number of bytes prepared for
                                           send                               */
    data_to_send_wr += len;               /* Bytes prepared to send counter     */
    return (len);                         /* Number of bytes accepted for send  */
}


/** \brief  Sends a single character over the USB CDC ACM Virtual COM Port

    The function puts requested data character to the send intermediate buffer
    and prepares it for sending over the Virtual COM Port.

    \param [in]         ch       Character to be sent.
    \return             -1       Function failed.
    \return                      Character accepted to be sent.
 */

int32_t USBD_CDC_ACM_PutChar(const uint8_t ch)
{
    if ((USBD_CDC_ACM_DataSend(&ch, 1)) == 1) {
        return ((uint32_t) ch);
    }

    return (-1);
}


/** \brief  Reads data received over the USB CDC ACM Virtual COM Port

    The function reads data from the receive intermediate buffer that was
    received over the Virtual COM Port.

    \param [in]         buf      Buffer to where data will be read.
    \param [in]         len      Maximum number of bytes to be read.
    \return                      Number of bytes actually read.
 */

int32_t USBD_CDC_ACM_DataRead(uint8_t *buf, int32_t len)
{
    int32_t len_data;

    if (ptr_data_received > ptr_data_read) { /*If there is already received data   */
        len_data = ptr_data_received - ptr_data_read; /* Available bytes of data  */

        if (len > len_data) {               /* If more requested then available   */
            len = len_data;    /* correct to return maximum available*/
        }

        memcpy(buf, ptr_data_read, len);    /* Copy received data to provided buf */
        ptr_data_read      += len;          /* Correct position of read pointer   */
    } else {
        len = 0;                            /* No data received                   */
    }

    return (len);                         /* Number of bytes actually read      */
}


/** \brief  Reads one character of data received over the USB CDC ACM Virtual COM Port

    The function reads data character from the receive intermediate buffer that
    was received over the Virtual COM Port.

    \return             -1       No character available.
    \return                      Received character.
 */

int32_t USBD_CDC_ACM_GetChar(void)
{
    uint8_t ch;

    if ((USBD_CDC_ACM_DataRead(&ch, 1)) == 1) {
        return ((int32_t) ch);
    }

    return (-1);
}


/** \brief  Retrieves number of bytes received over the USB CDC ACM Virtual COM Port

    The function retrieves number of bytes available in the intermediate buffer
    that were received over the Virtual COM Port.

    \return                      Number of bytes available for read.
 */

int32_t USBD_CDC_ACM_DataAvailable(void)
{
    return (ptr_data_received - ptr_data_read);
}


/** \brief  Sends a notification of Virtual COM Port statuses and line states

    The function sends error and line status of the Virtual COM Port over the
    Interrupt endpoint. (SerialState notification is defined in usbcdc11.pdf, 6.3.5.)

    \param [in]         stat     Error and line statuses (
                                   6. bit - bOverRun,
                                   5. bit - bParity,
                                   4. bit - bFraming,
                                   3. bit - bRingSignal,
                                   2. bit - bBreak,
                                   1. bit - bTxCarrier (DSR line state),
                                   0. bit - bRxCarrier (DCD line status)).
    \return             0        Function failed.
    \return             1        Function succeeded.
 */

int32_t USBD_CDC_ACM_Notify(uint16_t stat)
{
    if (USBD_Configuration) {
        USBD_CDC_ACM_NotifyBuf[0] = 0xA1;   /* bmRequestType                      */
        USBD_CDC_ACM_NotifyBuf[1] = CDC_NOTIFICATION_SERIAL_STATE;/* bNotification
                                          (SERIAL_STATE)                      */
        USBD_CDC_ACM_NotifyBuf[2] = 0x00;   /* wValue                             */
        USBD_CDC_ACM_NotifyBuf[3] = 0x00;
        USBD_CDC_ACM_NotifyBuf[4] = 0x00;   /* wIndex (Interface 0)               */
        USBD_CDC_ACM_NotifyBuf[5] = 0x00;
        USBD_CDC_ACM_NotifyBuf[6] = 0x02;   /* wLength                            */
        USBD_CDC_ACM_NotifyBuf[7] = 0x00;
        USBD_CDC_ACM_NotifyBuf[8] = stat >> 0; /* UART State Bitmap                  */
        USBD_CDC_ACM_NotifyBuf[9] = stat >> 8;
        /* Write notification to be sent      */
        USBD_WriteEP(usbd_cdc_acm_ep_intin | 0x80, USBD_CDC_ACM_NotifyBuf, 10);
        return (1);
    }

    return (0);
}


/*----------------- USB CDC ACM communication event handlers -----------------*/

/** \brief  Handle Reset Events

    The function handles Reset events.
 */

void USBD_CDC_ACM_Reset_Event(void)
{
    USBD_CDC_ACM_Reset();
}


/** \brief  Handle SOF Events

    The function handles Start Of Frame events. It checks if there is pending
    data on the Bulk Out endpoint and handles it
    (USBD_CDC_ACM_EP_BULKOUT_HandleData) if there is enough space in the
    intermediate receive buffer and it calls received function callback
    (USBD_CDC_ACM_DataReceived) it also activates data send over the Bulk In
    endpoint if there is data to be sent (USBD_CDC_ACM_EP_BULKIN_HandleData).
 */

void USBD_CDC_ACM_SOF_Event(void)
{
    if (!USBD_Configuration) {
        // Don't process events until CDC is
        // configured and the endpoints enabled
        return;
    }
    if ((!data_read_access)         &&    /* If not read active                 */
            (ptr_data_received == ptr_data_read) &&     /* If received and read
                                                     pointers point to same
                                                     the location             */
            (ptr_data_received != USBD_CDC_ACM_ReceiveBuf)) {
        /* and if receive
                                                       pointer does not already
                                                       point to the start of
                                                       the receive buffer       */
        data_read_access = 1;               /* Block access to read data          */
        ptr_data_received = USBD_CDC_ACM_ReceiveBuf;  /* Correct received pointer
                                                     to point to the start of
                                                     the receive buffer       */
        ptr_data_read     = USBD_CDC_ACM_ReceiveBuf;  /* Correct read pointer to
                                                     point to the start of the
                                                     receive buffer           */
        data_no_space_for_receive  = 0;               /* There is space for
                                                     reception available      */
        data_read_access = 0;               /* Allow access to read data          */
    }

    if (data_received_pending_pckts &&    /* If packets are pending             */
            (!data_read_access)          &&    /* and if not read active             */
            (!data_no_space_for_receive)) {    /* and if there is space to receive   */
        data_read_access = 1;               /* Disable access to read data        */
        USBD_CDC_ACM_EP_BULKOUT_HandleData(); /* Handle received data             */
        data_read_access = 0;               /* Enable access to read data         */

        if (ptr_data_received != ptr_data_read) {
            USBD_CDC_ACM_DataReceived(ptr_data_received - ptr_data_read);
        }  /* Call

                                           received callback                  */
    }

    if ((!data_send_access)         &&    /* If send data is not being accessed */
            (!data_send_active)         &&    /* and send is not active             */
            (data_to_send_wr - data_to_send_rd) /* and if there is data to be sent    */
//&& ((control_line_state & 3) == 3)    /* and if DTR and RTS is 1            */
       ) {
        data_send_access = 1;               /* Block access to send data          */
        data_send_active = 1;               /* Start data sending                 */
        USBD_CDC_ACM_EP_BULKIN_HandleData();/* Handle data to send                */
        data_send_access = 0;               /* Allow access to send data          */
    }
}


/** \brief  Handle Interrupt In Endpoint Events

    The function handles Interrupt In endpoint events.

    \param [in]         event    Type of event (USBD_EVT_IN - input event).
 */

void USBD_CDC_ACM_EP_INTIN_Event(uint32_t event)
{
    /* Notification will be loadad aynchronously and sent automatically upon
       Interrupt IN token reception                                             */
}


/** \brief  Handle Bulk Out Endpoint Received Data

    The function handles data received on the Bulk Out endpoint. It reads the
    received data to the receive intermediate buffer if there is enough space
    available.
 */

static void USBD_CDC_ACM_EP_BULKOUT_HandleData()
{
    uint32_t len_free_to_recv;
    int32_t len_received;

    if ((usbd_cdc_acm_receivebuf_sz - (ptr_data_received - USBD_CDC_ACM_ReceiveBuf)) >= usbd_cdc_acm_maxpacketsize1[USBD_HighSpeed]) {
        /* If there is space for 1 max packet */
        /* Read received packet to receive buf*/
        len_free_to_recv = usbd_cdc_acm_receivebuf_sz - (ptr_data_received - USBD_CDC_ACM_ReceiveBuf);
        len_received       = USBD_ReadEP(usbd_cdc_acm_ep_bulkout, ptr_data_received, len_free_to_recv);
        ptr_data_received += len_received;  /* Correct pointer to received data   */

        if (data_received_pending_pckts &&  /* If packet was pending              */
                !data_receive_int_access) {      /* and not interrupt access           */
            data_received_pending_pckts--;    /* Decrement pending packets number   */
        }
    } else {
        data_no_space_for_receive = 1;      /* There is no space in receive buffer
                                           for the newly received data        */

        if (data_receive_int_access) {
            /* If this access is from interrupt
                                                   function                           */
            data_received_pending_pckts++;    /* then this is new unhandled packet  */
        }
    }
}


/** \brief  Handle Bulk In Endpoint Data to Send

    The function handles data to be sent on the Bulk In endpoint. It transmits
    pending data to be sent that is already in the send intermediate buffer,
    and it also sends Zero Length Packet if last packet sent was not a short
    packet.
 */

static void USBD_CDC_ACM_EP_BULKIN_HandleData(void)
{
    int32_t len_to_send, len_sent;

    if (!data_send_active) {              /* If sending is not active           */
        return;
    }

    len_to_send = data_to_send_wr - data_to_send_rd;  /* Num of data to send    */

    /* Check if sending is finished                                             */
    if (!len_to_send    &&                /* If all data was sent               */
            !data_send_zlp)  {                /* and ZLP was sent if necessary also */
        data_send_active = 0;               /* Sending not active any more        */
        return;
    }

    /* Check if data needs to be sent                                           */
    if (len_to_send) {
        /* If there is data available do be
                                                 sent                               */
        if ((ptr_data_sent >= ptr_data_to_send) && /* If data before end of buf avail*/
                ((ptr_data_sent + len_to_send) >= (USBD_CDC_ACM_SendBuf + usbd_cdc_acm_sendbuf_sz))) {
            /* and if available data wraps around
               the end of the send buffer         */
            /* Correct bytes to send to data
               available untill end of send buf   */
            len_to_send = USBD_CDC_ACM_SendBuf + usbd_cdc_acm_sendbuf_sz - ptr_data_sent;
        }

        if (len_to_send > usbd_cdc_acm_maxpacketsize1[USBD_HighSpeed]) {
            /* If
                                                   there is more data to be sent then
                                                   can be sent in a single packet     */
            /* Correct to send maximum pckt size  */
            len_to_send = usbd_cdc_acm_maxpacketsize1[USBD_HighSpeed];
        }
    } else if (data_send_zlp) {           /* or if ZLP should be sent           */
        len_to_send = 0;
    }

    data_send_zlp = 0;
    /* Send data                          */
    len_sent = USBD_WriteEP(usbd_cdc_acm_ep_bulkin | 0x80, ptr_data_sent, len_to_send);
    ptr_data_sent    += len_sent;         /* Correct position of sent pointer   */
    data_to_send_rd  += len_sent;         /* Correct num of bytes left to send  */

    if (ptr_data_sent == USBD_CDC_ACM_SendBuf + usbd_cdc_acm_sendbuf_sz)
        /* If pointer to sent data wraps      */
    {
        ptr_data_sent = USBD_CDC_ACM_SendBuf;
    } /* Correct it to beginning of send

                                           buffer                             */

    if ((data_to_send_wr == data_to_send_rd) &&   /* If there are no more
                                           bytes available to be sent         */
            (len_sent == usbd_cdc_acm_maxpacketsize1[USBD_HighSpeed])) {
        /* If last packet size was same as
           maximum packet size                */
        data_send_zlp = 1;                  /* ZLP packet should be sent          */
    } else {
        data_send_zlp = 0;                  /* No ZLP packet should be sent       */
    }
}


/** \brief  Handle Bulk Out Endpoint Events

    The function handles Bulk Out endpoint events. It calls
    USBD_CDC_ACM_EP_BULKOUT_HandleData function to handle received data
    unless data was being accessed in which case function just acknowledges
    that there is data to be handled later.

    \param [in]         event    Type of event (USBD_EVT_OUT - output event).
 */

void USBD_CDC_ACM_EP_BULKOUT_Event(uint32_t event)
{
    if (data_read_access) {
        /* If data is being accessed from
                                                 read function                      */
        data_received_pending_pckts++;      /* 1 more packet received and not
                                           handled                            */
        return;
    }

    data_read_access = 1;                 /* Block access to read data          */
    data_receive_int_access = 1;          /* Read access from interrupt function*/
    USBD_CDC_ACM_EP_BULKOUT_HandleData(); /* Handle received data               */
    data_receive_int_access = 0;          /* Read access from interrupt func end*/
    data_read_access = 0;                 /* Allow access to read data          */

    if (ptr_data_received != ptr_data_read) {
        USBD_CDC_ACM_DataReceived(ptr_data_received - ptr_data_read);
    }    /* Call

                                           received callback                  */
}


/** \brief  Handle Bulk In Endpoint Events

    The function handles Bulk In endpoint events. It calls
    USBD_CDC_ACM_EP_BULKIN_HandleData function to handle send data
    unless data was being accessed in which case function just returns.

    \param [in]         event    Type of event (USBD_EVT_IN - input event).
 */

void USBD_CDC_ACM_EP_BULKIN_Event(uint32_t event)
{
    if (data_send_access                  /* If send data is being accessed     */
// ||((control_line_state & 3) != 3)    /* or if DTR or RTS is 0              */
       ) {
        return;
    }

    data_send_access = 1;                 /* Block access to send data          */
    USBD_CDC_ACM_EP_BULKIN_HandleData();  /* Handle data to send                */
    data_send_access = 0;                 /* Allow access to send data          */
}


/** \brief  Handle Bulk In/Out Endpoint Events

    The function handles Bulk In/Out endpoint events. It is used for endpoints
    that do In and Out functionality on the same endpoint number. It dispatches
    events to appropriate In or Out event handlers.

    \param [in]         event    Type of event (
                                   USBD_EVT_IN  - input event,
                                   USBD_EVT_OUT - output event).
 */

void USBD_CDC_ACM_EP_BULK_Event(uint32_t event)
{
    if (event & USBD_EVT_OUT) {
        USBD_CDC_ACM_EP_BULKOUT_Event(event);
    }

    if (event & USBD_EVT_IN) {
        USBD_CDC_ACM_EP_BULKIN_Event(event);
    }
}


#ifdef __RTX                            /* RTX tasks for handling events      */

/** \brief  Task Handling Interrupt In Endpoint Events

    The task dispatches Interrupt In events to the Interrupt In handling
    function (USBD_CDC_ACM_EP_INTIN_Event).
 */

void USBD_RTX_CDC_ACM_EP_INTIN_Event(void)
{
    for (;;) {
        usbd_os_evt_wait_or(0xFFFF, 0xFFFF);
        USBD_CDC_ACM_EP_INTIN_Event(usbd_os_evt_get());
    }
}


/** \brief  Task Handling Bulk In Endpoint Events

    The task dispatches Bulk In events to the Bulk In handling
    function (USBD_CDC_ACM_EP_BULKIN_Event).
 */

void USBD_RTX_CDC_ACM_EP_BULKIN_Event(void)
{
    for (;;) {
        usbd_os_evt_wait_or(0xFFFF, 0xFFFF);

        if (usbd_os_evt_get() & USBD_EVT_IN) {
            USBD_CDC_ACM_EP_BULKIN_Event(0);
        }
    }
}


/** \brief  Task Handling Bulk Out Endpoint Events

    The task dispatches Bulk Out events to the Bulk Out handling
    function (USBD_CDC_ACM_EP_BULKOUT_Event).
 */

void USBD_RTX_CDC_ACM_EP_BULKOUT_Event(void)
{
    for (;;) {
        usbd_os_evt_wait_or(0xFFFF, 0xFFFF);

        if (usbd_os_evt_get() & USBD_EVT_OUT) {
            USBD_CDC_ACM_EP_BULKOUT_Event(0);
        }
    }
}


/** \brief  Task Handling Bulk In/Out Endpoint Events

    The task dispatches Bulk In/Out events to the Bulk In/Out handling
    function (USBD_CDC_ACM_EP_BULK_Event).
 */

void USBD_RTX_CDC_ACM_EP_BULK_Event(void)
{
    for (;;) {
        usbd_os_evt_wait_or(0xFFFF, 0xFFFF);
        USBD_CDC_ACM_EP_BULK_Event(usbd_os_evt_get());
    }
}
#endif

