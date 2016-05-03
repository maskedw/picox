#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
License: MIT license
Copyright (c) <2016> <MaskedW [maskedw00@gmail.com]>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""


import argparse
import shutil
import struct
import tempfile
from collections import OrderedDict
from pathlib import Path
import tabulate

TYPE_DIR = 0
TYPE_REG = 1


class RomFsEntry:
    def __init__(self, path, parent=None):
        self.flags = 0
        self.path = path
        self.parent = parent
        self.entry_off = 0
        self.parent_off = 0xFFFFFFFF
        self.sibling_off = 0xFFFFFFFF
        self.timestamp = 0
        self.name_off = 0

    @classmethod
    def format(cls):
        return '<LLLLL'

    def pack(self):
        return struct.pack('<LLLLL',
                           self.flags,
                           self.parent_off,
                           self.sibling_off,
                           self.name_off,
                           self.timestamp)


class RomFsFileEntry(RomFsEntry):
    def __init__(self, path, parent=None):
        super().__init__(path, parent)
        self.data_off = 0
        self.size = 0
        self.flags = TYPE_REG

    @classmethod
    def format(cls):
        return '<' + super().format().replace('<', '') + 'LL'

    def pack(self):
        return super().pack() + struct.pack('<LL',
                                            self.data_off,
                                            self.size)


class RomFsDirEntry(RomFsEntry):
    def __init__(self, path, parent=None):
        super().__init__(path, parent)
        self.children = []
        self.first_child_off = 0xFFFFFFFF
        self.flags = TYPE_DIR

    @classmethod
    def format(cls):
        return '<' + super().format().replace('<', '') + 'L'

    def pack(self):
        return super().pack() + struct.pack('<L',
                                            self.first_child_off)


class RomFsBuilder():
    def __init__(self):
        pass

    def build(self, root, out):
        root = Path(root).resolve()
        assert(root.is_dir())
        self._entry_table = OrderedDict()
        self._romimg = out

        magic = b'ROMF'
        self._romimg.write(magic)
        self._align()
        begin_entry = self._romimg.tell()

        self._root_entry = RomFsDirEntry(root)
        self._root_entry.entry_off = self._romimg.tell()
        self._makespace(struct.calcsize(self._root_entry.format()))
        self._align()
        self._entry_table[root] = self._root_entry

        self._step1()
        self._step2()
        self._step3()
        self._romimg.seek(begin_entry)
        for entry in self._entry_table.values():
            self._romimg.write(entry.pack())
            self._align()
        return self._get_summary()

    def _get_summary(self):
        headers = ('type', 'name', 'parent', 'entry_off', 'sibling_off',
                   'name_off')

        table = []
        for entry in self._entry_table.values():
            row = []
            if entry.flags == TYPE_REG:
                row.append('REG')
            else:
                row.append('DIR')
            row.append(entry.path.name)
            if entry.parent:
                parent = entry.parent.path.name
            else:
                parent = 'None'
            row.append(parent)
            row.append('0x{:08x}'.format(entry.entry_off))
            row.append('0x{:08x}'.format(entry.sibling_off))
            row.append('0x{:08x}'.format(entry.name_off))
            table.append(row)
        return tabulate.tabulate(table, headers)

    def _step1(self):
        def scan(parent):
            for x in parent.path.iterdir():
                if x.name == '.' or x.name == '..':
                    continue

                if x.is_dir():
                    entry = RomFsDirEntry(x, parent)
                elif x.is_file():
                    entry = RomFsFileEntry(x, parent)
                else:
                    raise RuntimeError('Invalid file type')

                x = x.resolve()
                parent.children.append(entry)
                self._entry_table[x] = entry
                entry.entry_off = self._romimg.tell()
                self._makespace(struct.calcsize(entry.format()))
                self._align()
                if x.is_dir():
                    scan(entry)
        scan(self._root_entry)

    def _step2(self):
        for entry in self._entry_table.values():
            if entry.parent:
                entry.parent_off = entry.parent.entry_off

            if entry.path.is_dir():
                prev = None
                for x in entry.children:
                    if prev:
                        prev.sibling_off = x.entry_off
                    else:
                        entry.first_child_off = entry.children[0].entry_off
                    prev = x
            else:
                entry.data_off = self._romimg.tell()
                data = entry.path.open('rb').read()
                entry.size = len(data)
                self._romimg.write(data)
                self._align()

    def _step3(self):
        for entry in self._entry_table.values():
            entry.name_off = self._romimg.tell()
            if entry == self._root_entry:
                name = '/'
            else:
                name = entry.path.name
            self._romimg.write(name.encode('utf8'))
            self._romimg.write(b'\0')
            self._align()

    def _align(self):
        align = 4
        pos = self._romimg.tell()
        if pos % align != 0:
            pos += align - (pos % align)
        self._romimg.seek(pos)
        pass

    def _makespace(self, size):
        self._romimg.write(b'\0' * size)


parser = argparse.ArgumentParser(
    description='To generate a ROMFS image of picox')
parser.add_argument('-o', '--out', type=str, required=True,
                    help='output path')
parser.add_argument('-v', '--verbose', required=False, action='store_true',
                    help='print mapfile')
parser.add_argument('root', metavar='ROOT',
                    help='romfs image of the root directory')

args = parser.parse_args()
out = Path(args.out)

# 出力先にディレクトリが指定されていないかの確認
with out.open('a+b') as f:
    pass

with tempfile.NamedTemporaryFile(delete=False) as f:
    builder = RomFsBuilder()
    summary = builder.build(args.root, f)
    tempname = f.name

shutil.move(tempname, str(out))
with out.with_suffix('.map').open('w') as f:
    f.write(summary)

if args.verbose:
    print(summary)
