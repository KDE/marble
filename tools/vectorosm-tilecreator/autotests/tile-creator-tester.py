#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2023 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import argparse
import glob
import re
import os
import subprocess
import sys
import tempfile

parser = argparse.ArgumentParser(description='Test driver for raw data tile creator tirex backend')
parser.add_argument('--tirex-backend', help='Path to the tile creator binary', type=str)
parser.add_argument('--data', help='Path to the test data files', type=str)
arguments = parser.parse_args()

def metaTileHash(x, y):
    h = []
    for i in range(5):
        h += [((x & 0x0f) << 4) | (y & 0x0f)];
        x >>= 4;
        y >>= 4;
    return h

# extract tiles from a meta tile
def readTile(metaTileFile, x, y):
    tile = open(metaTileFile, 'rb')
    tiledata = tile.read()
    if not tiledata[0:4] == b'META':
        print('Not a metatile file')
        sys.exit(1)
    count = int.from_bytes(tiledata[4:8], byteorder='little')
    xCount = int.from_bytes(tiledata[8:12], byteorder='little')
    yCount = int.from_bytes(tiledata[12:16], byteorder='little')
    z = int.from_bytes(tiledata[16:20], byteorder='little')
    entries = []
    for i in range(count):
        offset = int.from_bytes(tiledata[(20+i*8):(24+i*8)], byteorder='little')
        size = int.from_bytes(tiledata[(24+i*8):(28+i*8)], byteorder='little')
        entries.append((offset, size))

    idx = x * xCount + y
    entry = entries[idx]
    return tiledata[entry[0]:(entry[0]+entry[1])]

failCount = 0
for refFile in glob.iglob('*-z*-*-*.osm', root_dir=arguments.data):
    m = re.search(r'(.*)-z(\d+)-(\d+)-(\d+).osm', refFile)
    inputFile = f"{m.group(1)}-input.osm"
    x = int(m.group(3))
    y = int(m.group(4))
    z = int(m.group(2))
    print (inputFile, refFile, x, y, z)

    # run the tile creator in single-shot fake mode
    env = os.environ.copy()
    env['TIREX_BACKEND_SOCKET_FILENO'] = '1'
    env['QT_HASH_SEED'] = '0'
    subprocess.run([arguments.tirex_backend, '-x', str(x), '-y', str(y), '-z', str(z), '-c', '.',
                    '--source', os.path.join(arguments.data, inputFile)], env=env)

    # read output tile from metatile
    h = metaTileHash(x, y)
    metaTileName = os.path.join('output', f"{z}", f"{h[4]}", f"{h[3]}", f"{h[2]}", f"{h[1]}", f"{h[0]}.meta")
    tileData = readTile(metaTileName, 0, 0)

    with tempfile.TemporaryDirectory() as tmpdir:
        o5mOutFile = os.path.join(tmpdir, f"{m.group(1)}-z{z}-{x}-{y}.out.o5m")
        with open(o5mOutFile, 'wb') as f:
            f.write(tileData)

        # convert to OSM format
        osmOutFile = os.path.join(tmpdir, f"{m.group(1)}-z{z}-{x}-{y}.out.osm")
        subprocess.run(['osmconvert', f"-o={osmOutFile}", o5mOutFile])

        # normalize result for easier comparison
        subprocess.run(['python3', os.path.join(os.path.dirname(os.path.abspath(__file__)), 'osm-normalize.py'), osmOutFile])

        # compare
        r = subprocess.run(['diff', '-u', os.path.join(arguments.data, refFile), osmOutFile])
        if r.returncode != 0:
            subprocess.run(['cp', osmOutFile, f"{os.path.join(arguments.data, refFile)}.fail"])
            failCount += 1

print(f"Found {failCount} failures.")
if failCount > 0:
    sys.exit(1)
