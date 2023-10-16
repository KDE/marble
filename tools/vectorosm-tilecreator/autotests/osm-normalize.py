#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2023 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: LGPL-2.0-or-later

import argparse
import xml.etree.ElementTree as ET

parser = argparse.ArgumentParser(description='Normalize OSM data in XML format for easier comparison')
parser.add_argument('osm', nargs='+', help='OSM file to normalize', type=str)
arguments = parser.parse_args()

attrExclusionList = [ 'generator', 'timestamp', 'uid', 'user', 'visible', 'changeset', 'version' ]

def processRecursive(node):
    if not isinstance(node.tag, str):
        return

    # drop undesired attributes
    for attr in attrExclusionList:
        if attr in node.attrib:
            del node.attrib[attr]

    # sort tags
    node[:] = sorted(node, key=lambda node: node.get('k', ''))

    for child in node:
        processRecursive(child)

for f in arguments.osm:
    tree = ET.parse(f)
    root = tree.getroot()
    processRecursive(root)
    ET.indent(tree)
    tree.write(f)
