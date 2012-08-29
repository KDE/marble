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

# NOTE: this script only writes the catalogue file at the moment
#       which contains the latest available state vector for
#       every object listed

from __future__ import print_function

from lib.SpaceObject import SpaceObject
from lib.HorizonsClient import HorizonsClient
from lib.TASCClient import TASCClient
from lib.ObjectCatalogue import ObjectCatalogue

####[ Configuration ]#########################################################

object_catalogue_file   = "satcatalogue.msc"
data_file_base_url      = "http://bitquirl.net/~rene/esasocis/orbitdata"

# define all interesting objects
# FIXME: document possible values
OBJECTS = [
    # Mars Express
    SpaceObject( data_source        = SpaceObject.DATASOURCE_HORIZONS,
                 name               = 'Mars Express',
                 horizons_id        = -41,
                 category           = SpaceObject.CATEGORY_SPACECRAFTS,
                 related_body       = SpaceObject.BODY_MARS,
                 mission_start      = '2003-12-23 00:00' ),
    # Venus Express
    SpaceObject( data_source        = SpaceObject.DATASOURCE_HORIZONS,
                 name               = 'Venus Express',
                 horizons_id        = -248,
                 category           = SpaceObject.CATEGORY_SPACECRAFTS,
                 related_body       = SpaceObject.BODY_VENUS,
                 mission_start      = '2006-04-11 00:00' ),
    # Phobos
    SpaceObject( data_source        = SpaceObject.DATASOURCE_HORIZONS,
                 name               = 'Phobos',
                 horizons_id        = 401,
                 category           = SpaceObject.CATEGORY_MOONS,
                 related_body       = SpaceObject.BODY_MARS ),
    # Deimos
    SpaceObject( data_source        = SpaceObject.DATASOURCE_HORIZONS,
                 name               = 'Deimos',
                 horizons_id        = 402,
                 category           = SpaceObject.CATEGORY_MOONS,
                 related_body       = SpaceObject.BODY_MARS ),
    # Smart-1
    SpaceObject( data_source        = SpaceObject.DATASOURCE_TASC,
                 name               = "Smart-1",
                 tasc_mission       = "SM1",
                 category           = SpaceObject.CATEGORY_SPACECRAFTS,
                 related_body       = SpaceObject.BODY_MOON,
                 mission_start      = '2003-09-28 00:00',
                 mission_end        = '2006-09-03 00:00',
                 data_for_day       = '2005-02-28 00:00'),
]

##############################################################################

class OrbitDataFetcher(object):

    def __init__(self, object_catalogue):
        super(OrbitDataFetcher, self).__init__()
        self._object_catalogue = object_catalogue
        self._horizons_client = HorizonsClient()
        self._horizons_client.connect()
        self._tasc_client = TASCClient()

    def __del__(self):
        self._horizons_client.disconnect()

    def fetch(self, objects):
        vecs = []
        for obj in list(objects):
            if(obj.data_source is SpaceObject.DATASOURCE_HORIZONS):
                vecs = self._fetch_from_horizons(obj)
            elif(obj.data_source is SpaceObject.DATASOURCE_TASC):
                vecs = self._fetch_from_tasc(obj)
            else:
                print("Unsupported data source for " + obj.name)
            if(len(vecs) < 1):
                print("No data found! Skipping.")
                continue
            self._object_catalogue.add(obj, vecs[-1])

    def _fetch_from_horizons(self, space_object):
        return self._horizons_client.get_state_vectors_for_object(space_object)

    def _fetch_from_tasc(self, space_object):
        return self._tasc_client.get_state_vectors_for_object(space_object)

# let's start fetching...
object_catalogue = ObjectCatalogue(object_catalogue_file, data_file_base_url)
orbit_data_fetcher = OrbitDataFetcher(object_catalogue)
orbit_data_fetcher.fetch(OBJECTS)

