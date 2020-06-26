#!/usr/bin/env python
"""
 mbed
 Copyright (c) 2017-2019 ARM Limited

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
"""

from __future__ import print_function
import os
import struct
import binascii
import argparse
import logging
from six.moves import StringIO
import jinja2
from collections import namedtuple
from itertools import count

from elftools.common.py3compat import bytes2str
from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection

logger = logging.getLogger(__name__)
logger.addHandler(logging.NullHandler())


def main():
    parser = argparse.ArgumentParser(description="Algo Extracter")
    parser.add_argument("input", help="File to extract flash algo from")
    parser.add_argument("template", default="py_blob.tmpl",
                        help="Template to use")
    parser.add_argument("output", help="Output file")
    args = parser.parse_args()

    with open(args.input, "rb") as file_handle:
        data = file_handle.read()
    algo = PackFlashAlgo(data)
    algo.process_template(args.template, args.output)


class PackFlashAlgo(object):
    """
    Class to wrap a flash algo

    This class is intended to provide easy access to the information
    provided by a flash algorithm, such as symbols and the flash
    algorithm itself.
    """

    REQUIRED_SYMBOLS = set([
        "Init",
        "UnInit",
        "EraseSector",
        "ProgramPage",
    ])

    EXTRA_SYMBOLS = set([
        "BlankCheck",
        "EraseChip",
        "Verify",
    ])

    def __init__(self, data):
        """Construct a PackFlashAlgorithm from an ElfFileSimple"""
        self.elf = ElfFileSimple(data)
        self.flash_info = PackFlashInfo(self.elf)

        self.flash_start = self.flash_info.start
        self.flash_size = self.flash_info.size
        self.page_size = self.flash_info.page_size
        self.sector_sizes = self.flash_info.sector_info_list

        symbols = {}
        symbols.update(_extract_symbols(self.elf, self.REQUIRED_SYMBOLS))
        symbols.update(_extract_symbols(self.elf, self.EXTRA_SYMBOLS,
                                        default=0xFFFFFFFF))
        self.symbols = symbols

        sections_to_find = (
            ("PrgCode", "SHT_PROGBITS"),
            ("PrgData", "SHT_PROGBITS"),
            ("PrgData", "SHT_NOBITS"),
        )

        ro_rw_zi = _find_sections(self.elf, sections_to_find)
        ro_rw_zi = _algo_fill_zi_if_missing(ro_rw_zi)
        error_msg = _algo_check_for_section_problems(ro_rw_zi)
        if error_msg is not None:
            raise Exception(error_msg)

        sect_ro, sect_rw, sect_zi = ro_rw_zi
        self.ro_start = sect_ro["sh_addr"]
        self.ro_size = sect_ro["sh_size"]
        self.rw_start = sect_rw["sh_addr"]
        self.rw_size = sect_rw["sh_size"]
        self.zi_start = sect_zi["sh_addr"]
        self.zi_size = sect_zi["sh_size"]

        self.algo_data = _create_algo_bin(ro_rw_zi)

    def format_algo_data(self, spaces, group_size, fmt):
        """"
        Return a string representing algo_data suitable for use in a template

        The string is intended for use in a template.

        :param spaces: The number of leading spaces for each line
        :param group_size: number of elements per line (element type
            depends of format)
        :param fmt: - format to create - can be either "hex" or "c"
        """
        padding = " " * spaces
        if fmt == "hex":
            blob = binascii.b2a_hex(self.algo_data)
            line_list = []
            for i in xrange(0, len(blob), group_size):
                line_list.append('"' + blob[i:i + group_size] + '"')
            return ("\n" + padding).join(line_list)
        elif fmt == "c":
            blob = self.algo_data[:]
            pad_size = 0 if len(blob) % 4 == 0 else 4 - len(blob) % 4
            blob = blob + "\x00" * pad_size
            integer_list = struct.unpack("<" + "L" * (len(blob) / 4), blob)
            line_list = []
            for pos in range(0, len(integer_list), group_size):
                group = ["0x%08x" % value for value in
                         integer_list[pos:pos + group_size]]
                line_list.append(", ".join(group))
            return (",\n" + padding).join(line_list)
        else:
            raise Exception("Unsupported format %s" % fmt)

    def process_template(self, template_path, output_path, data_dict=None):
        """
        Generate output from the supplied template

        All the public methods and fields of this class can be accessed from
        the template via "algo".

        :param template_path: Relative or absolute file path to the template
        :param output_path: Relative or absolute file path to create
        :param data_dict: Additional data to use when generating
        """
        if data_dict is None:
            data_dict = {}
        else:
            assert isinstance(data_dict, dict)
            data_dict = dict(data_dict)
        assert "algo" not in data_dict, "algo already set by user data"
        data_dict["algo"] = self

        with open(template_path) as file_handle:
            template_text = file_handle.read()

        template = jinja2.Template(template_text)
        target_text = template.render(data_dict)

        with open(output_path, "wb") as file_handle:
            file_handle.write(target_text)


def _extract_symbols(simple_elf, symbols, default=None):
    """Fill 'symbols' field with required flash algo symbols"""
    to_ret = {}
    for symbol in symbols:
        if symbol not in simple_elf.symbols:
            if default is not None:
                to_ret[symbol] = default
                continue
            raise Exception("Missing symbol %s" % symbol)
        to_ret[symbol] = simple_elf.symbols[symbol].value
    return to_ret


def _find_sections(elf, name_type_pairs):
    """Return a list of sections the same length and order of the input list"""
    sections = [None] * len(name_type_pairs)
    for section in elf.iter_sections():
        section_name = bytes2str(section.name)
        section_type = section["sh_type"]
        for i, name_and_type in enumerate(name_type_pairs):
            if name_and_type != (section_name, section_type):
                continue
            if sections[i] is not None:
                raise Exception("Elf contains duplicate section %s attr %s" %
                                (section_name, section_type))
            sections[i] = section
    return sections


def _algo_fill_zi_if_missing(ro_rw_zi):
    """Create an empty zi section if it is missing"""
    s_ro, s_rw, s_zi = ro_rw_zi
    if s_rw is None:
        return ro_rw_zi
    if s_zi is not None:
        return ro_rw_zi
    s_zi = {
        "sh_addr": s_rw["sh_addr"] + s_rw["sh_size"],
        "sh_size": 0
    }
    return s_ro, s_rw, s_zi


def _algo_check_for_section_problems(ro_rw_zi):
    """Return a string describing any errors with the layout or None if good"""
    s_ro, s_rw, s_zi = ro_rw_zi
    if s_ro is None:
        return "RO section is missing"
    if s_rw is None:
        return "RW section is missing"
    if s_zi is None:
        return "ZI section is missing"
    if s_ro["sh_addr"] != 0:
        return "RO section does not start at address 0"
    if s_ro["sh_addr"] + s_ro["sh_size"] != s_rw["sh_addr"]:
        return "RW section does not follow RO section"
    if s_rw["sh_addr"] + s_rw["sh_size"] != s_zi["sh_addr"]:
        return "ZI section does not follow RW section"
    return None


def _create_algo_bin(ro_rw_zi):
    """Create a binary blob of the flash algo which can execute from ram"""
    sect_ro, sect_rw, sect_zi = ro_rw_zi
    algo_size = sect_ro["sh_size"] + sect_rw["sh_size"] + sect_zi["sh_size"]
    algo_data = bytearray(algo_size)
    for section in (sect_ro, sect_rw):
        start = section["sh_addr"]
        size = section["sh_size"]
        data = section.data()
        assert len(data) == size
        algo_data[start:start + size] = data
    return algo_data


class PackFlashInfo(object):
    """Wrapper class for the non-executable information in an FLM file"""

    FLASH_DEVICE_STRUCT = "<H128sHLLLLBxxxLL"
    FLASH_SECTORS_STRUCT = "<LL"
    FLASH_SECTORS_STRUCT_SIZE = struct.calcsize(FLASH_SECTORS_STRUCT)
    SECTOR_END = 0xFFFFFFFF

    def __init__(self, elf_simple):
        dev_info = elf_simple.symbols["FlashDevice"]
        info_start = dev_info.value
        info_size = struct.calcsize(self.FLASH_DEVICE_STRUCT)
        data = elf_simple.read(info_start, info_size)
        values = struct.unpack(self.FLASH_DEVICE_STRUCT, data)

        self.version = values[0]
        self.name = values[1].strip("\x00")
        self.type = values[2]
        self.start = values[3]
        self.size = values[4]
        self.page_size = values[5]
        self.value_empty = values[7]
        self.prog_timeout_ms = values[8]
        self.erase_timeout_ms = values[9]

        sector_gen = self._sector_and_sz_itr(elf_simple,
                                             info_start + info_size)
        self.sector_info_list = list(sector_gen)

    def __str__(self):
        desc = ""
        desc += "Flash Device:" + os.linesep
        desc += "  name=%s" % self.name + os.linesep
        desc += "  version=0x%x" % self.version + os.linesep
        desc += "  type=%i" % self.type + os.linesep
        desc += "  start=0x%x" % self.start + os.linesep
        desc += "  size=0x%x" % self.size + os.linesep
        desc += "  page_size=0x%x" % self.page_size + os.linesep
        desc += "  value_empty=0x%x" % self.value_empty + os.linesep
        desc += "  prog_timeout_ms=%i" % self.prog_timeout_ms + os.linesep
        desc += "  erase_timeout_ms=%i" % self.erase_timeout_ms + os.linesep
        desc += "  sectors:" + os.linesep
        for sector_start, sector_size in self.sector_info_list:
            desc += ("    start=0x%x, size=0x%x" %
                     (sector_start, sector_size) + os.linesep)
        return desc

    def _sector_and_sz_itr(self, elf_simple, data_start):
        """Iterator which returns starting address and sector size"""
        for entry_start in count(data_start, self.FLASH_SECTORS_STRUCT_SIZE):
            data = elf_simple.read(entry_start, self.FLASH_SECTORS_STRUCT_SIZE)
            size, start = struct.unpack(self.FLASH_SECTORS_STRUCT, data)
            start_and_size = start, size
            if start_and_size == (self.SECTOR_END, self.SECTOR_END):
                return
            yield start_and_size


SymbolSimple = namedtuple("SymbolSimple", "name, value, size")


class ElfFileSimple(ELFFile):
    """Wrapper for elf object which allows easy access to symbols and rom"""

    def __init__(self, data):
        """Construct a ElfFileSimple from bytes or a bytearray"""
        super(ElfFileSimple, self).__init__(StringIO.StringIO(data))
        self.symbols = self._read_symbol_table()

    def _read_symbol_table(self):
        """Read the symbol table into the field "symbols" for easy use"""
        section = self.get_section_by_name(b".symtab")
        if not section:
            raise Exception("Missing symbol table")

        if not isinstance(section, SymbolTableSection):
            raise Exception("Invalid symbol table section")

        symbols = {}
        for symbol in section.iter_symbols():
            name_str = bytes2str(symbol.name)
            if name_str in symbols:
                logging.debug("Duplicate symbol %s", name_str)
            symbols[name_str] = SymbolSimple(name_str, symbol["st_value"],
                                             symbol["st_size"])
        return symbols

    def read(self, addr, size):
        """Read program data from the elf file

        :param addr: physical address (load address) to read from
        :param size: number of bytes to read
        :return: Requested data or None if address is unmapped
        """
        for segment in self.iter_segments():
            seg_addr = segment["p_paddr"]
            seg_size = min(segment["p_memsz"], segment["p_filesz"])
            if addr >= seg_addr + seg_size:
                continue
            if addr + size <= seg_addr:
                continue
            # There is at least some overlap

            if addr >= seg_addr and addr + size <= seg_addr + seg_size:
                # Region is fully contained
                data = segment.data()
                start = addr - seg_addr
                return data[start:start + size]


if __name__ == '__main__':
    main()
