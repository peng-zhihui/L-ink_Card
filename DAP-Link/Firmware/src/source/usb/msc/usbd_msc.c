/**
 * @file    usbd_msc.c
 * @brief   Mass Storage Class driver
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
#include "util.h"

BOOL USBD_MSC_MediaReady = __FALSE;
BOOL USBD_MSC_ReadOnly = __FALSE;
U32 USBD_MSC_MemorySize;
U32 USBD_MSC_BlockSize;
U32 USBD_MSC_BlockGroup;
U32 USBD_MSC_BlockCount;
U8 *USBD_MSC_BlockBuf;

MSC_CBW USBD_MSC_CBW;       /* Command Block Wrapper */
MSC_CSW USBD_MSC_CSW;       /* Command Status Wrapper */

BOOL USBD_MSC_MediaReadyEx = __FALSE;   /* Previous state of Media ready */
BOOL MemOK;     /* Memory OK */

U32 Block;      /* R/W Block  */
U32 Offset;     /* R/W Offset */
U32 Length;     /* R/W Length */

U8 BulkStage;   /* Bulk Stage */
U32 BulkLen;    /* Bulk In/Out Length */


/* Dummy Weak Functions that need to be provided by user */
__weak void usbd_msc_init()
{

}
__weak void usbd_msc_read_sect(U32 block, U8 *buf, U32 num_of_blocks)
{

}
__weak void usbd_msc_write_sect(U32 block, U8 *buf, U32 num_of_blocks)
{

}
__weak void usbd_msc_start_stop(BOOL start)
{

}


/*
 *  Set Stall for USB Device MSC Endpoint
 *    Parameters:      EPNum: USB Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_MSC_SetStallEP(U32 EPNum)        /* set EP halt status according stall status */
{
    USBD_SetStallEP(EPNum);
    USBD_EndPointHalt  |= (EPNum & 0x80) ? ((1 << 16) << (EPNum & 0x0F)) : (1 << EPNum);
}


/*
 *  Clear Stall for USB Device MSC Endpoint
 *    Parameters:      EPNum: USB Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_MSC_ClrStallEP(U32 EPNum)        /* clear EP halt status according stall status */
{
    U32 n, m;
    n = USBD_SetupPacket.wIndexL & 0x8F;
    m = (n & 0x80) ? ((1 << 16) << (n & 0x0F)) : (1 << n);

    if ((n == (usbd_msc_ep_bulkin | 0x80)) && ((USBD_EndPointHalt & m) != 0)) {
        /* Compliance Test: rewrite CSW after unstall */
        if (USBD_MSC_CSW.dSignature == MSC_CSW_Signature) {
            USBD_WriteEP((usbd_msc_ep_bulkin | 0x80), (U8 *)&USBD_MSC_CSW, sizeof(USBD_MSC_CSW));
        }
    }
}


/*
 *  USB Device MSC Mass Storage Reset Request Callback
 *   Called automatically on USB Device Mass Storage Reset Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USBD_MSC_Reset(void)
{
    USBD_EndPointStall = 0x00000000;         /* EP must stay stalled */
    USBD_MSC_CSW.dSignature = 0;             /* invalid signature */
    BulkStage = MSC_BS_RESET;
    return (__TRUE);
}


/*
 *  USB Device MSC Get Max LUN Request Callback
 *   Called automatically on USB Device Get Max LUN Request
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USBD_MSC_GetMaxLUN(void)
{
    USBD_EP0Buf[0] = 0;                      /* one LUN associated with this device */
    return (__TRUE);
}


/*
 *  USB Device Check Media Ready
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USBD_MSC_CheckMedia(void)
{
    USBD_MSC_MediaReadyEx = USBD_MSC_MediaReady;

    if (!USBD_MSC_MediaReady) {
        if (USBD_MSC_CBW.dDataLength) {
            if ((USBD_MSC_CBW.bmFlags & 0x80) != 0) {
                USBD_MSC_SetStallEP(usbd_msc_ep_bulkin | 0x80);
            } else {
                if (USBD_MSC_CSW.dDataResidue != BulkLen) {
                    // Only stall if this isn't the last transfer
                    USBD_MSC_SetStallEP(usbd_msc_ep_bulkout);
                }
            }
        }

        USBD_MSC_CSW.bStatus = CSW_CMD_FAILED;
        USBD_MSC_SetCSW();
        return (__FALSE);
    }

    return (__TRUE);
}


/*
 *  USB Device MSC Memory Read Callback
 *   Called automatically on USB Device Memory Read Event
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_MemoryRead(void)
{
    U32 n, m;

    if (Block >= USBD_MSC_BlockCount) {
        n = 0;
        USBD_MSC_SetStallEP(usbd_msc_ep_bulkin | 0x80);
        USBD_MSC_CSW.bStatus = CSW_CMD_PASSED;
        USBD_MSC_SetCSW();
    } else {
        if (Length > usbd_msc_maxpacketsize[USBD_HighSpeed]) {
            n = usbd_msc_maxpacketsize[USBD_HighSpeed];
        } else {
            n = Length;
        }
    }

    if (!USBD_MSC_CheckMedia()) {
        n = 0;
    }

    if ((Offset == 0) && (n != 0)) {
        m = (Length + (USBD_MSC_BlockSize - 1)) / USBD_MSC_BlockSize;

        if (m > USBD_MSC_BlockGroup) {
            m = USBD_MSC_BlockGroup;
        }

        usbd_msc_read_sect(Block, USBD_MSC_BlockBuf, m);
    }

    if (n) {
        USBD_WriteEP(usbd_msc_ep_bulkin | 0x80, &USBD_MSC_BlockBuf[Offset], n);
        Offset += n;
        Length -= n;
    }

    if (Offset == USBD_MSC_BlockGroup * USBD_MSC_BlockSize) {
        Offset = 0;
        Block += USBD_MSC_BlockGroup;
    }

    USBD_MSC_CSW.dDataResidue -= n;

    if (!n) {
        return;
    }

    if (Length == 0) {
        BulkStage = MSC_BS_DATA_IN_LAST;
        USBD_MSC_CSW.bStatus = CSW_CMD_PASSED;
    }
}


/*
 *  USB Device MSC Memory Write Callback
 *   Called automatically on USB Device Memory Write Event
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_MemoryWrite(void)
{
    U32 n;

    if (Block >= USBD_MSC_BlockCount) {
        BulkLen = 0;
        USBD_MSC_SetStallEP(usbd_msc_ep_bulkout);
        USBD_MSC_CSW.bStatus = CSW_CMD_PASSED;
        USBD_MSC_SetCSW();
    }

    if (!USBD_MSC_CheckMedia()) {
        BulkLen = 0;
    }

    if (Offset + BulkLen > USBD_MSC_BlockSize) {
        // This write would have overflowed USBD_MSC_BlockBuf
        util_assert(0);
        return;
    }

    for (n = 0; n < BulkLen; n++) {
        USBD_MSC_BlockBuf[Offset + n] = USBD_MSC_BulkBuf[n];
    }

    Offset += BulkLen;
    Length -= BulkLen;

    if (BulkLen) {
        if ((Length == 0) && (Offset != 0)) {
            n = (Offset + (USBD_MSC_BlockSize - 1)) / USBD_MSC_BlockSize;

            if (n > USBD_MSC_BlockGroup) {
                n = USBD_MSC_BlockGroup;
            }

            usbd_msc_write_sect(Block, USBD_MSC_BlockBuf, n);
            Offset = 0;
            Block += n;
        } else if (Offset == USBD_MSC_BlockGroup * USBD_MSC_BlockSize) {
            usbd_msc_write_sect(Block, USBD_MSC_BlockBuf, USBD_MSC_BlockGroup);
            Offset = 0;
            Block += USBD_MSC_BlockGroup;
        }
    }

    USBD_MSC_CSW.dDataResidue -= BulkLen;

    if (!BulkLen) {
        return;
    }

    if ((Length == 0) || (BulkStage == MSC_BS_CSW)) {
        USBD_MSC_CSW.bStatus = CSW_CMD_PASSED;
        USBD_MSC_SetCSW();
    }
}


/*
 *  USB Device MSC Memory Verify Callback
 *   Called automatically on USB Device Memory Verify Event
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_MemoryVerify(void)
{
    U32 n;

    if (Block >= USBD_MSC_BlockCount) {
        BulkLen = 0;
        USBD_MSC_SetStallEP(usbd_msc_ep_bulkout);
        USBD_MSC_CSW.bStatus = CSW_CMD_PASSED;
        USBD_MSC_SetCSW();
    }

    if (!USBD_MSC_CheckMedia()) {
        BulkLen = 0;
    }

    if (BulkLen) {
        if ((Offset == 0) && (BulkLen != 0)) {
            n = (Length + (USBD_MSC_BlockSize - 1)) / USBD_MSC_BlockSize;

            if (n > USBD_MSC_BlockGroup) {
                n = USBD_MSC_BlockGroup;
            }

            usbd_msc_read_sect(Block, USBD_MSC_BlockBuf, n);
        }

        for (n = 0; n < BulkLen; n++) {
            if (USBD_MSC_BlockBuf[Offset + n] != USBD_MSC_BulkBuf[n]) {
                MemOK = __FALSE;
                break;
            }
        }
    }

    Offset += BulkLen;
    Length -= BulkLen;

    if (Offset == USBD_MSC_BlockGroup * USBD_MSC_BlockSize) {
        Offset = 0;
        Block += USBD_MSC_BlockGroup;
    }

    USBD_MSC_CSW.dDataResidue -= BulkLen;

    if (!BulkLen) {
        return;
    }

    if ((Length == 0) || (BulkStage == MSC_BS_CSW)) {
        USBD_MSC_CSW.bStatus = (MemOK) ? CSW_CMD_PASSED : CSW_CMD_FAILED;
        USBD_MSC_SetCSW();
    }
}


/*
 *  USB Device MSC SCSI Read/Write Setup Callback
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USBD_MSC_RWSetup(void)
{
    U32 n;
    /* Logical Block Address of First Block */
    n = (USBD_MSC_CBW.CB[2] << 24) |
        (USBD_MSC_CBW.CB[3] << 16) |
        (USBD_MSC_CBW.CB[4] <<  8) |
        (USBD_MSC_CBW.CB[5] <<  0);
    Block  = n;
    Offset = 0;

    /* Number of Blocks to transfer */
    switch (USBD_MSC_CBW.CB[0]) {
        case SCSI_WRITE10:
        case SCSI_VERIFY10:
            if (!USBD_MSC_CheckMedia()) {
                return (__FALSE);
            }

        case SCSI_READ10:
            n = (USBD_MSC_CBW.CB[7] <<  8) |
                (USBD_MSC_CBW.CB[8] <<  0);
            break;

        case SCSI_WRITE12:
            if (!USBD_MSC_CheckMedia()) {
                return (__FALSE);
            }

        case SCSI_READ12:
            n = (USBD_MSC_CBW.CB[6] << 24) |
                (USBD_MSC_CBW.CB[7] << 16) |
                (USBD_MSC_CBW.CB[8] <<  8) |
                (USBD_MSC_CBW.CB[9] <<  0);
            break;
    }

    Length = n * USBD_MSC_BlockSize;

    if (USBD_MSC_CBW.dDataLength == 0) {     /* host requests no data */
        USBD_MSC_CSW.bStatus = CSW_CMD_FAILED;
        USBD_MSC_SetCSW();
        return (__FALSE);
    }

    if (USBD_MSC_CBW.dDataLength != Length) {
        if ((USBD_MSC_CBW.bmFlags & 0x80) != 0) {  /* stall appropriate EP */
            USBD_MSC_SetStallEP(usbd_msc_ep_bulkin | 0x80);
        } else {
            USBD_MSC_SetStallEP(usbd_msc_ep_bulkout);
        }

        USBD_MSC_CSW.bStatus = CSW_CMD_FAILED;
        USBD_MSC_SetCSW();
        return (__FALSE);
    }

    return (__TRUE);
}


/*
 *  USB Device Check Data IN Format
 *    Parameters:      None
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL USBD_MSC_DataInFormat(void)
{
    if (USBD_MSC_CBW.dDataLength == 0) {
        USBD_MSC_CSW.bStatus = CSW_PHASE_ERROR;
        USBD_MSC_SetCSW();
        return (__FALSE);
    }

    if ((USBD_MSC_CBW.bmFlags & 0x80) == 0) {
        USBD_MSC_SetStallEP(usbd_msc_ep_bulkout);
        USBD_MSC_CSW.bStatus = CSW_PHASE_ERROR;
        USBD_MSC_SetCSW();
        return (__FALSE);
    }

    return (__TRUE);
}


/*
 *  USB Device Perform Data IN Transfer
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_DataInTransfer(void)
{
    if (BulkLen >= USBD_MSC_CBW.dDataLength) {
        BulkLen = USBD_MSC_CBW.dDataLength;
        BulkStage = MSC_BS_DATA_IN_LAST;
    } else {
        BulkStage = MSC_BS_DATA_IN_LAST_STALL; /* short or zero packet */
    }

    USBD_WriteEP(usbd_msc_ep_bulkin | 0x80, USBD_MSC_BulkBuf, BulkLen);
    USBD_MSC_CSW.dDataResidue -= BulkLen;
    USBD_MSC_CSW.bStatus = CSW_CMD_PASSED;
}


/*
 *  USB Device MSC SCSI Test Unit Ready Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_TestUnitReady(void)
{
    if (USBD_MSC_CBW.dDataLength != 0) {
        if ((USBD_MSC_CBW.bmFlags & 0x80) != 0) {
            USBD_MSC_SetStallEP(usbd_msc_ep_bulkin | 0x80);
        } else {
            USBD_MSC_SetStallEP(usbd_msc_ep_bulkout);
        }

        USBD_MSC_CSW.bStatus = CSW_CMD_FAILED;
        USBD_MSC_SetCSW();
        return;
    }

    if (!USBD_MSC_CheckMedia()) {
        return;
    }

    USBD_MSC_CSW.bStatus = CSW_CMD_PASSED;
    USBD_MSC_SetCSW();
}


/*
 *  USB Device MSC SCSI Request Sense Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_RequestSense(void)
{
    if (!USBD_MSC_DataInFormat()) {
        return;
    }

    USBD_MSC_BulkBuf[ 0] = 0x70;             /* Response Code */
    USBD_MSC_BulkBuf[ 1] = 0x00;

    if ((USBD_MSC_MediaReadyEx ^ USBD_MSC_MediaReady) & USBD_MSC_MediaReady) {  /* If media state changed to ready */
        USBD_MSC_BulkBuf[ 2] = 0x06;           /* UNIT ATTENTION */
        USBD_MSC_BulkBuf[12] = 0x28;           /* Additional Sense Code: Not ready to ready transition */
        USBD_MSC_BulkBuf[13] = 0x00;           /* Additional Sense Code Qualifier */
        USBD_MSC_MediaReadyEx = USBD_MSC_MediaReady;
    } else if (!USBD_MSC_MediaReady) {
        USBD_MSC_BulkBuf[ 2] = 0x02;           /* NOT READY */
        USBD_MSC_BulkBuf[12] = 0x3A;           /* Additional Sense Code: Medium not present */
        USBD_MSC_BulkBuf[13] = 0x00;           /* Additional Sense Code Qualifier */
    } else {
        if (USBD_MSC_CSW.bStatus == CSW_CMD_PASSED) {
            USBD_MSC_BulkBuf[ 2] = 0x00;         /* NO SENSE */
            USBD_MSC_BulkBuf[12] = 0x00;         /* Additional Sense Code: No additional code */
            USBD_MSC_BulkBuf[13] = 0x00;         /* Additional Sense Code Qualifier */
        } else {
            USBD_MSC_BulkBuf[ 2] = 0x05;         /* ILLEGAL REQUEST */
            USBD_MSC_BulkBuf[12] = 0x20;         /* Additional Sense Code: Invalid command */
            USBD_MSC_BulkBuf[13] = 0x00;         /* Additional Sense Code Qualifier */
        }
    }

    USBD_MSC_BulkBuf[ 3] = 0x00;
    USBD_MSC_BulkBuf[ 4] = 0x00;
    USBD_MSC_BulkBuf[ 5] = 0x00;
    USBD_MSC_BulkBuf[ 6] = 0x00;
    USBD_MSC_BulkBuf[ 7] = 0x0A;             /* Additional Length */
    USBD_MSC_BulkBuf[ 8] = 0x00;
    USBD_MSC_BulkBuf[ 9] = 0x00;
    USBD_MSC_BulkBuf[10] = 0x00;
    USBD_MSC_BulkBuf[11] = 0x00;
    USBD_MSC_BulkBuf[14] = 0x00;
    USBD_MSC_BulkBuf[15] = 0x00;
    USBD_MSC_BulkBuf[16] = 0x00;
    USBD_MSC_BulkBuf[17] = 0x00;
    BulkLen = 18;
    USBD_MSC_DataInTransfer();
}


/*
 *  USB Device MSC SCSI Inquiry Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_Inquiry(void)
{
    U8  i;
    U8 *ptr_str;

    if (!USBD_MSC_DataInFormat()) {
        return;
    }

    USBD_MSC_BulkBuf[ 0] = 0x00;             /* Direct Access Device */
    USBD_MSC_BulkBuf[ 1] = 0x80;             /* RMB = 1: Removable Medium */
    USBD_MSC_BulkBuf[ 2] = 0x02;             /* Version: ANSI X3.131: 1994 */
    USBD_MSC_BulkBuf[ 3] = 0x02;
    USBD_MSC_BulkBuf[ 4] = 36 - 4;           /* Additional Length */
    USBD_MSC_BulkBuf[ 5] = 0x00;             /* SCCS = 0: No Storage Controller Component */
    USBD_MSC_BulkBuf[ 6] = 0x00;
    USBD_MSC_BulkBuf[ 7] = 0x00;
    ptr_str = (U8 *)usbd_msc_inquiry_data;

    for (i = 8; i < 36; i++) {               /* Product Information    + */
        if (*ptr_str) {                        /* Product Revision Level   */
            USBD_MSC_BulkBuf[i] = *ptr_str++;
        } else {
            USBD_MSC_BulkBuf[i] = ' ';
        }
    }

    BulkLen = 36;
    USBD_MSC_DataInTransfer();
}


/*
 *  USB Device MSC SCSI Start Stop Unit Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_StartStopUnit(void)
{
    if (!USBD_MSC_CBW.CB[3]) {               /* If power condition modifier is 0 */
        USBD_MSC_MediaReady  = USBD_MSC_CBW.CB[4] & 0x01;   /* Media ready = START bit value */
        usbd_msc_start_stop(USBD_MSC_MediaReady);
        USBD_MSC_CSW.bStatus = CSW_CMD_PASSED; /* Start Stop Unit -> pass */
        USBD_MSC_SetCSW();
        return;
    }

    USBD_MSC_CSW.bStatus = CSW_CMD_FAILED;   /* Start Stop Unit -> fail */
    USBD_MSC_SetCSW();
}


/*
 *  USB Device MSC SCSI Media Removal Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_MediaRemoval(void)
{
    if (USBD_MSC_CBW.CB[4] & 1) {            /* If prevent */
        USBD_MSC_CSW.bStatus = CSW_CMD_FAILED;    /* Prevent media removal -> fail */
    } else {                                 /* If allow */
        USBD_MSC_CSW.bStatus = CSW_CMD_PASSED;    /* Allow media removal -> pass */
    }

    USBD_MSC_SetCSW();
}


/*
 *  USB Device MSC SCSI Mode Sense (6-Byte) Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_ModeSense6(void)
{
    if (!USBD_MSC_DataInFormat()) {
        return;
    }

    if (!USBD_MSC_CheckMedia()) {
        return;
    }

    USBD_MSC_BulkBuf[ 0] = 0x03;
    USBD_MSC_BulkBuf[ 1] = 0x00;
    USBD_MSC_BulkBuf[ 2] = (USBD_MSC_ReadOnly << 7);
    USBD_MSC_BulkBuf[ 3] = 0x00;
    BulkLen = 4;

    /* Win host requests maximum number of bytes but as all we have is 4 bytes we have
       to tell host back that it is all we have, that's why we correct residue */
    if (USBD_MSC_CSW.dDataResidue > BulkLen) {
        USBD_MSC_CBW.dDataLength  = BulkLen;
        USBD_MSC_CSW.dDataResidue = BulkLen;
    }

    USBD_MSC_DataInTransfer();
}


/*
 *  USB Device MSC SCSI Mode Sense (10-Byte) Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_ModeSense10(void)
{
    if (!USBD_MSC_DataInFormat()) {
        return;
    }

    if (!USBD_MSC_CheckMedia()) {
        return;
    }

    USBD_MSC_BulkBuf[ 0] = 0x00;
    USBD_MSC_BulkBuf[ 1] = 0x06;
    USBD_MSC_BulkBuf[ 2] = 0x00;
    USBD_MSC_BulkBuf[ 3] = (USBD_MSC_ReadOnly << 7);
    USBD_MSC_BulkBuf[ 4] = 0x00;
    USBD_MSC_BulkBuf[ 5] = 0x00;
    USBD_MSC_BulkBuf[ 6] = 0x00;
    USBD_MSC_BulkBuf[ 7] = 0x00;
    BulkLen = 8;

    /* Win host requests maximum number of bytes but as all we have is 8 bytes we have
       to tell host back that it is all we have, that's why we correct residue */
    if (USBD_MSC_CSW.dDataResidue > BulkLen) {
        USBD_MSC_CBW.dDataLength  = BulkLen;
        USBD_MSC_CSW.dDataResidue = BulkLen;
    }

    USBD_MSC_DataInTransfer();
}


/*
 *  USB Device MSC SCSI Read Capacity Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_ReadCapacity(void)
{
    if (!USBD_MSC_DataInFormat()) {
        return;
    }

    if (!USBD_MSC_CheckMedia()) {
        return;
    }

    /* Last Logical Block */
    USBD_MSC_BulkBuf[ 0] = ((USBD_MSC_BlockCount - 1) >> 24) & 0xFF;
    USBD_MSC_BulkBuf[ 1] = ((USBD_MSC_BlockCount - 1) >> 16) & 0xFF;
    USBD_MSC_BulkBuf[ 2] = ((USBD_MSC_BlockCount - 1) >>  8) & 0xFF;
    USBD_MSC_BulkBuf[ 3] = ((USBD_MSC_BlockCount - 1) >>  0) & 0xFF;
    /* Block Length */
    USBD_MSC_BulkBuf[ 4] = (USBD_MSC_BlockSize        >> 24) & 0xFF;
    USBD_MSC_BulkBuf[ 5] = (USBD_MSC_BlockSize        >> 16) & 0xFF;
    USBD_MSC_BulkBuf[ 6] = (USBD_MSC_BlockSize        >>  8) & 0xFF;
    USBD_MSC_BulkBuf[ 7] = (USBD_MSC_BlockSize        >>  0) & 0xFF;
    BulkLen = 8;
    USBD_MSC_DataInTransfer();
}


/*
 *  USB Device MSC SCSI Read Format Capacity Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_ReadFormatCapacity(void)
{
    if (!USBD_MSC_DataInFormat()) {
        return;
    }

    if (!USBD_MSC_CheckMedia()) {
        return;
    }

    USBD_MSC_BulkBuf[ 0] = 0x00;
    USBD_MSC_BulkBuf[ 1] = 0x00;
    USBD_MSC_BulkBuf[ 2] = 0x00;
    USBD_MSC_BulkBuf[ 3] = 0x08;                      /* Capacity List Length */
    /* Block Count */
    USBD_MSC_BulkBuf[ 4] = (USBD_MSC_BlockCount >> 24) & 0xFF;
    USBD_MSC_BulkBuf[ 5] = (USBD_MSC_BlockCount >> 16) & 0xFF;
    USBD_MSC_BulkBuf[ 6] = (USBD_MSC_BlockCount >>  8) & 0xFF;
    USBD_MSC_BulkBuf[ 7] = (USBD_MSC_BlockCount >>  0) & 0xFF;
    /* Block Length */
    USBD_MSC_BulkBuf[ 8] = 0x02;                      /* Descriptor Code: Formatted Media */
    USBD_MSC_BulkBuf[ 9] = (USBD_MSC_BlockSize  >> 16) & 0xFF;
    USBD_MSC_BulkBuf[10] = (USBD_MSC_BlockSize  >>  8) & 0xFF;
    USBD_MSC_BulkBuf[11] = (USBD_MSC_BlockSize  >>  0) & 0xFF;
    BulkLen = 12;

    /* Win host requests maximum number of bytes but as all we have is 12 bytes we have
       to tell host back that it is all we have, that's why we correct residue */
    if (USBD_MSC_CSW.dDataResidue > BulkLen) {
        USBD_MSC_CBW.dDataLength  = BulkLen;
        USBD_MSC_CSW.dDataResidue = BulkLen;
    }

    USBD_MSC_DataInTransfer();
}


/*
 *  USB Device MSC SCSI Synchronize Cache (10/16-Byte) Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_SynchronizeCache(void)
{
    /* Synchronize check always passes as we always write data dirrectly
       so cache is always synchronized                                          */
    USBD_MSC_CSW.bStatus = CSW_CMD_PASSED;
    USBD_MSC_SetCSW();
}


/*
 *  USB Device MSC ATA Pass-Through Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_ATAPassThrough(void)
{
    if (!USBD_MSC_DataInFormat()) {
        return;
    }

    USBD_MSC_BulkBuf[ 0] = 0x02;
    USBD_MSC_BulkBuf[ 1] = 0x02;
    BulkLen = 2;
    BulkStage = MSC_BS_DATA_IN_LAST;

    /* Win host requests maximum number of bytes but as all we have is 2 bytes we have
       to tell host back that it is all we have, that's why we correct residue */
    if (USBD_MSC_CSW.dDataResidue > BulkLen) {
        USBD_MSC_CBW.dDataLength  = BulkLen;
        USBD_MSC_CSW.dDataResidue = BulkLen;
    }

    USBD_WriteEP(usbd_msc_ep_bulkin | 0x80, USBD_MSC_BulkBuf, BulkLen);
    USBD_MSC_CSW.dDataResidue -= BulkLen;
    USBD_MSC_CSW.bStatus = CSW_CMD_FAILED;
}

/*
 *  USB Device MSC Service Action (16-Byte) Callback
 *    Parameters:      None
 *    Return Value:    None
 */


void USBD_MSC_ServiceActionIn16(void)
{
    if (!USBD_MSC_DataInFormat()) {
        return;
    }

    USBD_MSC_BulkBuf[ 0] = 0x20;
    USBD_MSC_BulkBuf[ 1] = 0x00;
    USBD_MSC_BulkBuf[31] = 0x00;
    BulkLen = 32;
    BulkStage = MSC_BS_DATA_IN_LAST;
    USBD_WriteEP(usbd_msc_ep_bulkin | 0x80, USBD_MSC_BulkBuf, BulkLen);
    USBD_MSC_CSW.dDataResidue -= BulkLen;
    USBD_MSC_CSW.bStatus = CSW_CMD_FAILED;
}


/*
 *  USB Device MSC Get Command Block Wrapper Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_GetCBW(void)
{
    U32 n;
    U32 copy_size;
    copy_size = MIN(BulkLen, sizeof(USBD_MSC_CBW));

    for (n = 0; n < copy_size; n++) {
        *((U8 *)&USBD_MSC_CBW + n) = USBD_MSC_BulkBuf[n];
    }

    if ((BulkLen == sizeof(USBD_MSC_CBW)) && (USBD_MSC_CBW.dSignature == MSC_CBW_Signature)) {
        /* Valid USBD_MSC_CBW */
        USBD_MSC_CSW.dTag = USBD_MSC_CBW.dTag;
        USBD_MSC_CSW.dDataResidue = USBD_MSC_CBW.dDataLength;

        if ((USBD_MSC_CBW.bLUN      >  0) ||
                (USBD_MSC_CBW.bCBLength <  1) ||
                (USBD_MSC_CBW.bCBLength > 16)) {
fail:
            USBD_MSC_CSW.bStatus = CSW_CMD_FAILED;
            USBD_MSC_SetCSW();
        } else {
            switch (USBD_MSC_CBW.CB[0]) {
                case SCSI_TEST_UNIT_READY:
                    USBD_MSC_TestUnitReady();
                    break;

                case SCSI_REQUEST_SENSE:
                    USBD_MSC_RequestSense();
                    break;

                case SCSI_FORMAT_UNIT:
                    goto fail;

                case SCSI_INQUIRY:
                    USBD_MSC_Inquiry();
                    break;

                case SCSI_START_STOP_UNIT:
                    USBD_MSC_StartStopUnit();
                    break;

                case SCSI_MEDIA_REMOVAL:
                    USBD_MSC_MediaRemoval();
                    break;

                case SCSI_MODE_SELECT6:
                    goto fail;

                case SCSI_MODE_SENSE6:
                    USBD_MSC_ModeSense6();
                    break;

                case SCSI_MODE_SELECT10:
                    goto fail;

                case SCSI_MODE_SENSE10:
                    USBD_MSC_ModeSense10();
                    break;

                case SCSI_READ_FORMAT_CAPACITIES:
                    USBD_MSC_ReadFormatCapacity();
                    break;

                case SCSI_READ_CAPACITY:
                    USBD_MSC_ReadCapacity();
                    break;

                case SCSI_ATA_COMMAND_PASS_THROUGH12:
                    USBD_MSC_ATAPassThrough();
                    break;

                case SCSI_SERVICE_ACTION_IN16:
                    USBD_MSC_ServiceActionIn16();
                    break;

                case SCSI_READ10:
                case SCSI_READ12:
                    if (USBD_MSC_RWSetup()) {
                        if ((USBD_MSC_CBW.bmFlags & 0x80) != 0) {
                            BulkStage = MSC_BS_DATA_IN;
                            USBD_MSC_MemoryRead();
                        } else {                       /* direction mismatch */
                            USBD_MSC_SetStallEP(usbd_msc_ep_bulkout);
                            USBD_MSC_CSW.bStatus = CSW_PHASE_ERROR;
                            USBD_MSC_SetCSW();
                        }
                    }

                    break;

                case SCSI_WRITE10:
                case SCSI_WRITE12:
                    if (USBD_MSC_RWSetup()) {
                        if ((USBD_MSC_CBW.bmFlags & 0x80) == 0) {
                            BulkStage = MSC_BS_DATA_OUT;
                        } else {                       /* direction mismatch */
                            USBD_MSC_SetStallEP(usbd_msc_ep_bulkin | 0x80);
                            USBD_MSC_CSW.bStatus = CSW_PHASE_ERROR;
                            USBD_MSC_SetCSW();
                        }
                    }

                    break;

                case SCSI_VERIFY10:
                    if ((USBD_MSC_CBW.CB[1] & 0x02) == 0) {
                        // BYTCHK = 0 -> CRC Check (not implemented)
                        USBD_MSC_CSW.bStatus = CSW_CMD_PASSED;
                        USBD_MSC_SetCSW();
                        break;
                    }

                    if (USBD_MSC_RWSetup()) {
                        if ((USBD_MSC_CBW.bmFlags & 0x80) == 0) {
                            BulkStage = MSC_BS_DATA_OUT;
                            MemOK = __TRUE;
                        } else {
                            USBD_MSC_SetStallEP(usbd_msc_ep_bulkin | 0x80);
                            USBD_MSC_CSW.bStatus = CSW_PHASE_ERROR;
                            USBD_MSC_SetCSW();
                        }
                    }

                    break;

                case SCSI_SYNC_CACHE10:
                case SCSI_SYNC_CACHE16:
                    USBD_MSC_SynchronizeCache();
                    break;

                case SCSI_REPORT_ID_INFO:
                    USBD_MSC_SetStallEP(usbd_msc_ep_bulkin | 0x80);
                    goto fail;

                default:
                    goto fail;
            }
        }
    } else {
        /* Invalid USBD_MSC_CBW */
        USBD_MSC_SetStallEP(usbd_msc_ep_bulkin | 0x80);
        /* set EP to stay stalled */
        USBD_EndPointStall |= ((1 << 16) << (usbd_msc_ep_bulkin & 0x0F));
        USBD_MSC_SetStallEP(usbd_msc_ep_bulkout);
        /* set EP to stay stalled */
        USBD_EndPointStall |=  1 << usbd_msc_ep_bulkout;
        BulkStage = MSC_BS_ERROR;
    }
}


/*
 *  USB Device MSC Set Command Status Wrapper Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_SetCSW(void)
{
    USBD_MSC_CSW.dSignature = MSC_CSW_Signature;
    USBD_WriteEP(usbd_msc_ep_bulkin | 0x80, (U8 *)&USBD_MSC_CSW, sizeof(USBD_MSC_CSW));
    BulkStage = MSC_BS_CSW;
}


/*
 *  USB Device MSC Bulk In Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_BulkIn(void)
{
    switch (BulkStage) {
        case MSC_BS_DATA_IN:
            switch (USBD_MSC_CBW.CB[0]) {
                case SCSI_READ10:
                case SCSI_READ12:
                    USBD_MSC_MemoryRead();
                    break;
            }

            break;

        case MSC_BS_DATA_IN_LAST:
            USBD_MSC_SetCSW();
            break;

        case MSC_BS_DATA_IN_LAST_STALL:
            USBD_MSC_SetStallEP(usbd_msc_ep_bulkin | 0x80);
            USBD_MSC_SetCSW();
            break;

        case MSC_BS_CSW:
            BulkStage = MSC_BS_CBW;
            break;

        default:
            break;
    }
}


/*
 *  USB Device MSC Bulk Out Callback
 *    Parameters:      None
 *    Return Value:    None
 */

void USBD_MSC_BulkOut(void)
{
    switch (BulkStage) {
        case MSC_BS_CBW:
            USBD_MSC_GetCBW();
            break;

        case MSC_BS_DATA_OUT:
            switch (USBD_MSC_CBW.CB[0]) {
                case SCSI_WRITE10:
                case SCSI_WRITE12:
                    USBD_MSC_MemoryWrite();
                    break;

                case SCSI_VERIFY10:
                    USBD_MSC_MemoryVerify();
                    break;
            }
            break;

        case MSC_BS_CSW:
            // Previous transfer must be complete
            // before the next transfer begins.
            //
            // If bulk out is stalled then just
            // drop this packet and don't assert.
            // This packet was left over from before
            // the transfer aborted with a stall.
            util_assert(USBD_EndPointHalt & (1 << usbd_msc_ep_bulkout));
            break;

        case MSC_BS_RESET:
            // If Bulk-Only Mass Storage Reset command was received on
            // Control Endpoint ignore next Bulk OUT transfer if it was not
            // a CBW (as it means it was a unprocessed leftover from
            // transfer before reset happened)
            BulkStage = MSC_BS_CBW;
            if (BulkLen == sizeof(USBD_MSC_CBW)) {
                // If it is a CBW size packet go process it as CBW
                USBD_MSC_GetCBW();
            }
            break;

        default:
            USBD_MSC_SetStallEP(usbd_msc_ep_bulkout);
            USBD_MSC_CSW.bStatus = CSW_PHASE_ERROR;
            USBD_MSC_SetCSW();
            break;
    }
}

/** \brief  Handle Reset Events

    The function handles Reset events.
 */

void USBD_MSC_Reset_Event(void)
{
    USBD_MSC_Reset();
}

/*
 *  USB Device MSC Bulk In Endpoint Event Callback
 *    Parameters:      event: not used (just for compatibility)
 *    Return Value:    None
 */

void USBD_MSC_EP_BULKIN_Event(U32 event)
{
    USBD_MSC_BulkIn();
}


/*
 *  USB Device MSC Bulk Out Endpoint Event Callback
 *    Parameters:      event: not used (just for compatibility)
 *    Return Value:    None
 */

void USBD_MSC_EP_BULKOUT_Event(U32 event)
{
    BulkLen = USBD_ReadEP(usbd_msc_ep_bulkout, USBD_MSC_BulkBuf, USBD_MSC_BulkBufSize);
    USBD_MSC_BulkOut();
}


/*
 *  USB Device MSC Bulk In/Out Endpoint Event Callback
 *    Parameters:      event: USB Device Event
 *                       USBD_EVT_OUT: Output Event
 *                       USBD_EVT_IN:  Input Event
 *    Return Value:    None
 */

void USBD_MSC_EP_BULK_Event(U32 event)
{
    if (event & USBD_EVT_OUT) {
        USBD_MSC_EP_BULKOUT_Event(0);
    }

    if (event & USBD_EVT_IN) {
        USBD_MSC_EP_BULKIN_Event(0);
    }
}


#ifdef __RTX                            /* RTX tasks for handling events */

/*
 *  USB Device MSC Bulk In Endpoint Event Handler Task
 *    Parameters:      None
 *    Return Value:    None
 */

__task void USBD_RTX_MSC_EP_BULKIN_Event(void)
{
    for (;;) {
        usbd_os_evt_wait_or(0xFFFF, 0xFFFF);

        if (usbd_os_evt_get() & USBD_EVT_IN) {
            USBD_MSC_EP_BULKIN_Event(0);
        }
    }
}


/*
 *  USB Device MSC Bulk Out Endpoint Event Handler Task
 *    Parameters:      None
 *    Return Value:    None
 */

__task void USBD_RTX_MSC_EP_BULKOUT_Event(void)
{
    for (;;) {
        usbd_os_evt_wait_or(0xFFFF, 0xFFFF);

        if (usbd_os_evt_get() & USBD_EVT_OUT) {
            USBD_MSC_EP_BULKOUT_Event(0);
        }
    }
}


/*
 *  USB Device MSC Bulk In/Out Endpoint Event Handler Task
 *    Parameters:      None
 *    Return Value:    None
 */

__task void USBD_RTX_MSC_EP_BULK_Event(void)
{
    for (;;) {
        usbd_os_evt_wait_or(0xFFFF, 0xFFFF);
        USBD_MSC_EP_BULK_Event(usbd_os_evt_get());
    }
}
#endif
