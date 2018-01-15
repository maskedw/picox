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

import re
import sys
import jinja2
import itertools
from more_itertools import chunked
import argparse
from pathlib import Path


CSOURCE_TEMPLATE = '''\
#include <stdint.h>
#include <stddef.h>

union Bin2C__MaxAlign
{
    char c; int i; long l; long long ll; void* p;
    float f; double d; long double ld; void(*fp)(void);
};

{% for x in binaries %}
union bin2c_{{x.name}}_tag
{
    uint8_t data[{{x.size}}];
    union Bin2C__MaxAlign align;
};
const size_t bin2c_{{x.name}}_size = {{x.size}};
const union bin2c_{{x.name}}_tag bin2c_{{x.name}}_data = { {
{{x.data}}
} };
{% endfor -%}
'''

CHEADER_TEMPLATE = '''\
#ifndef bin2c_{{filename}}_h_
#define bin2c_{{filename}}_h_

#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

{% for x in binaries %}
extern const size_t bin2c_{{x.name}}_size;
extern const uint8_t bin2c_{{x.name}}_data;
{%- endfor %}


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* bin2c_{{filename}}_h_ */
'''


def normalize(name):
    name = re.sub(r'[^a-zA-Z0-9_]', '_', name)
    return name


class Hex:
    def __init__(self, path):
        self.path = path
        pass

    def __str__(self):
        with Path(self.path).open('rb') as f:
            data = f.read()
        hexstr = ','.join('0x{:02X}'.format(x) for x in data)
        return '\n'.join(''.join(x) for x in chunked(hexstr, len('0x00,') * 8))


class Bin:
    def __init__(self, path):
        path = Path(path)
        if not path.exists() or not path.is_file():
            raise RuntimeError('fatal error: "{}" no such file'.format(path))

        self.name = normalize(path.name)
        self.size = path.stat().st_size
        self.data = Hex(path)


parser = argparse.ArgumentParser(
    description='To convert the binary to C source and header')
parser.add_argument('-f', '--file', required=False,
                    help='load the targets from a file')
parser.add_argument('-o', '--out', metavar='OUT', required=True,
                    help='output path')
parser.add_argument('files', metavar='FILE', nargs='*',
                    help='target files')

args = parser.parse_args()
if not args.file and not args.files:
    raise RuntimeError('fatal error: no input files')

files = args.files
out = Path(args.out)
if args.file:
    lines = filter(None, (line.strip() for line in open(args.file)))
    files = itertools.chain(lines, files)

render_args = {}

filenames = [Path(x).name for x in files]
filenames.append(out.name)

try:
    for x in filenames:
        x.encode('ascii')
except UnicodeEncodeError:
    sys.exit('UnicodeEncodeError::"{}" filename must be "ASCII"'.format(x))

out = out.with_name(normalize(out.name))
render_args['filename'] = out.name
render_args['binaries'] = [Bin(x) for x in files]


with out.with_suffix('.c').open('w') as f:
    ret = jinja2.Template(CSOURCE_TEMPLATE).render(render_args)
    f.write(ret)

with out.with_suffix('.h').open('w') as f:
    ret = jinja2.Template(CHEADER_TEMPLATE).render(render_args)
    f.write(ret)
