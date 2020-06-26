/**
 * @file    target_family.h
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

#ifndef TARGET_FAMILY_H
#define TARGET_FAMILY_H

#include <stdint.h>
#include <string.h>

//! @brief Creates a family ID from a vendor ID and family index within that vendor.
#define VENDOR_TO_FAMILY(vendor, family) ((vendor) << 8 | (family))

//! @brief States into which the target can be placed.
//!
//! These enums are passed to target_set_state() and indicate the desired state into which
//! the target should be reset.
typedef enum _target_state {
    RESET_HOLD,              //!< Hold target in reset
    RESET_PROGRAM,           //!< Reset target and setup for flash programming
    RESET_RUN,               //!< Reset target and run normally
    NO_DEBUG,                //!< Disable debug on running target
    DEBUG,                   //!< Enable debug on running target
    HALT,                    //!< Halt the target without resetting it
    RUN,                     //!< Resume the target without resetting it
    POST_FLASH_RESET,        //!< Reset target after flash programming
    POWER_ON,                //!< Poweron the target
    SHUTDOWN,                //!< Poweroff the target
} target_state_t;

//! @brief Options for reset.
typedef enum _reset_type {
    kHardwareReset = 1,
    kSoftwareReset,
} reset_type_t;

//! @brief Unique IDs for vendors.
//!
//! The vendor IDs are the same as those used for the _DeviceVendorEnum_ defined for the PDSC file
//! format from CMSIS-Packs. See the [DeviceVendorEnum
//! documentation](https://arm-software.github.io/CMSIS_5/Pack/html/pdsc_family_pg.html#DeviceVendorEnum)
//! for the list of ID values.
enum _vendor_ids {
    kStub_VendorID = 0,
    kNXP_VendorID = 11,
    kTI_VendorID = 16,
    kNordic_VendorID = 54,
    kToshiba_VendorID = 92,
    kRenesas_VendorID = 117,
    kWiznet_VendorID = 122,
    kRealtek_VendorID = 124,
};

//! @brief Unique IDs for device families supported by DAPLink.
//!
//! The values of these enums are created with the VENDOR_TO_FAMILY() macro. Vendor IDs come from
//! the #_vendor_ids enumeration. The family index for each ID is simply an integer that is unique
//! within the family.
//!
//! There are several "stub" families defined with a stub vendor. These families are meant to be
//! used for devices that do not require any customized behaviour in order to be successfully
//! controlled by DAPLink. The individual stub families provide some options for what reset type
//! should be used, either hardware or software.
//!
//! To add a new family, first determine if you can simply use one of the stub families. For many
//! devices, the stub families are sufficient and using them reduces complexity.
//!
//! If you do need a new family ID, first check if the vendor is present in #_vendor_ids. If not,
//! add the vendor ID to that enum (see its documentation for the source of vendor ID values).
//! Then pick a unique family index by adding 1 to the highest existing family index within that
//! vendor. For a family with a new vendor, the family index should be 1.
typedef enum _family_id {
    kStub_HWReset_FamilyID = VENDOR_TO_FAMILY(kStub_VendorID, 1),
    kStub_SWVectReset_FamilyID = VENDOR_TO_FAMILY(kStub_VendorID, 2),
    kStub_SWSysReset_FamilyID = VENDOR_TO_FAMILY(kStub_VendorID, 3),
    kNXP_KinetisK_FamilyID = VENDOR_TO_FAMILY(kNXP_VendorID, 1),
    kNXP_KinetisL_FamilyID = VENDOR_TO_FAMILY(kNXP_VendorID, 2),
    kNXP_Mimxrt_FamilyID = VENDOR_TO_FAMILY(kNXP_VendorID, 3),
    kNXP_RapidIot_FamilyID = VENDOR_TO_FAMILY(kNXP_VendorID, 4),
    kNXP_KinetisK32W_FamilyID = VENDOR_TO_FAMILY(kNXP_VendorID, 5),
    kNordic_Nrf51_FamilyID = VENDOR_TO_FAMILY(kNordic_VendorID, 1),
    kNordic_Nrf52_FamilyID = VENDOR_TO_FAMILY(kNordic_VendorID, 2),
    kRealtek_Rtl8195am_FamilyID = VENDOR_TO_FAMILY(kRealtek_VendorID, 1),
    kTI_Cc3220sf_FamilyID = VENDOR_TO_FAMILY(kTI_VendorID, 1),
    kToshiba_Tz_FamilyID = VENDOR_TO_FAMILY(kToshiba_VendorID, 1),
    kWiznet_W7500_FamilyID = VENDOR_TO_FAMILY(kWiznet_VendorID, 1),
    kRenesas_FamilyID = VENDOR_TO_FAMILY(kRenesas_VendorID, 1),
} family_id_t;

//! @brief Defines all characteristics of a device family.
typedef struct target_family_descriptor {
    uint16_t family_id;                         /*!< Use to select or identify target family from defined target family or custom ones */
    reset_type_t default_reset_type;            /*!< Target family can select predefined reset from  kHardwareReset and kSoftwareReset */
    uint32_t soft_reset_type;                   /*!< Families can override software reset type to VECTRESET or SYSRESETREQ */
    void (*target_before_init_debug)(void);     /*!< Target dependant function before debug initialization */
    void (*prerun_target_config)(void);         /*!< Target specific initialization */
    uint8_t (*target_unlock_sequence)(void);    /*!< Unlock targets that can enter lock state */
    uint8_t (*security_bits_set)(uint32_t addr, uint8_t *data, uint32_t size);  /*!< Check security bits in the programmable flash region */
    uint8_t (*target_set_state)(target_state_t state);      /*!< Families can customize target debug states */
    void (*swd_set_target_reset)(uint8_t asserted);         /*!< Families can customize how to send reset to the target */
    uint8_t (*validate_bin_nvic)(const uint8_t *buf);       /*!< Validate a bin file to be flash by drag and drop */
    uint8_t (*validate_hexfile)(const uint8_t *buf);        /*!< Validate a hex file to be flash by drag and drop */
    uint32_t apsel;                             /*!< APSEL for the family */
} target_family_descriptor_t;

//! @brief The active family used by the board.
//!
//! This global is initialized by init_family() just after DAPLink boots. Normally it matches
//! the family specified by the #board_info_t::family_id field of #g_board_info.
extern const target_family_descriptor_t *g_target_family;

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Initialize g_target_family.
void init_family(void);

//! @brief Reset the target into a new state.
//!
//! Used to prepare the target for some operation, or release it for user control.
uint8_t target_set_state(target_state_t state);

//! @brief Controls reset of the target.
void swd_set_target_reset(uint8_t asserted);

//! @brief Get the APSEL for the AHB-AP to use for controlling the target.
uint32_t target_get_apsel(void);

#ifdef __cplusplus
}
#endif

#endif
