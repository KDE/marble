#!/usr/bin/env python
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

"""
Modifies version numbers and related constants in cmake and C++ code
to simplify version bumping for Marble releases.
"""

import argparse, re, sys, os.path, subprocess
from tempfile import mkstemp
from shutil import move
from os import remove, close

def printUsage():
    print ('Usage: ' + sys.argv[0] + ' version')
    print ('\tWhere version is a version number in major.minor.patch format, e.g. 0.19.1')

def versionNumber(version):
    match = re.search('([0-4])\\.([0-9]+)\\.([0-9]+)', version)
    if not match:
        raise argparse.ArgumentTypeError('Cannot parse version number ' + version)
    else:
        major = int(match.group(1)) 
        minor = int(match.group(2))
        patch = int(match.group(3))
        
        if minor < 10:
            msg = 'Minor version number too small: Application version is minor version - 10, which should not be smaller than 0.'
            raise argparse.ArgumentTypeError(msg)
        return major, minor, patch
    
def generateVersionString(major, minor, patch):
    humanReadable = 'stable release'
    if patch >= 20:
        humanReadable = '{}.{} development version'.format(major, minor+1)
    if patch >= 80:
        humanReadable = '{}.{} Beta 1'.format(major, minor+1)
    if patch >= 90:
        humanReadable = '{}.{} Beta 2'.format(major, minor+1)
    if patch >= 95:
        humanReadable = '{}.{} Beta 3'.format(major, minor+1)
    if patch >= 97:
        humanReadable = '{}.{} Release Candidate'.format(major, minor+1)
    if patch >= 98:
        humanReadable = '{}.{} Release Candidate {}'.format(major, minor+1, patch-96)
    return '{}.{}.{} ({})'.format(major, minor, patch, humanReadable)

def replaceInFile(fileName, searchTerm, replacement):
    fh, abs_path = mkstemp()
    with open(abs_path,'w') as newFile:
        with open(fileName) as oldFile:
            for line in oldFile:
                if re.search(searchTerm, line):
                    newFile.write(replacement + '\n')
                else:
                    newFile.write(line)
    close(fh)
    remove(fileName)
    move(abs_path, fileName)
    
def ensureCleanOrExit(rootDir, fileName):
    status = subprocess.check_output(['git', 'status', '--short', '--porcelain', fileName], cwd=rootDir)
    if len(status.strip()) > 0:
        print ('File ' + fileName + ' contains local modifications. Please undo or stash them before proceeding.')
        sys.exit(1)

## Main script

parser = argparse.ArgumentParser(description='Update Marble library and application version numbers')
parser.add_argument('version', type=versionNumber, help='New version number in major.minor.patch format, e.g. 0.19.1')
parser.add_argument('--commit', action='store_true', help='Commit changed files automatically')
args = parser.parse_args()

major, minor, patch = args.version
rootDir = os.path.realpath(os.path.join(os.path.dirname(__file__), '..', '..'))

# CMake is responsible to set the so version of the library files
libFileName = os.path.join(rootDir, 'src', 'lib', 'marble', 'CMakeLists.txt')
ensureCleanOrExit(rootDir, libFileName)
libVersionFile = os.path.join(rootDir, 'src', 'lib', 'marble', 'MarbleGlobal.h')
ensureCleanOrExit(rootDir, libVersionFile)
appVersionFile = os.path.join(rootDir, 'src', 'apps', 'marble-ui', 'ControlView.cpp')
ensureCleanOrExit(rootDir, appVersionFile)
winappVersionFile = os.path.join(rootDir, 'install', 'windows', 'marble-common.iss')
ensureCleanOrExit(rootDir, winappVersionFile)

replaceInFile(libFileName, 
              'set\\(MARBLE_LIB_VERSION_MAJOR "[0-9]"\\)',
              'set(MARBLE_LIB_VERSION_MAJOR "{}")'.format(major))
soVersion = minor + 1 if patch > 19 else minor
replaceInFile(libFileName,
              'set\\(MARBLE_LIB_VERSION_MINOR "[0-9]+"\\)',
              'set(MARBLE_LIB_VERSION_MINOR "{}")'.format(soVersion))
replaceInFile(libFileName,
              'set\\(MARBLE_LIB_VERSION_PATCH "[0-9]+"\\)',
              'set(MARBLE_LIB_VERSION_PATCH "{}")'.format(0))
replaceInFile(libFileName, 
              'set\\(MARBLE_ABI_VERSION "[0-9]+"\\)',
              'set(MARBLE_ABI_VERSION "{}")'.format(soVersion))

# We have version constants in MarbleGlobal.h
libVersionOld = 'const QString MARBLE_VERSION_STRING = QString::fromLatin1\( ".*" \);'
libVersionNew = 'const QString MARBLE_VERSION_STRING = QString::fromLatin1( "{}" );'.format(generateVersionString(major, minor, patch))
replaceInFile(libVersionFile, libVersionOld, libVersionNew)
libVersionHexOld = '#define MARBLE_VERSION 0x[0-9a-f]{6}'
libVersionHexNew = '#define MARBLE_VERSION 0x{:02x}{:02x}{:02x}'.format(major, minor, patch)
replaceInFile(libVersionFile, libVersionHexOld, libVersionHexNew)

appVersionOld = '    return "[0-5]\\.[0-9]+\\.[0-9]+ (.*)";'
appVersionNew = '    return "{}";'.format(generateVersionString(major+2, minor-25, patch))
replaceInFile(appVersionFile, appVersionOld, appVersionNew)

winappVersionOld = '#define MyAppVersion "[0-5]\\.[0-9]+\\.[0-9]+"'
winappVersionNew = '#define MyAppVersion "{}.{}.{}"'.format(major+2, minor-25, patch)
replaceInFile(winappVersionFile, winappVersionOld, winappVersionNew)

if args.commit:
    versionStringNew = generateVersionString(major, minor, patch)
    subprocess.call(['git', 'commit', '--message=Version bump to {}'.format(versionStringNew), libFileName, libVersionFile, appVersionFile], winappVersionFile, cwd=rootDir)
    print ('Version bump committed. Please check the output of "git show HEAD".')
