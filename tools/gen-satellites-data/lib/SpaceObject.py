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

import time
import calendar

class SpaceObject(object):

    DATE_FORMAT             = '%Y-%m-%d %H:%M'

    # data sources
    DATASOURCE_HORIZONS     = 1
    DATASOURCE_TSAC         = 2

    # object types
    OBJECTTYPE_SPACECRAFT   = "Spacecraft"
    OBJECTTYPE_MOON         = "Moon"

    # body constants
    BODY_MERCUR             = "Mercur"
    BODY_VENUS              = "Venus"
    BODY_EARTH              = "Earth"
    BODY_MARS               = "Mars"
    BODY_JUPITER            = "Jupiter"
    BODY_SATURN             = "Saturn"
    BODY_URANUS             = "Uranus"
    BODY_NEPTUN             = "Neptun"

    def __init__(self, **kwargs):
        super(SpaceObject, self).__init__()
        self._data_source = None
        self._filename_prefix = None
        self._name = None
        self._horizons_id = None
        self._object_type = None
        self._related_body = None
        self._mission_start = None
        self._mission_end = None
        self._data_interval_days = 31
        for prop in kwargs.keys():
            if(hasattr(self, prop)):
                setattr(self, prop, kwargs[prop])

    @property
    def data_source(self):
        return self._data_source

    @data_source.setter
    def data_source(self, value):
        self._data_source = int(value)

    @property
    def filename_prefix(self):
        if(self._filename_prefix is None):
            # let's hope this is valid:
            return self.name.replace(' ', '_').lower()
        return self._filename_prefix

    @filename_prefix.setter
    def filename_prefix(self, value):
        self._filename_prefix = str(value)

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, value):
        self._name = str(value)

    @property
    def horizons_id(self):
        if(self._horizons_id is None):
            return self.name
        return self._horizons_id

    @horizons_id.setter
    def horizons_id(self, value):
        self._horizons_id = value

    @property
    def object_type(self):
        return self._object_type

    @object_type.setter
    def object_type(self, value):
        self._object_type = str(value)

    @property
    def related_body(self):
        return self._related_body

    @related_body.setter
    def related_body(self, value):
        self._related_body = str(value)

    @property
    def mission_start(self):
        return self._mission_start

    @mission_start.setter
    def mission_start(self, value):
        t = time.strptime(value, self.DATE_FORMAT)
        self._mission_start = calendar.timegm(t)

    @property
    def mission_end(self):
        return self._mission_end

    @mission_end.setter
    def mission_end(self, value):
        t = time.strptime(value, self.DATE_FORMAT)
        self._mission_end = calendar.timegm(t)

    @property
    def data_interval_days(self):
        return self._data_interval_days

    @data_interval_days.setter
    def data_interval_days(self, value):
        self._data_interval_days = int(value)

