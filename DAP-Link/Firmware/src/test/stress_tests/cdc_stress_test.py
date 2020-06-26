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
import serial

should_exit = False
exit_cond = threading.Condition()
print_mut = threading.RLock()
global_start_time = time.time()


def _get_time():
    return time.time() - global_start_time


def sync_print(msg):
    with print_mut:
        print(msg)


def cdc_throughput_main(thread_index, serial_port):
    global should_exit
    ser = None
    try:
        count = 0
        while not should_exit:
            ser = serial.Serial(serial_port)  # open serial port
            ser.baudrate = 115200
            ser.write("this is test data")
            ser.baudrate = 9600
            ser.write("more test data")
            ser.close()

            if count % 10 == 0:
                sync_print("Thread %i on loop %10i at %.6f - %s - port %s" %
                           (thread_index, count, _get_time(),
                            time.strftime("%H:%M:%S"), serial_port))
            count += 1

    except:
        sync_print("Error on thread %i serial port %s" %
                   (thread_index, serial_port))
        if ser is not None:
            ser.close()
        with exit_cond:
            should_exit = 1
            exit_cond.notify_all()
        raise


def main():
    global should_exit
    lstools = mbed_lstools.create()
    mbed_list = lstools.list_mbeds()
    for thread_index, mbed in enumerate(mbed_list):
        cdc_thread = threading.Thread(target=cdc_throughput_main,
                                      args=(thread_index, mbed['serial_port']))
        cdc_thread.start()

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
