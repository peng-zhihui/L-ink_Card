/**
 * @file    usb.h
 * @brief   USB Header
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

#ifndef __USB_H__
#define __USB_H__

/* General USB header files                                                   */
#include "usb_def.h"
#include "usb_cdc.h"
#include "usb_hid.h"
#include "usb_msc.h"
#include "usb_bulk.h"
#include "usb_webusb.h"
#include "usb_winusb.h"


/* USB Device header files                                                    */
#include "usbd_core.h"
#include "usbd_core_cdc.h"
#include "usbd_core_hid.h"
#include "usbd_core_msc.h"
#include "usbd_core_webusb.h"
#include "usbd_core_winusb.h"

#include "usbd_desc.h"
#include "usbd_event.h"
#include "usbd_cdc_acm.h"
#include "usbd_hid.h"
#include "usbd_msc.h"
#include "usbd_hw.h"

#endif  /* __USB_H__ */
