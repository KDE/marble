#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# This file is part of the Marble Virtual Globe.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2015 Dennis Nienhüser <nienhueser@kde.org>
#

import os
import sys
import argparse
from subprocess import call
import tempfile
import shutil

class Filter(object):
    '''
    Decides which files to include in the APK
    '''

    def __init__(self, base):
        self.base = base.rstrip(os.sep)

    def shouldPackage(self, dir, files):
        # Change absolute directory name to one relative to the installation directory
        dir = dir.replace(self.base, '', 1)

        if dir == '':
            # Would not end up in the package, but is slightly faster
            return ['include']
        if dir == '/assets/data':
            # Currently not used
            return ['mwdbii', 'weather', 'naturalearth', 'flags']
        elif dir == '/assets/data/maps':
            # Other planets are not used
            return ['moon']
        elif dir == '/assets/data/maps/earth':
            # Unused map themes
            return ['srtm', 'srtm2', 'bluemarble', 'temp-july', 'precip-july', 'temp-dec', 'precip-dec', 'citylights', 'plain', 'schagen1689', 'political', 'behaim1492', 'openstreetmap', 'clouds', 'sentinel2']
        elif dir == '/assets/data/maps/earth/openstreetmap':
            # Large images from example KML tour
            return [item for item in files if item.endswith('.png') or item.endswith('.jpg')]
        elif dir == '/assets/data/placemarks':
            # Only include very basic placemarks
            return ['moonlandingsites.cache', 'moonterrain.cache', 'elevplacemarks.cache', 'otherplacemarks.cache', 'cityplacemarks.cache']
        elif dir == '/assets/data/svg':
            # Large images. worldmap.svg is used by the overviewmap, bring back if that plugin should be enabled
            return ['application-x-marble.svg', 'marsmap.svg', 'marble-logo.svg', 'lunarmap.svg', 'worldmap.svg']
        elif dir == '/assets/plugins':
            # Whitelisted plugins, all others are ignored
            search = ['LatLonPlugin', 'NominatimSearchPlugin', 'LocalDatabasePlugin', 'LocalOsmSearchPlugin']
            routing = ['CycleStreetsPlugin', 'OpenRouteServicePlugin', 'NominatimReverseGeocodingPlugin']
            fileFormats = ['CachePlugin', 'GpxPlugin', 'KmlPlugin', 'OsmPlugin']
            floatItems = ['License', 'MapScaleFloatItem']
            positioning = ['QtPositioningPositionProviderPlugin']
            render = ['StarsPlugin', 'GraticulePlugin']
            plugins = search + routing + fileFormats + floatItems + positioning + render
            whitelist = set(['lib{}.so'.format(plugin) for plugin in plugins])
            masked = [item for item in files if item not in whitelist]
            if len(files) - len(masked) != len(whitelist):
                print ('Warning: At least one white-listed plugin is not installed')
            return masked
        elif dir.startswith('/libs/'):
            # other android app binary
            return ['libMarbleBehaim.so']

        return []

qtDir = os.environ.get('Qt5_android')
if qtDir is None:
    print ('Please setup the Qt5_android environment variable point to your Qt installation')
    sys.exit(1)

parser = argparse.ArgumentParser(description='Create an Android application package (APK) for Marble Maps')
parser.add_argument('--target', help='Target filename (or directory) for the .apk package')
parser.add_argument('--keystore', help='Keystore file for signing the package')
parser.add_argument('--storepass', help='Keystore password for signing the package')
parser.add_argument('--release', action='store_true', help='Build a release package')
parser.add_argument('--install', action='store_true', help='Install the package to a connected device (uninstalling it before, if needed)')
parser.add_argument('--reinstall', action='store_true', help='Install the package to a connected device (keeping previous data intact, if any)')
parser.add_argument('directory', help='The directory where the Android build is installed to')
args = parser.parse_args()

# Sanity check for given options
if not args.install and not args.reinstall and args.target is None:
    print('Please pass one of --install, --reinstall or --target to either install the package directly or store it locally.')
    sys.exit(1)
jsonFile = os.path.join(args.directory, 'share', 'deploy-marble-maps.json')
if not os.path.isfile(jsonFile):
    print('Cannot find {}. Is {} really a Marble Android installation?'.format(jsonFile, args.directory))
    sys.exit(1)

# Gather needed tools
deployExe = os.path.join(qtDir, 'bin', 'androiddeployqt')
antExe = os.environ.get('ANT', '/usr/bin/ant')

with tempfile.TemporaryDirectory() as tempDir:
    os.rmdir(tempDir) #  shutil.copytree does not like directories to exist
    filter = Filter(args.directory)
    shutil.copytree(args.directory, tempDir, ignore=filter.shouldPackage)
    deployOptions = ['--verbose', '--output', tempDir, '--input', jsonFile, '--ant', antExe]

    # Debug vs. release type packages, signing options
    if args.release or args.keystore is not None:
        deployOptions.append('--release')
    if args.keystore is not None:
        deployOptions.append('--sign')
        deployOptions.append(args.keystore)
        deployOptions.append('Marble')
        deployOptions.append('--tsa')
        deployOptions.append('http://timestamp.digicert.com')
    if args.storepass is not None:
        deployOptions.append('--storepass')
        deployOptions.append(args.storepass)

    # Options for installing the created package to a connected device
    if args.install:
        deployOptions.append('--install')
    if args.reinstall:
        deployOptions.append('--reinstall')
    call([deployExe] + deployOptions)

    if args.target is not None:
        packageType = 'debug'
        if args.keystore is not None:
            packageType = 'release-signed'
        elif args.release:
            packageType = 'release-unsigned'

        targetFile = os.path.join(tempDir, 'bin', 'QtApp-{}.apk'.format(packageType))
        if os.path.isfile(targetFile):
            shutil.copy(targetFile, args.target)
            print ('Created APK at ' + args.target)
        else:
            sys.exit(1)
