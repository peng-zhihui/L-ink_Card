/**
 * @file    vfs_user.h
 * @brief   VFS user files header.
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2020, ARM Limited, All Rights Reserved
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

#ifndef VFS_USER_H
#define VFS_USER_H

#include <stdbool.h>
#include "virtual_fs.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Hook for file changes in root directory.
//!
//! @param filename Name of the file that has changed.
//! @param change Type of change.
//! @param file
//! @param new_file_data Data being written to the file if _change_ is #VFS_FILE_CHANGED.
//! @retval true The hook handled the change notification, the caller should do nothing else.
//! @retval false The hook did nothing, continue with normal behaviour.
bool vfs_user_file_change_handler_hook(const vfs_filename_t filename,
        vfs_file_change_t change, vfs_file_t file, vfs_file_t new_file_data);

//! @brief Hook for magic files.
//!
//! This hook is intended to simplify checking for magic files. In addition to allowing support for
//! new magic files, you can also change the behaviour of or disable standard magic files.
//!
//! @param filename Name of the file that was created.
//! @param[out] do_remount Whether the caller should remount the MSD volume. Only applies if true
//!     is returned. The default is true, so if the hook does not modify this parameter and returns
//!     true, a remount is performed.
//! @retval true The hook handled the specified file. A remount will be performed if requested,
//!     but otherwise no other standard behaviour is applied.
//! @retval false The hook did not handle the file; continue with canonical behaviour.
bool vfs_user_magic_file_hook(const vfs_filename_t filename, bool *do_remount);

#ifdef __cplusplus
}
#endif

#endif // VFS_USER_H
