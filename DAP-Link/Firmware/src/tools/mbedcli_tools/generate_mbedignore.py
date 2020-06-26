#
# DAPLink Interface Firmware
# Copyright (c) 2009-2018, ARM Limited, All Rights Reserved
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

import os

def add_list_path(input_path_list, ouput_path_dict):
    for path_list in input_path_list:
        repath = os.path.normpath(path_list)
        if os.path.isfile(repath):
            dir_path = os.path.dirname(repath)
            if dir_path in ouput_path_dict:
                ouput_path_dict[dir_path].append(os.path.basename(repath))
            else:
                ouput_path_dict[dir_path]= [os.path.basename(repath)]
        elif repath not in ouput_path_dict: #check if it's already in the folder filter list
            ouput_path_dict[repath]=[]

def generate_mbedignore(build_dicts, topsource = 'source', filename = '.mbedignore'):
    sources_tag = ['sources', 'includes']
    sources_list = []
    src_folder_file_dict = {}
    if 'common' in build_dicts:
        #do sources first
        for tag in sources_tag:
            if tag in build_dicts['common']:
                if type(build_dicts['common'][tag]) is dict:
                    for srd_dir in build_dicts['common'][tag]:
                        if type(build_dicts['common'][tag][srd_dir]) is list:
                            add_list_path(build_dicts['common'][tag][srd_dir], src_folder_file_dict)
                elif type(build_dicts['common'][tag]) is list:
                    add_list_path(build_dicts['common'][tag], src_folder_file_dict)
    ignore_list = []
    for file in os.listdir("./"):
        if file != topsource:
            ignore_list.append(file)
    for root, dirs, files in os.walk(topsource):
        if root in src_folder_file_dict:
            if src_folder_file_dict[root] != []:
                for filter_file in files:
                    if filter_file not in src_folder_file_dict[root] and not filter_file.endswith('.h'):
                        ignore_list.append(os.path.join(root, filter_file))
            continue
        else:
            for src_folder in src_folder_file_dict:
                if root in src_folder:
                    for filter_file in files:
                        ignore_list.append(os.path.join(root, filter_file))
                    break
            else:
                ignore_list.append(root)
    with open(filename, 'w') as ignore_file:
        for ignore_path in ignore_list:
            if os.path.isdir(ignore_path): 
                 ignore_path = os.path.join(ignore_path, '*')
            ignore_file.write(ignore_path+'\n')
