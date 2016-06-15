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

"""
Creates vector tiles (small .o5m files with standardized path/filenames) from larger osm (pbf) files
"""


import sys
import os
import math
import csv
import time
from subprocess import call
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

def download(url, directory, refresh):
    filename = url.split('/')[-1]
    path = os.path.join(directory, filename)
    if os.path.exists(path):
        if refresh >= 0:
            aDay = 60 * 60 * 24
            age = (time.time() - os.path.getmtime(path)) / aDay
            if age < refresh:
                return filename
            # else download again
        else:
            return filename
    http = urllib3.PoolManager()
    r = http.request('GET', url, preload_content=False)
    chunk_size = 8192
    file_size_dl = 0
    fileSize = int(r.getheader("content-length"))

    with open(os.path.join(directory, filename), 'wb') as out:
        while True:
            data = r.read(chunk_size)
            if data is None or len(data) == 0:
                break
            file_size_dl += len(data)
            out.write(data)
            print ("Downloading %s: %.1f/%.1f Mb (%3.1f%%)\r" % (filename, file_size_dl / 1024.0 / 1024.0, fileSize / 1024.0 / 1024.0, file_size_dl * 100. / fileSize), end='')

    r.release_conn()
    out.close()
    print ("Done")

    return filename

def run(filenames, cache, refresh, directory, overwrite, zoomLevels):
    for csvfilename in filenames:
        with open(csvfilename, 'r') as csvfile:
            reader = csv.reader(csvfile, delimiter=';', quotechar='|')
            for bounds in reader:
                filename = download(bounds[0], cache, refresh)
                for zoom in zoomLevels:
                    bottomLeft = deg2num(float(bounds[3]), float(bounds[2]), zoom)
                    topRight = deg2num(float(bounds[5]), float(bounds[4]), zoom)
                    xDiff = topRight[0]-bottomLeft[0]
                    yDiff = bottomLeft[1]-topRight[1]
                    total = xDiff*yDiff
                    count = 0
                    cutted = "{}/{}.{}-{}-{}-{}.osm.o5m".format(cache, filename, bounds[2], bounds[3], bounds[4], bounds[5])
                    if not os.path.exists(cutted):
                        print ("Creating cut out region {}".format(cutted))
                        call(["osmconvert", "-t={}/osmconvert_tmp-".format(cache), "--complete-ways", "--complex-ways", "--drop-version", "-b={},{},{},{}".format(bounds[2], bounds[3], bounds[4], bounds[5]), "-o={}".format(cutted), os.path.join(cache, filename)])
                    for x in range(1+bottomLeft[0], topRight[0]+1):
                        for y in range(1+topRight[1], bottomLeft[1]+1):
                            count += 1
                            tl = num2deg(x-1, y-1, zoom)
                            br = num2deg(x, y, zoom)
                            path = "{}/{}/{}".format(directory, zoom, x-1)
                            target = "{}.o5m".format(y-1)
                            filterTarget = "{}_tmp.o5m".format(y-1)
                            if not overwrite and os.path.exists(os.path.join(path, target)):
                                print("Skipping existing file {}\r".format(os.path.join(path, target)), end='')
                            else:
                                call(["mkdir", "-p", path])
                                print ("{} level {}: {}/{} {}\r".format(bounds[1], zoom, count, total, os.path.join(path, target)), end='')
                                filterLevel = "levels/{}.level".format(zoom)
                                if os.path.exists(filterLevel):
                                    call(["osmconvert", "-t={}/osmconvert_tmp-".format(cache), "--complete-ways", "--complex-ways", "--drop-version", "-b={},{},{},{}".format(tl[1],br[0],br[1],tl[0]), cutted, "-o={}".format(os.path.join(path, filterTarget))])
                                    call(["osmfilter", "--parameter-file={}".format(filterLevel), os.path.join(path, filterTarget), "-o={}".format(os.path.join(path, target))])
                                    os.remove(os.path.join(path, filterTarget))
                                else:
                                    call(["osmconvert", "-t={}/osmconvert_tmp-".format(cache), "--complete-ways", "--complex-ways", "--drop-version", "-b={},{},{},{}".format(tl[1],br[0],br[1],tl[0]), cutted, "-o={}".format(os.path.join(path, target))])
                                call(["chmod", "644", os.path.join(path, target)])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Create OSM Vector Tiles for Marble')
    parser.add_argument('file', nargs='+', help='a file with semicolon separated lines in the form filename.osm.pbf;Area Name;west;south;east;north')
    parser.add_argument('-o', '--overwrite', action='store_true', help='Create tiles even if they exist already')
    parser.add_argument('-d', '--directory', help='directory to write tiles to', default='.')
    parser.add_argument('-c', '--cache', help='directory to store intermediate files in', default='.')
    parser.add_argument('-r', '--refresh', type=int, default=-1, help='Re-download cached OSM base file if it is older than REFRESH days (-1: do not re-download)')
    parser.add_argument('-z', '--zoomLevels', type=int, nargs='+', help='zoom levels to generate', default=[13,15,17])
    args = parser.parse_args()
    run(args.file, args.cache, args.refresh, args.directory, args.overwrite)    
    
    