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
import subprocess
from pathlib import Path


parser = argparse.ArgumentParser(
    description='Do the replacement of files')
parser.add_argument('-f', '--file', required=True,
                    help='sed file')
parser.add_argument('-n', '--dry-run', required=False,
                    action='store_true',
                    help='Perform only the output of the target file')
parser.add_argument('targets', metavar='TARGET', nargs='+',
                    help='target dirs')

args = parser.parse_args()

cond = r"\( -type f -not -path '*/\.*' -a -name \*.c -o -name \*.cpp -o -name \*.h -o " \
       r"-name \*.hpp -o -name \*.pro -o -name \*.txt -o -name \*.md -o " \
       r"-name \*.mkd -o -name \*.cmake \)"

for x in args.targets:
    if args.dry_run:
        cmd = 'find {} {}'.format(x, cond)
    else:
        cmd = 'find {} {} -print0 | xargs -0 sed -i -f {}'.format(x, cond, args.file)
    subprocess.check_call(cmd, shell=True)
