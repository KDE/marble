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
import hashlib

class Tile(object):

    def __init__(self, x, y, zoom):
        self.x = x
        self.y = y
        self.zoom = zoom

    def west(self):
        return self.__longitude(self.x)

    def east(self):
        return self.__longitude(self.x+1)

    def north(self):
        return self.__latitude(self.y)

    def south(self):
        return self.__latitude(self.y+1)

    def __longitude(self, x):
        n = 2.0 ** self.zoom
        return x / n * 360.0 - 180.0

    def __latitude(self, y):
        n = 2.0 ** self.zoom
        lat_rad = math.atan(math.sinh(math.pi * (1 - 2 * y / n)))
        return math.degrees(lat_rad)


class Coordinate(object):

    def __init__(self, lon, lat):
        self.lon = lon
        self.lat = lat

    def tile(self, zoom):
        lat_rad = math.radians(self.lat)
        n = 2.0 ** zoom
        xtile = int((self.lon + 180.0) / 360.0 * n)
        ytile = int((1.0 - math.log(math.tan(lat_rad) + (1 / math.cos(lat_rad))) / math.pi) / 2.0 * n)
        return Tile(xtile, ytile, zoom)


class TileLevelRegion(object):

    def __init__(self, coordinateA, coordinateB, zoom):
        west = min(coordinateA.lon, coordinateB.lon)
        east = max(coordinateA.lon, coordinateB.lon)
        south = min(coordinateA.lat, coordinateB.lat)
        north = max(coordinateA.lat, coordinateB.lat)
        self.topLeft = Coordinate(west, north).tile(zoom)
        self.bottomRight = Coordinate(east, south).tile(zoom)

    def tileCount(self):
        xCount = 1 + self.bottomRight.x - self.topLeft.x
        yCount = 1 + self.bottomRight.y - self.topLeft.y
        return xCount * yCount

    def tiles(self):
        for x in range(self.topLeft.x, self.bottomRight.x+1):
            for y  in range(self.topLeft.y, self.bottomRight.y+1):
                yield Tile(x, y, self.topLeft.zoom)


class InputProvider(object):

    def __init__(self, cacheDirectory, inputFile, refresh, overwrite):
        self._cacheDirectory = cacheDirectory
        self.__inputFile = download(inputFile, cacheDirectory, refresh)
        self.overwrite = overwrite
        self.__createdFiles = set()

        md5 = hashlib.md5()
        md5.update(self.__inputFile.encode('utf-8'))
        self.__inputDigest = md5.hexdigest()[0:7]

    def file(self, tile):
        return self.__zoomOut(tile, tile.zoom - 2)

    def __zoomOut(self, tile, zoom):
        if zoom < 0:
            return self.__inputFile

        coordinate = Coordinate(tile.west(), tile.north())
        baseZoom = max(0, zoom)
        baseTile = coordinate.tile(baseZoom)
        cutted = "{}/{}_{}_{}_{}.o5m".format(self._cacheDirectory, self.__inputDigest, baseTile.zoom, baseTile.x, baseTile.y)
        if (self.overwrite and cutted not in self.__createdFiles) or not os.path.exists(cutted):
            print ("Creating cut out region {}{}\r".format(cutted, ' ' * 10), end='')
            self.__createdFiles.add(cutted)
            inputFile = self.__zoomOut(tile, zoom - 2)
            call(["osmconvert", "-t={}/osmconvert_tmp-".format(self._cacheDirectory), "--complete-ways", "--complex-ways", "--drop-version", "-b={},{},{},{}".format(baseTile.west(), baseTile.south(), baseTile.east(), baseTile.north()), "-o={}".format(cutted), inputFile])
        return cutted


def download(url, directory, refresh):
    filename = url.split('/')[-1]
    path = os.path.join(directory, filename)
    if os.path.exists(path):
        if refresh >= 0:
            aDay = 60 * 60 * 24
            age = (time.time() - os.path.getmtime(path)) / aDay
            if age < refresh:
                return path
            # else download again
        else:
            return path
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

    return path

def run(filenames, cache, refresh, directory, overwrite, zoomLevels):
    for csvfilename in filenames:
        with open(csvfilename, 'r') as csvfile:
            reader = csv.reader(csvfile, delimiter=';', quotechar='|')
            for bounds in reader:
                inputProvider = InputProvider(cache, bounds[0], refresh, overwrite)
                topLeft = Coordinate(float(bounds[2]), float(bounds[5]))
                bottomRight = Coordinate(float(bounds[4]), float(bounds[3]))
                for zoom in zoomLevels:
                    bbox = TileLevelRegion(topLeft, bottomRight, zoom)
                    total = bbox.tileCount()
                    count = 0
                    for tile in bbox.tiles():
                        count += 1
                        path = "{}/{}/{}".format(directory, zoom, tile.x)
                        target = "{}.o5m".format(tile.y)
                        boxString = "-b={},{},{},{}".format(tile.west(), tile.south(), tile.east(), tile.north())
                        filterTarget = "{}_tmp.o5m".format(tile.y)
                        if not overwrite and os.path.exists(os.path.join(path, target)):
                            print("Skipping existing file {}\r".format(os.path.join(path, target)), end='')
                        else:
                            cutted = inputProvider.file(tile)
                            call(["mkdir", "-p", path])
                            print ("{} level {}: {}/{} {}\r".format(bounds[1], zoom, count, total, os.path.join(path, target)), end='')
                            filterLevel = "levels/{}.level".format(zoom)
                            if os.path.exists(filterLevel):
                                call(["osmconvert", "-t={}/osmconvert_tmp-".format(cache), "--complete-ways", "--complex-ways", "--drop-version", boxString, cutted, "-o={}".format(os.path.join(cache, filterTarget))])
                                call(["osmfilter", "--parameter-file={}".format(filterLevel), os.path.join(cache, filterTarget), "-o={}".format(os.path.join(path, target))])
                                os.remove(os.path.join(cache, filterTarget))
                            else:
                                call(["osmconvert", "-t={}/osmconvert_tmp-".format(cache), "--complete-ways", "--complex-ways", "--drop-version", boxString, cutted, "-o={}".format(os.path.join(path, target))])
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
    run(args.file, args.cache, args.refresh, args.directory, args.overwrite, args.zoomLevels)
    
    
