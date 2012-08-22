#!/usr/bin/python2
#
# This file is part of the Marble Virtual Globe.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2012 Rene Kuettner <rene@bitkanal.net>
#

from __future__ import print_function

from lib.HorizonsClient import HorizonsClient
from lib.ObjectCatalogue import ObjectCatalogue

import time
import calendar

####[ Configuration ]#########################################################

object_catalogue_file   = "satcatalogue.txt"
data_file_base_url      = "http://bitquirl.net/~rene/orbitdata/"

# define all interesting objects
horizons_objects = [
 #   file, name, planet, start date, end date, intervalDays
    ['mex.txt', 'Mars Express',  'Mars',  '2003-12-23 00:00', None, 10 ],
    ['vex.txt', 'Venus Express', 'Venus', '2006-04-11 00:00', None, 10 ] ]

##############################################################################

class OrbitDataFetcher(object):

    def __init__(self, objectCatalogue):
        super(OrbitDataFetcher, self).__init__()
        self._objectCatalogue = objectCatalogue
        self._horizonsClient = HorizonsClient()
        self._horizonsClient.connect()

    def __del__(self):
        self._horizonsClient.disconnect()

    def fetch_from_horizons(self, targetFile, name, planet,
                            tStart, tEnd, intervalDays):
        data = self._horizonsClient.get_state_vector(
            name, planet, tStart, tEnd, intervalDays)
        self._write_data_file(targetFile, data)
        self._objectCatalogue.add_object(name, planet, tStart, tEnd, targetFile)

    def _write_data_file(self, filename, data):
        f = open(filename, "w+")
        f.write(data)
        f.close()

# helper
def to_unix_timestamp(text):
    if(text is None):
        t = time.gmtime(time.time())
        return calendar.timegm(
            time.strptime('{0:02d}-{1:02d}-{2:02d}'.format(
                t.tm_year, t.tm_mon, t.tm_mday), '%Y-%m-%d'))
    return calendar.timegm(time.strptime(text, '%Y-%m-%d %H:%M'))

objectCatalogue = ObjectCatalogue(object_catalogue_file, data_file_base_url)
orbitDataFetcher = OrbitDataFetcher(objectCatalogue)

# request data from horizons
for obj in horizons_objects:
    obj[3] = to_unix_timestamp(obj[3])
    obj[4] = to_unix_timestamp(obj[4])
    orbitDataFetcher.fetch_from_horizons(*obj)

