#
# DAPLink Interface Firmware
# Copyright (c) 2016-2017, ARM Limited, All Rights Reserved
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import mbed_lstools
import threading
import time
import os
import sys


# This prevents the following error message from getting
# displayed on windows if the mbed dismounts unexpectedly
# during a transfer:
#   There is no disk in the drive. Please insert a disk into
#   drive \Device\<Harddiskx>\<rdrive>
def disable_popup():
    if sys.platform.startswith("win"):
        # pylint: disable=invalid-name
        import ctypes
        SEM_FAILCRITICALERRORS = 1
        GetErrorMode = \
            ctypes.windll.kernel32.GetErrorMode  # @UndefinedVariable
        GetErrorMode.restype = ctypes.c_uint
        GetErrorMode.argtypes = []
        SetErrorMode = \
            ctypes.windll.kernel32.SetErrorMode  # @UndefinedVariable
        SetErrorMode.restype = ctypes.c_uint
        SetErrorMode.argtypes = [ctypes.c_uint]

        err_mode = GetErrorMode()
        err_mode |= SEM_FAILCRITICALERRORS
        SetErrorMode(err_mode)

MAX_REMOUNT_TIME = 5 * 60
should_exit = False
exit_cond = threading.Condition()
print_mut = threading.RLock()
global_start_time = time.time()


class ExitException(Exception):
    pass


def _get_time():
    return time.time() - global_start_time


def sync_print(msg):
    with print_mut:
        print(msg)


def get_mount_point(board_id):
    lstools = mbed_lstools.create()
    mbed_list = lstools.list_mbeds()
    for mbed in mbed_list:
        if mbed['target_id'] == board_id:
            return mbed['mount_point']
    else:
        Exception("Board %s not found" % board_id)


def msd_remount_main(thread_index, board_id):
    global should_exit
    try:
        mount_point = get_mount_point(board_id)
        while True:
            if should_exit:
                raise ExitException()

            # Trigger a remount
            sync_print("Triggering remount for %i %s - %s at %.6f - %s" %
                       (thread_index, mount_point, board_id, _get_time(),
                        time.strftime("%H:%M:%S")))
            file_path = mount_point + "/" + "refresh.act"
            with open(file_path, "wb") as _:
                pass

            # Wait for board to dismount
            start_time = time.time()
            while os.path.exists(mount_point):
                if should_exit:
                    raise ExitException()
                if time.time() - start_time > MAX_REMOUNT_TIME:
                    raise Exception("Board remount timed out")
                time.sleep(0.1)
            sync_print("Drive %s dismount" % mount_point)

            # Wait for drive to come back
            mount_point = None
            start_time = time.time()
            while mount_point is None:
                if should_exit:
                    raise ExitException()
                mount_point = get_mount_point(board_id)
                if time.time() - start_time > MAX_REMOUNT_TIME:
                    raise Exception("Board remount timed out")
                time.sleep(0.1)
            assert os.path.exists(mount_point)

            sync_print("Remount complete as %s" % mount_point)
    except ExitException:
        pass
    except:
        sync_print("Thread %i exception board %s" % (thread_index, board_id))
        with exit_cond:
            should_exit = 1
            exit_cond.notify_all()
        raise


def main():
    global should_exit
    disable_popup()
    lstools = mbed_lstools.create()
    mbed_list = lstools.list_mbeds()
    for thread_index, mbed in enumerate(mbed_list):
        msd_thread = threading.Thread(target=msd_remount_main,
                                      args=(thread_index, mbed['target_id']))
        msd_thread.start()

    try:
        with exit_cond:
            while not should_exit:
                exit_cond.wait(1)
    except KeyboardInterrupt:
        pass
    should_exit = True

    sync_print("Exiting")

if __name__ == "__main__":
    main()
