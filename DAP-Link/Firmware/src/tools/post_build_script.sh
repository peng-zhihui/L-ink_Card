#!/bin/bash -e
##
## DAPLink Interface Firmware
## Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
## SPDX-License-Identifier: Apache-2.0
##
## Licensed under the Apache License, Version 2.0 (the "License"); you may
## not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
## http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
## WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##

export PROJECT_DIR=build
export TOOLS=../../../tools

python --version 2> /dev/null

fromelf --bin $PROJECT_DIR/*.axf -o $PROJECT_DIR/firmware.bin

fromelf --i32 $PROJECT_DIR/*.axf -o $PROJECT_DIR/firmware.hex

python ${TOOLS}/post_compute_crc.py ${PROJECT_DIR}/firmware.hex ${PROJECT_DIR}/firmware_crc
