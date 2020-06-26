/**********************************************************************
 *
 * Filename:    crc.h
 *
 * Description: A header file describing the various CRC standards.
 *
 * Notes:
 *
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

/**
 * @file    crc.h
 * @brief   CRC functions
 */

#ifndef _crc_h
#define _crc_h

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t crc16(const void *data, int nBytes);
uint32_t crc32(const void *data, int nBytes);
uint32_t crc32_continue(uint32_t prev_crc, const void *data, int nBytes);

#ifdef __cplusplus
}
#endif

#endif
