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
 
"""

Usage example:

python mbedapi.py  --repo http://developer.mbed.org/users/dan/code/pubtest/
--user dan --api http://developer.mbed.org --platform mbed-LPC1768
--destdir /tmp/ --debug 2

This will compile http://developer.mbed.org/users/dan/code/pubtest/
for the 1768 and download the result.

Examples of options:
--extra_symbols "foo=bar,x=y"

--replace_file "main.cpp:/tmp/replace_main.cpp"
(can be repeated)

"""
import os
import getpass
import json
import time
import requests
import logging

MBED_API_SERVER = 'https://os.mbed.com/'


def build_repo(user, password, repo, platform, destdir,
               replace='', symbols='', clean=False, api=MBED_API_SERVER):

    payload = {
        'clean': clean,
        'platform': platform,
        'repo': repo,
        'extra_symbols': symbols
    }

    if replace:
        replace = []
        for pair in replace:
            dest = pair.split(':')[0]
            src = pair.split(':')[1]
            print(dest)
            cwd = os.getcwd()
            srcfile = open(os.path.join(cwd, src), 'r')
            replace.append({dest: srcfile.read()})

        payload['replace'] = json.dumps(replace)
        logging.debug("Payload is: %s" % payload)

    auth = (user, password,)

    # send task to api
    logging.debug(api + "/api/v2/tasks/compiler/start/" + "| data: " +
                  str(payload))
    r = requests.post(api + "/api/v2/tasks/compiler/start/",
                      data=payload, auth=auth)

    logging.debug(r.content)

    if r.status_code != 200:
        raise Exception("Error while talking to the mbed API")

    uuid = json.loads(r.content)['result']['data']['task_id']
    logging.debug("Task accepted and given ID: %s" % uuid)
    success = False

    # poll for output
    for check in range(0, 40):
        logging.debug("Checking for output: cycle %s of %s" % (check, 10))
        time.sleep(2)
        r = requests.get(api + "/api/v2/tasks/compiler/output/%s" %
                         uuid, auth=auth)
        logging.debug(r.content)
        response = json.loads(r.content)
        messages = response['result']['data']['new_messages']
        percent = 0
        for message in messages:
            if message.get('message'):
                if message.get('type') != 'debug':
                    logging.info("[%s] %s" % (message['type'],
                                              message['message']))
            if message.get('action'):
                if message.get('percent'):
                    percent = message['percent']
                logging.info("[%s%% - %s] %s " % (percent, message['action'],
                                                  message.get('file', '')))

        if response['result']['data']['task_complete']:
            logging.info("Task completed.")
            success = response['result']['data']['compilation_success']
            logging.info("Compile success: %s" % (success))
            break

    # now download
    if success:
        logging.info("Downloading your binary")
        params = {
            'repomode': True,
            'program': response['result']['data']['program'],
            'binary': response['result']['data']['binary'],
            'task_id': uuid
        }
        r = requests.get(api + "/api/v2/tasks/compiler/bin/",
                         params=params, auth=auth)
        destination = os.path.join(destdir,
                                   response['result']['data']['binary'])

        with open(destination, 'wb') as fd:
            for chunk in r.iter_content(1024):
                fd.write(chunk)

        logging.info("Finished!")
    else:
        raise Exception("Failed to build platform %s" % platform)
    return destination

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='Build an mbed repository.')
    parser.add_argument('--user', type=str, help='Your username on mbed.', required=True)
    parser.add_argument('--password', type=str, help='Your password on mbed.', default=None, required=False)
    parser.add_argument('--api', type=str, help='URL to API server', required=False, default=MBED_API_SERVER)
    parser.add_argument('--repo', type=str, help='URL of repository to build.', required=True)
    parser.add_argument('--platform', type=str, help='Platform name', required=True)
    parser.add_argument('--destdir', type=str, help='Binary destination directory', required=True)
    parser.add_argument('--replace_file', type=str, help='Replace file and build. Can be repeated. Syntax: remotepath:localpath', required=False, action='append')
    parser.add_argument('--extra_symbols', type=str, help='Provide extra symbols to build system', required=False, action='append')
    parser.add_argument('--clean', action='store_true', help='Force clean build')
    parser.add_argument('--debug', help='Show debugging info', required=False)

    args = parser.parse_args()
    if args.debug:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)
    if args.password is None:
        args.password = getpass.getpass('mbed password: ')
    build_repo(args.user, args.password, args.repo, args.platform,
               args.destdir, args.replace_file, args.extra_symbols,
               args.clean, args.api)
