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

import argparse
import yaml
import sys
import logging
from generate_custom_cli_files import generate_custom_targets
from generate_custom_cli_files import generate_custom_profile
from generate_mbedignore import generate_mbedignore

logging.basicConfig(format='Line: %(lineno)d %(message)s')
logger = logging.getLogger('yaml gen')
logger.setLevel(logging.DEBUG)

def parse_yaml_dicts(input_dicts, output_dicts):
    for key in input_dicts:
        if key in output_dicts: #merge
            if type(output_dicts[key]) is dict:
                parse_yaml_dicts(input_dicts[key], output_dicts[key])
            elif type(output_dicts[key]) is list:
                output_dicts[key].extend(input_dicts[key])
            else:
                logger.error('Wrong type', typeof(output_dicts[key]))
        else:
            output_dicts[key] = input_dicts[key]

def parse_yaml_files(list_of_list, data_dict):
    if type(list_of_list) is list:
        for entry in list_of_list:
            if type(entry) is list:
                parse_yaml_files(entry, data_dict)
            elif type(entry) is str and entry.endswith('.yaml'):
                try:
                    with open(entry, 'r') as yaml_file:
                        yaml_entries = yaml.load(yaml_file)
                        parse_yaml_dicts(yaml_entries, data_dict)

                except yaml.YAMLError as ex:
                    logger.error("Found yaml parse error", ex)
            else:
                logger.error('Wrong type', typeof(entry))
    else:
        logger.error('Wrong type', typeof(list_of_list))

def generate_mbedcli_files(projects_yaml, target_project):
    with open(projects_yaml, 'r') as top_yaml:
        try:
            build_data = {}
            topdict = yaml.load(top_yaml)
            for dict_key in topdict:
                if dict_key == 'projects':
                    for project in topdict[dict_key]:
                        if project == target_project:
                            parse_yaml_files(topdict[dict_key][project], build_data)
                            generate_custom_targets(project, build_data)
                            generate_custom_profile(build_data)
                            generate_mbedignore(build_data)
                            break
                    break
        except yaml.YAMLError as ex:
            logger.error("Found yaml parse error", ex)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='projects.yaml file parser')
    parser.add_argument("--projects_yaml", type=str, default='projects.yaml', help="top projects.yaml file")
    parser.add_argument("--target_project", type=str, help="target project to be compiled")
    args = parser.parse_args()
    
    if not args.target_project or args.target_project is '':
        sys.exit('Need a target project to compile DAPLink!')

    generate_mbedcli_files(args.projects_yaml, args.target_project)
