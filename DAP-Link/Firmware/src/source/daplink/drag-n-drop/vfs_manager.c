/**
 * @file    vfs_manager.c
 * @brief   Implementation of vfs_manager.h
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
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

#include <ctype.h>

#include "main.h"
#include "cmsis_os2.h"
#include "rl_usb.h"
#include "virtual_fs.h"
#include "vfs_manager.h"
#include "daplink_debug.h"
#include "info.h"
#include "settings.h"
#include "daplink.h"
#include "util.h"
#include "version_git.h"
#include "IO_Config.h"
#include "file_stream.h"
#include "error.h"

// Set to 1 to enable debugging
#define DEBUG_VFS_MANAGER     0

#if DEBUG_VFS_MANAGER
#define vfs_mngr_printf    debug_msg
#else
#define vfs_mngr_printf(...)
#endif

#define INVALID_TIMEOUT_MS  0xFFFFFFFF
#define MAX_EVENT_TIME_MS   60000

#define CONNECT_DELAY_MS 0
#define RECONNECT_DELAY_MS 2500    // Must be above 1s for windows (more for linux)
// TRANSFER_IN_PROGRESS
#define DISCONNECT_DELAY_TRANSFER_TIMEOUT_MS 20000
// TRANSFER_CAN_BE_FINISHED
#define DISCONNECT_DELAY_TRANSFER_IDLE_MS 500
// TRANSFER_NOT_STARTED || TRASNFER_FINISHED
#define DISCONNECT_DELAY_MS 500

// Make sure none of the delays exceed the max time
COMPILER_ASSERT(CONNECT_DELAY_MS < MAX_EVENT_TIME_MS);
COMPILER_ASSERT(RECONNECT_DELAY_MS < MAX_EVENT_TIME_MS);
COMPILER_ASSERT(DISCONNECT_DELAY_TRANSFER_TIMEOUT_MS < MAX_EVENT_TIME_MS);
COMPILER_ASSERT(DISCONNECT_DELAY_TRANSFER_IDLE_MS < MAX_EVENT_TIME_MS);
COMPILER_ASSERT(DISCONNECT_DELAY_MS < MAX_EVENT_TIME_MS);

typedef enum {
    TRANSFER_NOT_STARTED,
    TRANSFER_IN_PROGRESS,
    TRANSFER_CAN_BE_FINISHED,
    TRASNFER_FINISHED,
} transfer_state_t;

typedef struct {
    vfs_file_t file_to_program;     // A pointer to the directory entry of the file being programmed
    vfs_sector_t start_sector;      // Start sector of the file being programmed by stream
    vfs_sector_t file_start_sector; // Start sector of the file being programmed by vfs
    vfs_sector_t file_next_sector;  // Expected next sector of the file
    vfs_sector_t last_ooo_sector;   // Last out of order sector within the file
    uint32_t size_processed;        // The number of bytes processed by the stream
    uint32_t file_size;             // Size of the file indicated by root dir.  Only allowed to increase
    uint32_t size_transferred;      // The number of bytes transferred
    transfer_state_t transfer_state;// Transfer state
    bool stream_open;               // State of the stream
    bool stream_started;            // Stream processing started. This only gets reset remount
    bool stream_finished;           // Stream processing is done. This only gets reset remount
    bool stream_optional_finish;    // True if the stream processing can be considered done
    bool file_info_optional_finish; // True if the file transfer can be considered done
    bool transfer_timeout;          // Set if the transfer was finished because of a timeout. This only gets reset remount
    stream_type_t stream;           // Current stream or STREAM_TYPE_NONE is stream is closed.  This only gets reset remount
} file_transfer_state_t;

typedef enum {
    VFS_MNGR_STATE_DISCONNECTED,
    VFS_MNGR_STATE_RECONNECTING,
    VFS_MNGR_STATE_CONNECTED
} vfs_mngr_state_t;

static const file_transfer_state_t default_transfer_state = {
    VFS_FILE_INVALID,
    VFS_INVALID_SECTOR,
    VFS_INVALID_SECTOR,
    VFS_INVALID_SECTOR,
    VFS_INVALID_SECTOR,
    0,
    0,
    0,
    TRANSFER_NOT_STARTED,
    false,
    false,
    false,
    false,
    false,
    false,
    STREAM_TYPE_NONE,
};

//Compile option not to include MSC at all, these will be dummy variables
#ifndef MSC_ENDPOINT
BOOL USBD_MSC_MediaReady = __FALSE;
BOOL USBD_MSC_ReadOnly = __FALSE;
U32 USBD_MSC_MemorySize;
U32 USBD_MSC_BlockSize;
U32 USBD_MSC_BlockGroup;
U32 USBD_MSC_BlockCount;
U8 *USBD_MSC_BlockBuf;
#endif

static uint32_t usb_buffer[VFS_SECTOR_SIZE / sizeof(uint32_t)];
static error_t fail_reason = ERROR_SUCCESS;
static file_transfer_state_t file_transfer_state;

// These variables can be access from multiple threads
// so access to them must be synchronized
static vfs_mngr_state_t vfs_state;
static vfs_mngr_state_t vfs_state_next;
static uint32_t time_usb_idle;

static osMutexId_t sync_mutex;
static osThreadId_t sync_thread = 0;

// Synchronization functions
static void sync_init(void);
static void sync_assert_usb_thread(void);
static void sync_lock(void);
static void sync_unlock(void);

static bool changing_state(void);
static void build_filesystem(void);
static void file_change_handler(const vfs_filename_t filename, vfs_file_change_t change, vfs_file_t file, vfs_file_t new_file_data);
static void file_data_handler(uint32_t sector, const uint8_t *buf, uint32_t num_of_sectors);
static bool ready_for_state_change(void);
static void abort_remount(void);

static void transfer_update_file_info(vfs_file_t file, uint32_t start_sector, uint32_t size, stream_type_t stream);
static void transfer_reset_file_info(void);
static void transfer_stream_open(stream_type_t stream, uint32_t start_sector);
static void transfer_stream_data(uint32_t sector, const uint8_t *data, uint32_t size);
static void transfer_update_state(error_t status);


void vfs_mngr_fs_enable(bool enable)
{
    sync_lock();

    if (enable) {
        if (VFS_MNGR_STATE_DISCONNECTED == vfs_state_next) {
            vfs_state_next = VFS_MNGR_STATE_CONNECTED;
        }
    } else {
        vfs_state_next = VFS_MNGR_STATE_DISCONNECTED;
    }

    sync_unlock();
}

void vfs_mngr_fs_remount(void)
{
    sync_lock();

    // Only start a remount if in the connected state and not in a transition
    if (!changing_state() && (VFS_MNGR_STATE_CONNECTED == vfs_state)) {
        vfs_state_next = VFS_MNGR_STATE_RECONNECTING;
    }

    sync_unlock();
}

void vfs_mngr_init(bool enable)
{
    sync_assert_usb_thread();
    build_filesystem();

    if (enable) {
        vfs_state = VFS_MNGR_STATE_CONNECTED;
        vfs_state_next = VFS_MNGR_STATE_CONNECTED;
        USBD_MSC_MediaReady = 1;
    } else {
        vfs_state = VFS_MNGR_STATE_DISCONNECTED;
        vfs_state_next = VFS_MNGR_STATE_DISCONNECTED;
        USBD_MSC_MediaReady = 0;
    }
}

void vfs_mngr_periodic(uint32_t elapsed_ms)
{
    bool change_state;
    vfs_mngr_state_t vfs_state_local;
    vfs_mngr_state_t vfs_state_local_prev;
    sync_assert_usb_thread();
    sync_lock();

    // Return immediately if the desired state has been reached
    if (!changing_state()) {
        sync_unlock();
        return;
    }

    change_state = ready_for_state_change();

    if (time_usb_idle < MAX_EVENT_TIME_MS) {
        time_usb_idle += elapsed_ms;
    }

    if (!change_state) {
        sync_unlock();
        return;
    }

    vfs_mngr_printf("vfs_mngr_periodic()\r\n");
    vfs_mngr_printf("   time_usb_idle=%i\r\n", time_usb_idle);
    vfs_mngr_printf("   transfer_state=%i\r\n", file_transfer_state.transfer_state);
    // Transistion to new state
    vfs_state_local_prev = vfs_state;
    vfs_state = vfs_state_next;

    switch (vfs_state) {
        case VFS_MNGR_STATE_RECONNECTING:
            // Transition back to the connected state
            vfs_state_next = VFS_MNGR_STATE_CONNECTED;
            break;

        default:
            // No state change logic required in other states
            break;
    }

    vfs_state_local = vfs_state;
    time_usb_idle = 0;
    sync_unlock();
    // Processing when leaving a state
    vfs_mngr_printf("    state %i->%i\r\n", vfs_state_local_prev, vfs_state_local);

    switch (vfs_state_local_prev) {
        case VFS_MNGR_STATE_DISCONNECTED:
            // No action needed
            break;

        case VFS_MNGR_STATE_RECONNECTING:
            // No action needed
            break;

        case VFS_MNGR_STATE_CONNECTED:

            // Close ongoing transfer if there is one
            if (file_transfer_state.transfer_state != TRASNFER_FINISHED) {
                vfs_mngr_printf("    transfer timeout\r\n");
                file_transfer_state.transfer_timeout = true;
                transfer_update_state(ERROR_SUCCESS);
            }

            util_assert(TRASNFER_FINISHED == file_transfer_state.transfer_state);
            vfs_user_disconnecting();
            break;
    }

    // Processing when entering a state
    switch (vfs_state_local) {
        case VFS_MNGR_STATE_DISCONNECTED:
            USBD_MSC_MediaReady = 0;
            break;

        case VFS_MNGR_STATE_RECONNECTING:
            USBD_MSC_MediaReady = 0;
            break;

        case VFS_MNGR_STATE_CONNECTED:
            build_filesystem();
            USBD_MSC_MediaReady = 1;
            break;
    }

    return;
}

error_t vfs_mngr_get_transfer_status()
{
    sync_assert_usb_thread();
    return fail_reason;
}

void usbd_msc_init(void)
{
    sync_init();
    build_filesystem();
    vfs_state = VFS_MNGR_STATE_DISCONNECTED;
    vfs_state_next = VFS_MNGR_STATE_DISCONNECTED;
    time_usb_idle = 0;
    USBD_MSC_MediaReady = 0;
}

void usbd_msc_read_sect(uint32_t sector, uint8_t *buf, uint32_t num_of_sectors)
{
    sync_assert_usb_thread();

    // dont proceed if we're not ready
    if (!USBD_MSC_MediaReady) {
        return;
    }

    // indicate msc activity
    main_blink_msc_led(MAIN_LED_FLASH);
    vfs_read(sector, buf, num_of_sectors);
}

void usbd_msc_write_sect(uint32_t sector, uint8_t *buf, uint32_t num_of_sectors)
{
    sync_assert_usb_thread();

    if (!USBD_MSC_MediaReady) {
        return;
    }

    // Restart the disconnect counter on every packet
    // so the device does not detach in the middle of a
    // transfer.
    time_usb_idle = 0;

    if (TRASNFER_FINISHED == file_transfer_state.transfer_state) {
        return;
    }

    // indicate msc activity
    main_blink_msc_led(MAIN_LED_FLASH);
    vfs_write(sector, buf, num_of_sectors);
    if (TRASNFER_FINISHED == file_transfer_state.transfer_state) {
        return;
    }
    file_data_handler(sector, buf, num_of_sectors);
}

static void sync_init(void)
{
    sync_thread = osThreadGetId();
    sync_mutex = osMutexNew(NULL);
}

static void sync_assert_usb_thread(void)
{
    util_assert(osThreadGetId() == sync_thread);
}

static void sync_lock(void)
{
    osMutexAcquire(sync_mutex, 0);
}

static void sync_unlock(void)
{
    osMutexRelease(sync_mutex);
}

static bool changing_state()
{
    return vfs_state != vfs_state_next;
}

static void build_filesystem()
{
    // Update anything that could have changed file system state
    file_transfer_state = default_transfer_state;
    vfs_user_build_filesystem();
    vfs_set_file_change_callback(file_change_handler);
    // Set mass storage parameters
    USBD_MSC_MemorySize = vfs_get_total_size();
    USBD_MSC_BlockSize  = VFS_SECTOR_SIZE;
    USBD_MSC_BlockGroup = 1;
    USBD_MSC_BlockCount = USBD_MSC_MemorySize / USBD_MSC_BlockSize;
    USBD_MSC_BlockBuf   = (uint8_t *)usb_buffer;
}

// Callback to handle changes to the root directory.  Should be used with vfs_set_file_change_callback
static void file_change_handler(const vfs_filename_t filename, vfs_file_change_t change, vfs_file_t file, vfs_file_t new_file_data)
{
    vfs_mngr_printf("vfs_manager file_change_handler(name=%*s, file=%p, change=%i)\r\n", 11, filename, file, change);
    vfs_user_file_change_handler(filename, change, file, new_file_data);
    if (TRASNFER_FINISHED == file_transfer_state.transfer_state) {
        // If the transfer is finished stop further processing
        return;
    }

    if (VFS_FILE_CHANGED == change) {
        if (file == file_transfer_state.file_to_program) {
            stream_type_t stream;
            uint32_t size = vfs_file_get_size(new_file_data);
            vfs_sector_t sector = vfs_file_get_start_sector(new_file_data);
            stream = stream_type_from_name(filename);
            transfer_update_file_info(file, sector, size, stream);
        }
    }

    if (VFS_FILE_CREATED == change) {
        stream_type_t stream;

        if (STREAM_TYPE_NONE != stream_type_from_name(filename)) {
            // Check for a know file extension to detect the current file being
            // transferred.  Ignore hidden files since MAC uses hidden files with
            // the same extension to keep track of transfer info in some cases.
            if (!(VFS_FILE_ATTR_HIDDEN & vfs_file_get_attr(new_file_data))) {
                stream = stream_type_from_name(filename);
                uint32_t size = vfs_file_get_size(new_file_data);
                vfs_sector_t sector = vfs_file_get_start_sector(new_file_data);
                transfer_update_file_info(file, sector, size, stream);
            }
        }
    }

    if (VFS_FILE_DELETED == change) {
        if (file == file_transfer_state.file_to_program) {
            // The file that was being transferred has been deleted
            transfer_reset_file_info();
        }
    }
}

// Handler for file data arriving over USB.  This function is responsible
// for detecting the start of a BIN/HEX file and performing programming
static void file_data_handler(uint32_t sector, const uint8_t *buf, uint32_t num_of_sectors)
{
    stream_type_t stream;
    uint32_t size;

    // this is the key for starting a file write - we dont care what file types are sent
    //  just look for something unique (NVIC table, hex, srec, etc) until root dir is updated
    if (!file_transfer_state.stream_started) {
        // look for file types we can program
        stream = stream_start_identify((uint8_t *)buf, VFS_SECTOR_SIZE * num_of_sectors);

        if (STREAM_TYPE_NONE != stream) {
            transfer_stream_open(stream, sector);
        }
    }

    if (file_transfer_state.stream_started) {
        // Ignore sectors coming before this file
        if (sector < file_transfer_state.start_sector) {
            return;
        }

        // sectors must be in order
        if (sector != file_transfer_state.file_next_sector) {
            vfs_mngr_printf("vfs_manager file_data_handler sector=%i\r\n", sector);

            if (sector < file_transfer_state.file_next_sector) {
                vfs_mngr_printf("    sector out of order! lowest ooo = %i\r\n",
                                file_transfer_state.last_ooo_sector);

                if (VFS_INVALID_SECTOR == file_transfer_state.last_ooo_sector) {
                    file_transfer_state.last_ooo_sector = sector;
                }

                file_transfer_state.last_ooo_sector =
                    MIN(file_transfer_state.last_ooo_sector, sector);
            } else {
                vfs_mngr_printf("    sector not part of file transfer\r\n");
            }

            vfs_mngr_printf("    discarding data - size transferred=0x%x, data=%x,%x,%x,%x,...\r\n",
                            file_transfer_state.size_transferred, buf[0], buf[1], buf[2], buf[3]);
            return;
        }

        // This sector could be part of the file so record it
        size = VFS_SECTOR_SIZE * num_of_sectors;
        file_transfer_state.size_transferred += size;
        file_transfer_state.file_next_sector = sector + num_of_sectors;

        // If stream processing is done then discard the data
        if (file_transfer_state.stream_finished) {
            vfs_mngr_printf("vfs_manager file_data_handler\r\n    sector=%i, size=%i\r\n", sector, size);
            vfs_mngr_printf("    discarding data - size transferred=0x%x, data=%x,%x,%x,%x,...\r\n",
                            file_transfer_state.size_transferred, buf[0], buf[1], buf[2], buf[3]);
            transfer_update_state(ERROR_SUCCESS);
            return;
        }

        transfer_stream_data(sector, buf, size);
    }
}

static bool ready_for_state_change(void)
{
    uint32_t timeout_ms = INVALID_TIMEOUT_MS;
    util_assert(vfs_state != vfs_state_next);

    if (VFS_MNGR_STATE_CONNECTED == vfs_state) {
        switch (file_transfer_state.transfer_state) {
            case TRANSFER_NOT_STARTED:
            case TRASNFER_FINISHED:
                timeout_ms = DISCONNECT_DELAY_MS;
                break;

            case TRANSFER_IN_PROGRESS:
                timeout_ms = DISCONNECT_DELAY_TRANSFER_TIMEOUT_MS;
                break;

            case TRANSFER_CAN_BE_FINISHED:
                timeout_ms = DISCONNECT_DELAY_TRANSFER_IDLE_MS;
                break;

            default:
                util_assert(0);
                timeout_ms = DISCONNECT_DELAY_MS;
                break;
        }
    } else if ((VFS_MNGR_STATE_DISCONNECTED == vfs_state) &&
               (VFS_MNGR_STATE_CONNECTED == vfs_state_next)) {
        timeout_ms = CONNECT_DELAY_MS;
    } else if ((VFS_MNGR_STATE_RECONNECTING == vfs_state) &&
               (VFS_MNGR_STATE_CONNECTED == vfs_state_next)) {
        timeout_ms = RECONNECT_DELAY_MS;
    } else if ((VFS_MNGR_STATE_RECONNECTING == vfs_state) &&
               (VFS_MNGR_STATE_DISCONNECTED == vfs_state_next)) {
        timeout_ms = 0;
    }

    if (INVALID_TIMEOUT_MS == timeout_ms) {
        util_assert(0);
        timeout_ms = 0;
    }

    return time_usb_idle > timeout_ms ? true : false;
}

// Abort a remount if one is pending
void abort_remount(void)
{
    sync_lock();

    // Only abort a remount if in the connected state and reconnecting is the next state
    if ((VFS_MNGR_STATE_RECONNECTING == vfs_state_next) && (VFS_MNGR_STATE_CONNECTED == vfs_state)) {
        vfs_state_next = VFS_MNGR_STATE_CONNECTED;
    }

    sync_unlock();
}

// Update the tranfer state with file information
static void transfer_update_file_info(vfs_file_t file, uint32_t start_sector, uint32_t size, stream_type_t stream)
{
    vfs_mngr_printf("vfs_manager transfer_update_file_info(file=%p, start_sector=%i, size=%i)\r\n", file, start_sector, size);

    if (TRASNFER_FINISHED == file_transfer_state.transfer_state) {
        util_assert(0);
        return;
    }

    // Initialize the directory entry if it has not been set
    if (VFS_FILE_INVALID == file_transfer_state.file_to_program) {
        file_transfer_state.file_to_program = file;

        if (file != VFS_FILE_INVALID) {
            vfs_mngr_printf("    file_to_program=%p\r\n", file);
        }
    }

    // Initialize the starting sector if it has not been set
    if (VFS_INVALID_SECTOR == file_transfer_state.file_start_sector) {
        file_transfer_state.file_start_sector = start_sector;

        if (start_sector != VFS_INVALID_SECTOR) {
            vfs_mngr_printf("    start_sector=%i\r\n", start_sector);
        }
    }

    // Initialize the stream if it has not been set
    if (STREAM_TYPE_NONE == file_transfer_state.stream) {
        file_transfer_state.stream = stream;

        if (stream != STREAM_TYPE_NONE) {
            vfs_mngr_printf("    stream=%i\r\n", stream);
        }
    }

    // Check - File size must either grow or be smaller than the size already transferred
    if ((size < file_transfer_state.file_size) && (size < file_transfer_state.size_transferred) && (size > 0)) {
        vfs_mngr_printf("    error: file size changed from %i to %i\r\n", file_transfer_state.file_size, size);
        transfer_update_state(ERROR_ERROR_DURING_TRANSFER);
        return;
    }

    // Check - Starting sector must be the same  - this is optional for file info since it may not be present initially
    if ((VFS_INVALID_SECTOR != start_sector) && (start_sector != file_transfer_state.file_start_sector)) {
        vfs_mngr_printf("    error: starting sector changed from %i to %i\r\n", file_transfer_state.file_start_sector, start_sector);
        transfer_update_state(ERROR_ERROR_DURING_TRANSFER);
        return;
    }

    // Check - stream must be the same
    if ((stream != STREAM_TYPE_NONE) && (stream != file_transfer_state.stream)) {
        vfs_mngr_printf("    error: changed types during transfer from %i to %i\r\n", file_transfer_state.stream, stream);
        transfer_update_state(ERROR_ERROR_DURING_TRANSFER);
        return;
    }

    // Update values - Size is the only value that can change
    file_transfer_state.file_size = size;
    vfs_mngr_printf("    updated size=%i\r\n", size);

    transfer_update_state(ERROR_SUCCESS);
}

// Reset the transfer information or error if transfer is already in progress
static void transfer_reset_file_info()
{
    vfs_mngr_printf("vfs_manager transfer_reset_file_info()\r\n");
    //check if the data started streaming; size can be updated on matching start sector and stream type
    if(file_transfer_state.stream_started){
        //file, start sector and size has to be updated
        file_transfer_state.file_to_program = VFS_FILE_INVALID;
        file_transfer_state.file_start_sector = VFS_INVALID_SECTOR;
        file_transfer_state.file_size = 0;
    }else{
          file_transfer_state = default_transfer_state;
          abort_remount();
    }

}

// Update the tranfer state with new information
static void transfer_stream_open(stream_type_t stream, uint32_t start_sector)
{
    error_t status;
    util_assert(!file_transfer_state.stream_open);
    util_assert(start_sector != VFS_INVALID_SECTOR);
    vfs_mngr_printf("vfs_manager transfer_update_stream_open(stream=%i, start_sector=%i)\r\n",
                    stream, start_sector);

    // Initialize the starting sector if it has not been set
    if (VFS_INVALID_SECTOR == file_transfer_state.start_sector) {
        file_transfer_state.start_sector = start_sector;

        if (start_sector != VFS_INVALID_SECTOR) {
            vfs_mngr_printf("    start_sector=%i\r\n", start_sector);
        }
    }

    // Initialize the stream if it has not been set
    if (STREAM_TYPE_NONE == file_transfer_state.stream) {
        file_transfer_state.stream = stream;

        if (stream != STREAM_TYPE_NONE) {
            vfs_mngr_printf("    stream=%i\r\n", stream);
        }
    }

    // Check - Starting sector must be the same
    if (start_sector != file_transfer_state.start_sector) {
        vfs_mngr_printf("    error: starting sector changed from %i to %i\r\n", file_transfer_state.start_sector, start_sector);
        transfer_update_state(ERROR_ERROR_DURING_TRANSFER);
        return;
    }

    // Check - stream must be the same
    if (stream != file_transfer_state.stream) {
        vfs_mngr_printf("    error: changed types during transfer from %i to %i\r\n", file_transfer_state.stream, stream);
        transfer_update_state(ERROR_ERROR_DURING_TRANSFER);
        return;
    }

    // Open stream
    status = stream_open(stream);
    vfs_mngr_printf("    stream_open stream=%i ret %i\r\n", stream, status);

    if (ERROR_SUCCESS == status) {
        file_transfer_state.file_next_sector = start_sector;
        file_transfer_state.stream_open = true;
        file_transfer_state.stream_started = true;
    }

    transfer_update_state(status);
}

// Update the tranfer state with new information
static void transfer_stream_data(uint32_t sector, const uint8_t *data, uint32_t size)
{
    error_t status;
    vfs_mngr_printf("vfs_manager transfer_stream_data(sector=%i, size=%i)\r\n", sector, size);
    vfs_mngr_printf("    size processed=0x%x, data=%x,%x,%x,%x,...\r\n",
                    file_transfer_state.size_processed, data[0], data[1], data[2], data[3]);

    if (file_transfer_state.stream_finished) {
        util_assert(0);
        return;
    }

    util_assert(size % VFS_SECTOR_SIZE == 0);
    util_assert(file_transfer_state.stream_open);
    status = stream_write((uint8_t *)data, size);
    vfs_mngr_printf("    stream_write ret=%i\r\n", status);

    if (ERROR_SUCCESS_DONE == status) {
        // Override status so ERROR_SUCCESS_DONE
        // does not get passed into transfer_update_state
        status = stream_close();
        vfs_mngr_printf("    stream_close ret=%i\r\n", status);
        file_transfer_state.stream_open = false;
        file_transfer_state.stream_finished = true;
        file_transfer_state.stream_optional_finish = true;
    } else if (ERROR_SUCCESS_DONE_OR_CONTINUE == status) {
        status = ERROR_SUCCESS;
        file_transfer_state.stream_optional_finish = true;
    } else {
        file_transfer_state.stream_optional_finish = false;
    }

    file_transfer_state.size_processed += size;
    transfer_update_state(status);
}

// Check if the current transfer is still in progress, done, or if an error has occurred
static void transfer_update_state(error_t status)
{
    bool transfer_timeout;
    bool transfer_started;
    bool transfer_can_be_finished;
    bool transfer_must_be_finished;
    bool out_of_order_sector;
    error_t local_status = status;
    util_assert((status != ERROR_SUCCESS_DONE) &&
                (status != ERROR_SUCCESS_DONE_OR_CONTINUE));

    if (TRASNFER_FINISHED == file_transfer_state.transfer_state) {
        util_assert(0);
        return;
    }

    // Update file info status.  The end of a file is never known for sure since
    // what looks like a complete file could be part of a file getting flushed to disk.
    // The criteria for an successful optional finish is
    // 1. A file has been detected
    // 2. The size of the file indicated in the root dir has been transferred
    // 3. The file size is greater than zero
    // 4. Matching start sector set by stream and vfs changes
    file_transfer_state.file_info_optional_finish =
        (file_transfer_state.file_to_program != VFS_FILE_INVALID) &&
        (file_transfer_state.size_transferred >= file_transfer_state.file_size) &&
        (file_transfer_state.file_size > 0) &&
        (file_transfer_state.start_sector == file_transfer_state.file_start_sector);
    transfer_timeout = file_transfer_state.transfer_timeout;
    transfer_started = (VFS_FILE_INVALID != file_transfer_state.file_to_program) ||
                       (STREAM_TYPE_NONE != file_transfer_state.stream);
    // The transfer can be finished if both file and stream processing
    // can be considered complete
    transfer_can_be_finished = file_transfer_state.file_info_optional_finish &&
                               file_transfer_state.stream_optional_finish;
    // The transfer must be fnished if stream processing is for sure complete
    // and file processing can be considered complete
    transfer_must_be_finished = file_transfer_state.stream_finished &&
                                file_transfer_state.file_info_optional_finish;
    out_of_order_sector = false;

    if (file_transfer_state.last_ooo_sector != VFS_INVALID_SECTOR) {
        util_assert(file_transfer_state.start_sector != VFS_INVALID_SECTOR);
        uint32_t sector_offset = (file_transfer_state.last_ooo_sector -
                                  file_transfer_state.start_sector) * VFS_SECTOR_SIZE;

        if (sector_offset < file_transfer_state.size_processed) {
            // The out of order sector was within the range of data already
            // processed.
            out_of_order_sector = true;
        }
    }

    // Set the transfer state and set the status if necessary
    if (local_status != ERROR_SUCCESS) {
        file_transfer_state.transfer_state = TRASNFER_FINISHED;
    } else if (transfer_timeout) {
        if (out_of_order_sector) {
            local_status = ERROR_OOO_SECTOR;
        } else if (!transfer_started) {
            local_status = ERROR_SUCCESS;
        } else if (transfer_can_be_finished) {
            local_status = ERROR_SUCCESS;
        } else {
            local_status = ERROR_TRANSFER_TIMEOUT;
        }

        file_transfer_state.transfer_state = TRASNFER_FINISHED;
    } else if (transfer_must_be_finished) {
        file_transfer_state.transfer_state = TRASNFER_FINISHED;
    } else if (transfer_can_be_finished) {
        file_transfer_state.transfer_state = TRANSFER_CAN_BE_FINISHED;
    } else if (transfer_started) {
        file_transfer_state.transfer_state = TRANSFER_IN_PROGRESS;
    }

    if (TRASNFER_FINISHED == file_transfer_state.transfer_state) {
        vfs_mngr_printf("vfs_manager transfer_update_state(status=%i)\r\n", status);
        vfs_mngr_printf("    file=%p, start_sect= %i %i, size=%i\r\n",
                        file_transfer_state.file_to_program, file_transfer_state.start_sector,
                        file_transfer_state.file_start_sector, file_transfer_state.file_size);
        vfs_mngr_printf("    stream=%i, size_processed=%i, opt_finish=%i, timeout=%i\r\n",
                        file_transfer_state.stream, file_transfer_state.size_processed,
                        file_transfer_state.file_info_optional_finish, transfer_timeout);

        // Close the file stream if it is open
        if (file_transfer_state.stream_open) {
            error_t close_status;
            close_status = stream_close();
            vfs_mngr_printf("    stream closed ret=%i\r\n", close_status);
            file_transfer_state.stream_open = false;

            if (ERROR_SUCCESS == local_status) {
                local_status = close_status;
            }
        }

        // Set the fail reason
        fail_reason = local_status;
        vfs_mngr_printf("    Transfer finished, status: %i=%s\r\n", fail_reason, error_get_string(fail_reason));
    }

    // If this state change is not from aborting a transfer
    // due to a remount then trigger a remount
    if (!transfer_timeout) {
        vfs_mngr_fs_remount();
    }
}
