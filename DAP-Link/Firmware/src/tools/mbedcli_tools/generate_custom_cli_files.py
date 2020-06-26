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

import json

def generate_custom_profile(build_dicts, compiler='ARM', tool_search='make_armcc', filename = 'custom_profile.json'):
    profile_dict = {
        compiler: {
            "common": [],
            "asm": [], 
            "c": [], 
            "cxx": [], 
            "ld": []
        }
    }
    if 'tool_specific' in build_dicts:
        for entry in build_dicts['tool_specific']:
            if entry == tool_search:
                if 'misc' in build_dicts['tool_specific'][entry]:
                    for flags in build_dicts['tool_specific'][entry]['misc']:
                        if flags.endswith('_flags'):
                            cli_entry = flags[:-len('_flags')]
                            if cli_entry in profile_dict[compiler]:
                                profile_dict[compiler][cli_entry].extend(build_dicts['tool_specific'][entry]['misc'][flags])
                            else:
                                profile_dict[compiler][cli_entry] = build_dicts['tool_specific'][entry]['misc'][flags]

    with open(filename, 'w') as custom_profile:
        json.dump(profile_dict, custom_profile, indent=4, separators=(',', ': '))

def generate_custom_targets(target_name, build_dicts, out_ext='hex', filename = 'custom_targets.json'):
    target_dict = { 
        target_name : {
            "core": build_dicts['common']['core'][0],
            "supported_toolchains": ["ARM"],
            "inherits": ["Target"],
            "OUTPUT_EXT": out_ext,
            "macros" : []
        } 
    }
    if 'common' in build_dicts:
        for entry in build_dicts['common']:
            if entry == 'macros':
                if entry in target_dict[target_name]:
                    target_dict[target_name][entry].extend(build_dicts['common'][entry])
                else: 
                    target_dict[target_name][entry] = build_dicts['common'][entry]

    with open(filename, 'w') as custom_targets:
        json.dump(target_dict,custom_targets, indent=4, separators=(',', ': '))
