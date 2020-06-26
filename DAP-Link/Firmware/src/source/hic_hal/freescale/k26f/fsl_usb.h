/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __FSL_USB_H__
#define __FSL_USB_H__

#include <stdint.h>
#include <stdio.h>
// #include "usb_osa.h"
// #include "usb_misc.h"
// #include "usb_spec.h"

/*!
 * @addtogroup usb_drv
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Defines USB stack major version */
#define USB_STACK_VERSION_MAJOR (0x01U)
/*! @brief Defines USB stack minor version */
#define USB_STACK_VERSION_MINOR (0x00U)
/*! @brief Defines USB stack bugfix version */
#define USB_STACK_VERSION_BUGFIX (0x00U)

/*! @brief USB stack version definition */
#define USB_MAKE_VERSION(major, minor, bugfix) (((major) << 16) | ((minor) << 8) | (bugfix))

/*! @brief USB error code */
typedef enum _usb_status
{
    kStatus_USB_Success = 0x00U, /*!< Success */
    kStatus_USB_Error,           /*!< Failed */

    kStatus_USB_Busy,                       /*!< Busy */
    kStatus_USB_InvalidHandle,              /*!< Invalid handle */
    kStatus_USB_InvalidParameter,           /*!< Invalid parameter */
    kStatus_USB_InvalidRequest,             /*!< Invalid request */
    kStatus_USB_ControllerNotFound,         /*!< Controller cannot be found */
    kStatus_USB_InvalidControllerInterface, /*!< Invalid controller interface */

    kStatus_USB_NotSupported,   /*!< Configuration is not supported */
    kStatus_USB_Retry,          /*!< Enumeration get configuration retry */
    kStatus_USB_TransferStall,  /*!< Transfer stalled */
    kStatus_USB_TransferFailed, /*!< Transfer failed */
    kStatus_USB_AllocFail,      /*!< Allocation failed */
    kStatus_USB_LackSwapBuffer, /*!< Insufficient swap buffer for KHCI */
    kStatus_USB_TransferCancel, /*!< The transfer cancelled */
    kStatus_USB_BandwidthFail,  /*!< Allocate bandwidth failed */
    kStatus_USB_MSDStatusFail,  /*!< For MSD, the CSW status means fail */
} usb_status_t;

/*! @brief USB host handle type define */
typedef void *usb_host_handle;

/*! @brief USB device handle type define. For device stack it is the whole device handle; for host stack it is the
 * attached device instance handle*/
typedef void *usb_device_handle;

/*! @brief USB OTG handle type define */
typedef void *usb_otg_handle;

/*! @brief USB controller ID */
typedef enum _usb_controller_index
{
    kUSB_ControllerKhci0 = 0U, /*!< KHCI 0U */
    kUSB_ControllerKhci1,      /*!< KHCI 1U, Currently, there are no platforms which have two KHCI IPs, this is reserved
                                  to be used in the future. */
    kUSB_ControllerEhci0,      /*!< EHCI 0U */
    kUSB_ControllerEhci1,      /*!< EHCI 1U, Currently, there are no platforms which have two KHCI IPs, this is reserved
                                  to be used in the future. */
} usb_controller_index_t;

/**
* @brief USB stack version fields
*/
typedef struct _usb_version
{
    uint8_t major;  /*!< Major */
    uint8_t minor;  /*!< Minor */
    uint8_t bugfix; /*!< Bug fix */
} usb_version_t;

/*******************************************************************************
 * API
 ******************************************************************************/

/*! @} */

#endif /* __FSL_USB_H__ */
