#!/bin/bash -e
#
# DAPLink Interface Firmware
# Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
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

export TOOLS=../../../tools
export DIR=../../../source/daplink
#  Create dsion_git_tmpl.txt ${DIR}\version_git.h

python --version 2> nul

#  Run python script to delete default version_git.h and create the real one
python ${TOOLS}/pre_build_script.py
