/**
 * @file    FlashOS.h
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

#ifndef FLASHOS_H
#define FLASHOS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VERS        1       // Interface Version 1.01
#define NAME_MAX    128     // Max size of the routine name
#define PAGE_MAX    65536   // Max Page Size for Programming
#define SECTOR_NUM  512     // Max Number of Sector Items
#define SECTOR_END  0xFFFFFFFF, 0xFFFFFFFF

// FlashDevice.devType interface mechanism
#define UNKNOWN     0
#define ONCHIP      1
#define EXT8BIT     2
#define EXT16BIT    3
#define EXT32BIT    4
#define EXTSPI      5

/**
    @struct FlashSector
    @brief  A structure to describe the size and start address of a flash sector
 */
struct FlashSector {
    uint32_t szSector;      /*!< Sector Size in Bytes */
    uint32_t adrSector;     /*!< Address of Sector */
};

/**
    @struct FlashDevice
    @brief  A structure to describe particulars of a flash memory sub-system
        and requirements of the driver
 */
struct FlashDevice {
    uint16_t vers;          /*!< Version Number and Architecture */
    char devName[NAME_MAX]; /*!< Device Name and Description */
    uint16_t devType;       /*!< Device Type: ONCHIP, EXT8BIT, EXT16BIT, ... */
    uint32_t devAdr;        /*!< Default Device Start Address */
    uint32_t szDev;         /*!< Total Size of Device */
    uint32_t szPage;        /*!< Programming Page Size */
    uint32_t res;           /*!< Reserved for future Extension */
    uint8_t  valEmpty;      /*!< Content of Erased Memory */
    uint32_t toProg;        /*!< Time Out of Program Page Function */
    uint32_t toErase;       /*!< Time Out of Erase Sector Function */
    struct FlashSector sectors[SECTOR_NUM]; /*!< Entries to describe flash memory layout */
};

#ifdef __cplusplus
}
#endif

#endif
