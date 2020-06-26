/**
 * @file    target_board.h
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2018-2019, ARM Limited, All Rights Reserved
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

#ifndef TARGET_BOARD_H
#define TARGET_BOARD_H

#include <stdint.h>
#include "target_config.h"
#include "target_family.h"
#include "virtual_fs.h"

//! @brief Current board info version.
//!
//! - Version 1: Initial version.
enum _board_info_version {
    kBoardInfoVersion = 1, //!< The current board info version.
};

//! @brief Flags for board_info
enum _board_info_flags {
	kEnablePageErase = (1 << 0),            /*!< Enable page programming and sector erase for drag and drop */
    kEnableUnderResetConnect = (1 << 1),    /*!< Enable under reset connection when enabling debug mode */
};

/*!
 * @brief Board customization info.
 *
 * Each board must have a unique 4-character Board ID. For Mbed OS targets, the Board ID is the same
 * as the Mbed Platform ID. These IDs are nominally allocated by Arm in order to guarantee there are
 * no conflicts between boards. Please see the DAPLink documentation for more.
 *
 * The family_id member tells DAPLink which device family the on-board target belongs to. This then
 * determines certain behaviours, such as how to reset the target. Family IDs are defined in the
 * #family_id_t enumeration.
 *
 * The board initialization function pointers allow the board to override the routines defined
 * by the device family.
 */
typedef struct __attribute__((__packed__)) board_info {
    uint16_t info_version;              /*!< Version number of the board info */ 
    uint16_t family_id;                 /*!< Use to select or identify target family from defined target family or custom ones */
    char board_id[5];                   /*!< 4-char board ID plus null terminator */
    uint8_t _padding[3];
    uint32_t flags;                     /*!< Flags from #_board_info_flags */
    target_cfg_t *target_cfg;           /*!< Specific chip configuration for the target and enables MSD when non-NULL */

    //! @name MSD customization
    //@{
    vfs_filename_t daplink_url_name;    /*!< Customize the URL file name */
    vfs_filename_t daplink_drive_name;  /*!< Customize the MSD DAPLink drive name */
    char daplink_target_url[64];        /*!< Customize the target url in DETAILS.TXT */
    //@}

    //! @name Board initialization customization
    //@{
    void (*prerun_board_config)(void);                      /*!< Specific board debug/ID related initialization */
    void (*swd_set_target_reset)(uint8_t asserted);         /*!< Boards can customize how to send reset to the target with precedence over target family */
    uint8_t (*target_set_state)(target_state_t state);  /*!< Boards can customize target debug states with precedence over target family */
    uint32_t soft_reset_type;                               /*!< Boards can override software reset type to VECTRESET or SYSRESETREQ */
    //@}
} board_info_t;

//! @brief Information describing the board on which DAPLink is running.
extern const board_info_t g_board_info;

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Returns the 4-char ID of the board used by the running firmware.
//!
//! For firmware with no board, the board ID is "0000".
const char * get_board_id(void);

//! @brief Returns the family ID for the target associated with the board.
//!
//! The family ID will be 0 if there is no board.
uint16_t get_family_id(void);

//! @brief Whether the board has a valid flash algo.
uint8_t flash_algo_valid(void);

//! @brief Returns the MSD HTML help filename or a default.
static inline const char * get_daplink_url_name ( void ) { return ((g_board_info.daplink_url_name[0] != 0) ? g_board_info.daplink_url_name : "MBED    HTM"); }

//! @brief Returns the MSD volume name or a default.
static inline const char * get_daplink_drive_name ( void ) { return ((g_board_info.daplink_drive_name[0] != 0) ? g_board_info.daplink_drive_name : "DAPLINK    "); }

//! @brief Returns the target information URL or a default.
static inline const char * get_daplink_target_url ( void ) { return ((g_board_info.daplink_target_url[0] != 0) ? g_board_info.daplink_target_url : "https://mbed.org/device/?code=@U?version=@V?target_id=@T"); }

#ifdef __cplusplus
}
#endif

#endif
