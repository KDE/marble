#!/usr/bin/python2
#
# This file is part of the Marble Virtual Globe.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2012-2014 Rene Kuettner <rene@bitkanal.net>
#

# Create a Marble Satellite Catalog file with data from various sources.
# See also: http://techbase.kde.org/Projects/Marble/SatelliteCatalogFormat

from __future__ import print_function

from lib.SpaceObjectCatalog import (SpaceObject, HorizonsSpaceObject,
                                   TASCSpaceObject, SpaceObjectCatalog)
from lib.DataDownloader import DataDownloader

####[ Configuration ]#########################################################

object_catalog_file     = "PlanetarySatellites.xml"
data_file_base_url      = "http://files.kde.org/marble/satellites"

# define all interesting objects
# FIXME: document possible values
OBJECTS = [
    # Mars Express
    HorizonsSpaceObject(
        name            = 'Mars Express',
        horizons_id     = -41,
        category        = SpaceObject.CATEGORY_SPACEPROBES,
        related_body    = SpaceObject.BODY_MARS,
        mission_start   = '2003-12-23 00:00',
        ),
    # Mars Odyssey
    #HorizonsSpaceObject(
    #    name            = 'Mars Odyssey',
    #    horizons_id     = -53,
    #    category        = SpaceObject.CATEGORY_SPACEPROBES,
    #    related_body    = SpaceObject.BODY_MARS,
    #    mission_start   = '2001-11-02 00:00',
    #    ),
    # Mars Reconnaisance
    #HorizonsSpaceObject(
    #    name            = 'Mars Reconnaisance',
    #    horizons_id     = -74,
    #    category        = SpaceObject.CATEGORY_SPACEPROBES,
    #    related_body    = SpaceObject.BODY_MARS,
    #    mission_start   = '2005-08-12 11:43',
    #    ),
    # Maven
    HorizonsSpaceObject(
        name            = 'Maven',
        horizons_id     = -202,
        category        = SpaceObject.CATEGORY_SPACEPROBES,
        related_body    = SpaceObject.BODY_MARS,
        mission_start   = '2013-11-18 18:28',
        data_from       = '2014-09-22 04:00'
        ),
    # Mangalyaan
    HorizonsSpaceObject(
        name            = 'Mangalyaan',
        horizons_id     = -3,
        category        = SpaceObject.CATEGORY_SPACEPROBES,
        related_body    = SpaceObject.BODY_MARS,
        mission_start   = '2013-11-05 09:08',
        data_from       = '2014-09-24 09:00'
        ),
    # Venus Express
    HorizonsSpaceObject(
        name            = 'Venus Express',
        horizons_id     = -248,
        category        = SpaceObject.CATEGORY_SPACEPROBES,
        related_body    = SpaceObject.BODY_VENUS,
        mission_start   = '2006-04-11 00:00',
        ),
    # Phobos
    HorizonsSpaceObject(
        name            = 'Phobos',
        horizons_id     = 401,
        category        = SpaceObject.CATEGORY_MOONS,
        related_body    = SpaceObject.BODY_MARS,
        ),
    # Deimos
    HorizonsSpaceObject(
        name            = 'Deimos',
        horizons_id     = 402,
        category        = SpaceObject.CATEGORY_MOONS,
        related_body    = SpaceObject.BODY_MARS,
        ),
    # Smart-1
    TASCSpaceObject(
        name            = "Smart-1",
        tasc_mission    = "SM1",
        category        = SpaceObject.CATEGORY_SPACEPROBES,
        related_body    = SpaceObject.BODY_MOON,
        mission_start   = '2003-09-28 00:00',
        mission_end     = '2006-09-03 00:00',
        data_for_day    = '2005-02-28 00:00',
        ),
]

##############################################################################

class OrbitDataFetcher(object):

    def __init__(self, object_catalog):
        super(OrbitDataFetcher, self).__init__()
        self._object_catalog = object_catalog
        self._downloader = DataDownloader()

    def fetch(self, objects):
        vecs = []
        for obj in list(objects):
            vecs = self._downloader.download_state_vectors(obj)
            if len(vecs) < 1:
                print("No data found! Skipping.")
                continue
            self._object_catalog.add(obj, vecs[-1])

# let's start fetching...
object_catalog = SpaceObjectCatalog(object_catalog_file, data_file_base_url)
orbit_data_fetcher = OrbitDataFetcher(object_catalog)
orbit_data_fetcher.fetch(OBJECTS)
object_catalog.write()
