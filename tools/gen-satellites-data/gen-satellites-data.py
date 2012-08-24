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

from lib.SpaceObject import SpaceObject
from lib.HorizonsClient import HorizonsClient
from lib.ObjectCatalogue import ObjectCatalogue

import time
import calendar

####[ Configuration ]#########################################################

object_catalogue_file   = "satcatalogue.txt"
data_file_base_url      = "http://bitquirl.net/~rene/orbitdata"

# define all interesting objects
# FIXME: document possible values
OBJECTS = [
    # Mars Express
    SpaceObject( data_source        = SpaceObject.DATASOURCE_HORIZONS,
                 name               = 'Mars Express',
                 horizons_id        = -41,
                 object_type        = SpaceObject.OBJECTTYPE_SPACECRAFT,
                 related_body       = SpaceObject.BODY_MARS,
                 mission_start      = '2003-12-23 00:00' ),
    # Venus Express
    SpaceObject( data_source        = SpaceObject.DATASOURCE_HORIZONS,
                 name               = 'Venus Express',
                 horizons_id        = -248,
                 object_type        = SpaceObject.OBJECTTYPE_SPACECRAFT,
                 related_body       = SpaceObject.BODY_VENUS,
                 mission_start      = '2006-04-11 00:00' ),
    # Phobos
    SpaceObject( data_source        = SpaceObject.DATASOURCE_HORIZONS,
                 name               = 'Phobos',
                 horizons_id        = 401,
                 object_type        = SpaceObject.OBJECTTYPE_MOON,
                 related_body       = SpaceObject.BODY_MARS ),
    # Deimos
    SpaceObject( data_source        = SpaceObject.DATASOURCE_HORIZONS,
                 name               = 'Deimos',
                 horizons_id        = 402,
                 object_type        = SpaceObject.OBJECTTYPE_MOON,
                 related_body       = SpaceObject.BODY_MARS ),
]

##############################################################################

class OrbitDataFetcher(object):

    def __init__(self, object_catalogue):
        super(OrbitDataFetcher, self).__init__()
        self._object_catalogue = object_catalogue
        self._horizons_client = HorizonsClient()
        self._horizons_client.connect()

    def __del__(self):
        self._horizons_client.disconnect()

    def fetch(self, objects):
        for obj in list(objects):
            if(obj.data_source is SpaceObject.DATASOURCE_HORIZONS):
                self._fetch_from_horizons(obj)
            else:
                print("Unsupported data source for " + obj.name)

    def _fetch_from_horizons(self, space_object):
        vecs = self._horizons_client.get_state_vectors_for_object(space_object)
        if(len(vecs) < 1):
            print("No data found! Skipping.")
            return
        #self._write_data_file(space_object.filename_prefix + '.txt', vecs)
        self._object_catalogue.add(space_object, vecs[-1])

    #def _write_data_file(self, filename, vectors):
    #    f = open(filename, "w+")
    #    for vector in vectors:
    #        f.write(','.join([str(x) for x in vector]))
    #    f.close()

# let's start fetching...
object_catalogue = ObjectCatalogue(object_catalogue_file, data_file_base_url)
orbit_data_fetcher = OrbitDataFetcher(object_catalogue)
orbit_data_fetcher.fetch(OBJECTS)

