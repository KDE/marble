#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import argparse
import sys

parser = argparse.ArgumentParser(description='Inspect Tirex Metatiles')
parser.add_argument('metatile', nargs=1, help='Metatile file')
parser.add_argument('--list', help='List table of contents of the given metatile', action='store_true')
parser.add_argument('--extract', nargs=1, type=int, help='Extract the given sub-tile data')
arguments = parser.parse_args()

tile = open(arguments.metatile[0], 'rb')
tiledata = tile.read()
if not tiledata[0:4] == b'META':
    print('Not a metatile file')
    sys.exit(1)

count = int.from_bytes(tiledata[4:8], byteorder='little')
x = int.from_bytes(tiledata[8:12], byteorder='little')
y = int.from_bytes(tiledata[12:16], byteorder='little')
z = int.from_bytes(tiledata[16:20], byteorder='little')

entries = []
for i in range(count):
    offset = int.from_bytes(tiledata[(20+i*8):(24+i*8)], byteorder='little')
    size = int.from_bytes(tiledata[(24+i*8):(28+i*8)], byteorder='little')
    entries.append((offset, size))

if arguments.list:
    print(f"Count: {count}\nX: {x}\nY: {y}\nZ: {z}\nEntries:")
    for i in range(count):
        print(f"  {i}: Offset: {entries[i][0]}, Size: {entries[i][1]}")
    sys.exit(0)

if arguments.extract[0] < 0 or arguments.extract[0] > count:
    print('Tile index out of range')
    sys.exit(1)

idx = arguments.extract[0]
entry = entries[idx]
sys.stdout.buffer.write(tiledata[entry[0]:(entry[0]+entry[1])])
