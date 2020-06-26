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
 
from __future__ import absolute_import
from __future__ import print_function
import six
import sys
import time


class TestInfo(object):
    # Higher number = more severe
    SUBTEST = 0
    INFO = 1
    WARNING = 2
    FAILURE = 3

    _MSG_TYPES = set((INFO, WARNING, FAILURE))
    _MSG_LEVEL_TO_FMT_STR = {
        INFO: "Info: %s",
        WARNING: "Warning: %s",
        FAILURE: "Failure: %s",
    }

    def __init__(self, name, init_print=True):
        self._all = []
        self.failures = 0
        self.warnings = 0
        self.infos = 0
        self.name = name
        if init_print:
            self._print_msg("SubTest: " + name)

    def failure(self, msg):
        assert isinstance(msg, six.string_types)
        self._add_entry(self.FAILURE, msg)

    def warning(self, msg):
        assert isinstance(msg, six.string_types)
        self._add_entry(self.WARNING, msg)

    def info(self, msg):
        assert isinstance(msg, six.string_types)
        self._add_entry(self.INFO, msg)

    def print_msg(self, warning_level, max_recursion=0, spacing=2,
                  log_file=sys.stdout, _recursion_level=0):
        """
        Print problems at the given level

        By default only the top level passes and fails are printed.
        Set max_recursion to the number of subtests to be printed, or
        to None if all levels should be printed.
        """
        assert warning_level in self._MSG_TYPES
        assert max_recursion is None or max_recursion >= 0

        if self.get_failed():
            result_str = 'Failure'
            test_level = self.FAILURE
        elif self.get_warning():
            result_str = 'Warning'
            test_level = self.WARNING
        else:
            result_str = 'Pass'
            test_level = self.INFO
        prefix = ' ' * (_recursion_level * spacing)

        # Check if test should be printed - the waning level
        # is enabled, or this is the top level test
        if test_level < warning_level and _recursion_level != 0:
            return

        # Print test header
        print(prefix + "Test: %s: %s" % (self.name, result_str),
              file=log_file)

        # Check for recursion termination
        if max_recursion is not None and _recursion_level > max_recursion:
            return
        _recursion_level += 1

        # Print messages
        prefix = ' ' * (_recursion_level * spacing)
        for msg_level, msg in self._all:
            if msg_level == self.SUBTEST:
                test_info = msg
                test_info.print_msg(warning_level, max_recursion,
                                    spacing, log_file, _recursion_level)
            else:
                fmt = prefix + self._MSG_LEVEL_TO_FMT_STR[msg_level]
                if msg_level >= warning_level:
                    print(fmt % msg, file=log_file)

    def get_failed(self):
        self._update_counts()
        return self.failures != 0

    def get_warning(self):
        self._update_counts()
        return self.warnings != 0

    def get_name(self):
        return self.name

    def create_subtest(self, name):
        assert isinstance(name, six.string_types)
        test_info = TestInfo(name)
        self._add_entry(self.SUBTEST, test_info)
        return test_info

    def attach_subtest(self, subtest):
        assert isinstance(subtest, TestInfo)
        self._add_entry(self.SUBTEST, subtest)

    def get_counts(self):
        """
        Return the number of events that occured

        Return the number of even messages as a
        tuple containing (failure_count, warning_count, info_count).
        """
        self._update_counts()
        return self.failures, self.warnings, self.infos

    def _update_counts(self):
        self.failures, self.warnings, self.infos = 0, 0, 0
        for msg_level, msg in self._all:
            if msg_level == self.SUBTEST:
                test_info = msg
                failures, warnings, infos = test_info.get_counts()
                self.failures += failures
                self.warnings += warnings
                self.infos += infos
            else:
                if msg_level == self.FAILURE:
                    self.failures += 1
                elif msg_level == self.WARNING:
                    self.warnings += 1
                elif msg_level == self.INFO:
                    self.infos += 1
                else:
                    # Should never get here
                    assert False

    def _add_entry(self, entry_type, msg):
        if entry_type is self.SUBTEST:
            assert isinstance(msg, TestInfo)
            # Test name printed in constructor
        else:
            assert isinstance(msg, six.string_types)
            self._print_msg(msg)
        self._all.append((entry_type, msg))

    @staticmethod
    def _print_msg(msg):
        print(get_timestamp_tag() + msg)


class TestInfoStub(TestInfo):

    def __init__(self):
        super(TestInfoStub, self).__init__('stub test', False)

    def create_subtest(self, name):
        assert isinstance(name, six.string_types)
        return TestInfoStub()

    @staticmethod
    def _print_msg(msg):
        print(get_timestamp_tag() + "%s"%(msg,))

def get_timestamp_tag():
    return "[{:0<17f}] ".format(time.time())
