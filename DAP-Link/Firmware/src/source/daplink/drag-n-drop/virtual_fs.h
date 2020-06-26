/**
 * @file    virtual_fs.h
 * @brief   FAT 12/16 filesystem handling
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

#ifndef VIRTUAL_FS_H
#define VIRTUAL_FS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VFS_CLUSTER_SIZE        0x1000
#define VFS_SECTOR_SIZE         512
#define VFS_INVALID_SECTOR      0xFFFFFFFF
#define VFS_FILE_INVALID        0
#define VFS_MAX_FILES           16

typedef char vfs_filename_t[11];

typedef enum {
    VFS_FILE_ATTR_READ_ONLY     = (1 << 0),
    VFS_FILE_ATTR_HIDDEN        = (1 << 1),
    VFS_FILE_ATTR_SYSTEM        = (1 << 2),
    VFS_FILE_ATTR_VOLUME_LABEL  = (1 << 3),
    VFS_FILE_ATTR_SUB_DIR       = (1 << 4),
    VFS_FILE_ATTR_ARCHIVE       = (1 << 5),
} vfs_file_attr_bit_t;

typedef enum {
    VFS_FILE_CREATED = 0,   /*!< A new file was created */
    VFS_FILE_DELETED,       /*!< An existing file was deleted */
    VFS_FILE_CHANGED,       /*!< Some attribute of the file changed.
                                  Note: when a file is deleted or
                                  created a file changed
                                  notification will also occur*/
} vfs_file_change_t;

typedef void *vfs_file_t;
typedef uint32_t vfs_sector_t;

// Callback for when data is written to a file on the virtual filesystem
typedef void (*vfs_write_cb_t)(uint32_t sector_offset, const uint8_t *data, uint32_t num_sectors);
// Callback for when data is ready from the virtual filesystem
typedef uint32_t (*vfs_read_cb_t)(uint32_t sector_offset, uint8_t *data, uint32_t num_sectors);
// Callback for when a file's attributes are changed on the virtual filesystem.  Note that the 'file' parameter
// can be saved and compared to other files to see if they are referencing the same object.  The
// same cannot be done with new_file_data since it points to a temporary buffer.
typedef void (*vfs_file_change_cb_t)(const vfs_filename_t filename, vfs_file_change_t change,
                                     vfs_file_t file, vfs_file_t new_file_data);

// Initialize the filesystem with the given size and name
void vfs_init(const vfs_filename_t drive_name, uint32_t disk_size);

// Get the total size of the virtual filesystem
uint32_t vfs_get_total_size(void);

// Add a file to the virtual FS and return a handle to this file.
// This must be called before vfs_read or vfs_write are called.
// Adding a new file after vfs_read or vfs_write have been called results in undefined behavior.
vfs_file_t vfs_create_file(const vfs_filename_t filename, vfs_read_cb_t read_cb, vfs_write_cb_t write_cb, uint32_t len);

// Set the attributes of a file
void vfs_file_set_attr(vfs_file_t file, vfs_file_attr_bit_t attr);

// Get the starting sector of this file.
// NOTE - If the file size is 0 there is no starting
// sector so VFS_INVALID_SECTOR will be returned.
vfs_sector_t vfs_file_get_start_sector(vfs_file_t file);

// Get the size of the file.
uint32_t vfs_file_get_size(vfs_file_t file);

// Get the attributes of a file
vfs_file_attr_bit_t vfs_file_get_attr(vfs_file_t file);

// Set the callback when a file is created, deleted or has atributes changed.
void vfs_set_file_change_callback(vfs_file_change_cb_t cb);

// Read one or more sectors from the virtual filesystem
void vfs_read(uint32_t sector, uint8_t *buf, uint32_t num_of_sectors);

// Write one or more sectors to the virtual filesystem
void vfs_write(uint32_t sector, const uint8_t *buf, uint32_t num_of_sectors);

#ifdef __cplusplus
}
#endif

#endif
