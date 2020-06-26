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

project_tool=$1
env_folder=${2-env}
requirements_file=${3-requirements.txt}

if [[ "$project_tool" != "mbedcli" &&  "$project_tool" != "" ]]; then
	echo "Only mbedcli is supported in Linux build"
	exit 1
else
	echo "Compiling in Linux using mbedcli"
fi

if [[ "$ARM_PATH" == "" ]]; then
	echo "Need variable ARM_PATH to point to ARMCC directory"
	exit 1
else
	echo "Using ARMCC in $ARM_PATH"
fi

echo $env_folder
echo $requirements_file

if [ ! -d $env_folder ];then
	echo "Generating virtualenv folder $env_folder"
	virtualenv $env_folder
else
	echo "Using $env_folder"
fi

source $env_folder/bin/activate
pip install -r $requirements_file
mbed deploy

python tools/mbedcli_compile.py --clean --release
if [ $? -ne 0 ]; then
	exit $?
fi
python tools/copy_release_files.py --project-tool mbedcli
exit $?
