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
Creates vector tiles (small .osm.zip files with standardized path/filenames) from larger osm files
"""


import sys
import os
import math
import csv
from subprocess import call
import zipfile
import argparse
import urllib3

def deg2num(lat_deg, lon_deg, zoom):
    lat_rad = math.radians(lat_deg)
    n = 2.0 ** zoom
    xtile = int((lon_deg + 180.0) / 360.0 * n)
    ytile = int((1.0 - math.log(math.tan(lat_rad) + (1 / math.cos(lat_rad))) / math.pi) / 2.0 * n)
    return (xtile, ytile)

def num2deg(xtile, ytile, zoom):
    n = 2.0 ** zoom
    lon_deg = xtile / n * 360.0 - 180.0
    lat_rad = math.atan(math.sinh(math.pi * (1 - 2 * ytile / n)))
    lat_deg = math.degrees(lat_rad)
    return (lat_deg, lon_deg)

def download(url, directory):
    filename = url.split('/')[-1]
    if os.path.exists(os.path.join(directory, filename)):
        return filename

    http = urllib3.PoolManager()
    r = http.request('GET', url, preload_content=False)
    chunk_size = 8192
    file_size_dl = 0
    fileSize = int(r.getheader("content-length"))
    print ("Downloading: %s %.1f Mb" % (filename, fileSize / 1024.0 / 1024.0))

    with open(os.path.join(directory, filename), 'wb') as out:
        while True:
            data = r.read(chunk_size)
            if data is None or len(data) == 0:
                break
            file_size_dl += len(data)
            out.write(data)

            status = r"%10d  [%3.2f%%]" % (file_size_dl, file_size_dl * 100. / fileSize)
            status = status + chr(8)*(len(status)+1)
            print (status, end='')

    r.release_conn()
    out.close()
    print ("Done")

    return filename

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Create OSM Vector Tiles for Marble')
    parser.add_argument('file', nargs='+', help='a file with semicolon separated lines in the form filename.osm.pbf;Area Name;west;south;east;north')
    parser.add_argument('-o', '--overwrite', action='store_true', help='Create tiles even if they exist already')
    parser.add_argument('-d', '--directory', help='directory to write tiles to', default='.')
    parser.add_argument('-c', '--cache', help='directory to store intermediate files in', default='.')
    parser.add_argument('-z', '--zoomLevels', type=int, nargs='+', help='zoom levels to generate', default=[13,15,17])
    args = parser.parse_args()    
    
    for csvfilename in args.file:
        with open(csvfilename, 'r') as csvfile:
            reader = csv.reader(csvfile, delimiter=';', quotechar='|')
            for bounds in reader:
                filename = download(bounds[0], args.cache)
                for zoom in args.zoomLevels:
                    topLeft = deg2num(float(bounds[3]), float(bounds[2]), zoom)
                    bottomRight = deg2num(float(bounds[5]), float(bounds[4]), zoom)
                    xDiff = bottomRight[0]-topLeft[0]
                    yDiff = topLeft[1]-bottomRight[1]
                    total = xDiff*yDiff
                    count = 0
                    cutted = "{}/{}.{}-{}-{}-{}.osm.o5m".format(args.cache, filename, bounds[2], bounds[3], bounds[4], bounds[5])
                    if not os.path.exists(cutted):
                        print ("Creating cut out region {}".format(cutted))
                        call(["osmconvert", "-t={}/osmconvert_tmp-".format(args.cache), "--complete-ways", "--complex-ways", "--drop-version", "-b={},{},{},{}".format(bounds[2], bounds[3], bounds[4], bounds[5]), "-o={}".format(cutted), os.path.join(args.cache, filename)])
                    for x in range(1+topLeft[0], bottomRight[0]+1):
                        for y in range(1+bottomRight[1], topLeft[1]+1):
                            count += 1
                            tl = num2deg(x-1, y-1, zoom)
                            br = num2deg(x, y, zoom)
                            path = "{}/{}/{}".format(args.directory, zoom, x-1)
                            target = "{}.osm".format(y-1)
                            filterTarget = "{}_tmp.osm".format(y-1)
                            zipTarget = "{}.osm.zip".format(y-1)
                            if not args.overwrite and os.path.exists(os.path.join(path, zipTarget)):
                                print("Skipping existing file {}\r".format(os.path.join(path, zipTarget)), end='')
                            else:
                                call(["mkdir", "-p", path])
                                print ("{} level {}: {}/{} {}\r".format(bounds[1], zoom, count, total, os.path.join(path, zipTarget)), end='')
                                filterLevel = "levels/{}.level".format(zoom)
                                if os.path.exists(filterLevel):
                                    call(["osmconvert", "-t={}/osmconvert_tmp-".format(args.cache), "--complete-ways", "--complex-ways", "--drop-version", "-b={},{},{},{}".format(tl[1],br[0],br[1],tl[0]), cutted, "-o={}".format(os.path.join(path, filterTarget))])
                                    call(["osmfilter", "--parameter-file={}".format(filterLevel), os.path.join(path, filterTarget), "-o={}".format(os.path.join(args.cache, target))])
                                    os.remove(os.path.join(path, filterTarget))
                                else:
                                    call(["osmconvert", "-t={}/osmconvert_tmp-".format(args.cache), "--complete-ways", "--complex-ways", "--drop-version", "-b={},{},{},{}".format(tl[1],br[0],br[1],tl[0]), cutted, "-o={}".format(os.path.join(args.cache, target))])
                                call(["chmod", "644", os.path.join(args.cache, target)])
                                with zipfile.ZipFile(os.path.join(path, zipTarget), 'w') as myzip:
                                    myzip.write(os.path.join(args.cache, target), "{}.osm".format(y-1))
                                os.remove(os.path.join(args.cache, target))
