#
# This file is part of the Marble Virtual Globe.  
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2012-2014 Rene Kuettner <rene@bitkanal.net>
#

from __future__ import print_function

import time
import calendar

from lxml import etree
from lxml.builder import ElementMaker


class SpaceObject(object):

    DATE_FORMAT             = '%Y-%m-%d %H:%M'

    # object catagories 
    CATEGORY_SPACECRAFTS    = "Spacecrafts"
    CATEGORY_SPACEPROBES    = "Spaceprobes"
    CATEGORY_MOONS          = "Moons"

    # body constants
    BODY_MERCUR             = "Mercur"
    BODY_VENUS              = "Venus"
    BODY_EARTH              = "Earth"
    BODY_MARS               = "Mars"
    BODY_JUPITER            = "Jupiter"
    BODY_SATURN             = "Saturn"
    BODY_URANUS             = "Uranus"
    BODY_NEPTUN             = "Neptun"
    BODY_MOON               = "Moon"

    def __init__(self, **kwargs):
        super(SpaceObject, self).__init__()
        self._data_source = None
        self._filename_prefix = None
        self._name = None
        self._category = None
        self._related_body = None
        self._mission_start = None
        self._mission_end = None
        self._data_from = None
        self._data_until = None
        self._data_interval_days = 31
        for prop in kwargs.keys():
            if hasattr(self, prop):
                setattr(self, prop, kwargs[prop])

    @property
    def data_source(self):
        return self._data_source

    @property
    def filename_prefix(self):
        if self._filename_prefix is None:
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
    def category(self):
        return self._category

    @category.setter
    def category(self, value):
        self._category = str(value)

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
    def data_from(self):
        if self._data_from is None:
           if self.mission_start is None:
               return time.time() - (time.time() % (3600*24)) + 1
           else:
               return self.mission_start + (24*3600)
        return self._data_from

    @data_from.setter
    def data_from(self, value):
        t = time.strptime(value, self.DATE_FORMAT)
        self._data_from = calendar.timegm(t)

    @property
    def data_until(self):
        if self._data_until is None:
            if self.mission_end is None:
                return time.time()
            else:
                return self.mission_end - (24*3600)
        return self._data_until

    @data_until.setter
    def data_until(self, value):
        t = time.strptime(value, self.DATE_FORMAT)
        self._data_until = calendar.timegm(t)

    @property
    def data_for_day(self):
        return self.data_until - (self.data_until % (3600*24)) + 1

    @data_for_day.setter
    def data_for_day(self, value):
        tm = time.strptime(value, self.DATE_FORMAT)
        t = calendar.timegm(tm)
        self._data_from = t
        self._data_until = t + 60

    @property
    def data_interval_days(self):
        return self._data_interval_days

    @data_interval_days.setter
    def data_interval_days(self, value):
        self._data_interval_days = int(value)


class TASCSpaceObject(SpaceObject):

    def __init__(self, **kwargs):
        super(TASCSpaceObject, self).__init__(**kwargs)
        self._tasc_mission = None

    @property
    def tasc_mission(self):
        if self._tasc_mission is None:
            return self.name
        return self._tasc_mission

    @tasc_mission.setter
    def tasc_mission(self, value):
        self._tasc_mission = value


class HorizonsSpaceObject(SpaceObject):

    def __init__(self, **kwargs):
        super(HorizonsSpaceObject, self).__init__(**kwargs)
        self._horizons_id = None

    @property
    def horizons_id(self):
        if self._horizons_id is None:
            return self.name
        return self._horizons_id

    @horizons_id.setter
    def horizons_id(self, value):
        self._horizons_id = value


class SpaceObjectCatalog(object):

    def __init__(self, filename, baseURL):
        super(SpaceObjectCatalog, self).__init__()
        self._filename = filename;
        self._baseURL = baseURL
        self._file = None
        self._open()
        self._initXML();

    def __del__(self):
        self._close()

    def add(self, space_obj, latest_vector):
        #url  = self._baseURL + "/" + space_obj.filename_prefix + '.txt'
        #icon = self._baseURL + "/" + space_obj.filename_prefix + '.png'
        satellite = self._E.satellite(
            self._E.name(space_obj.name),
            self._E.category(space_obj._category),
            self._E.relatedBody(space_obj.related_body),
            self._E.stateVector(
                self._E.position(
                    x=str(latest_vector[1]),
                    y=str(latest_vector[2]),
                    z=str(latest_vector[3])
                ),
                self._E.velocity(
                    x=str(latest_vector[4]),
                    y=str(latest_vector[5]),
                    z=str(latest_vector[6])
                ),
            mjd=str(latest_vector[0])
            ),
            #allvectors=url,
            #icon=icon,
        )
        mission = self._E.mission()
        if space_obj.mission_start is not None:
            mission.append(self._E.start(str(space_obj.mission_start)))
        if space_obj.mission_end is not None:
            mission.append(self._E.end(str(space_obj.mission_end)))
        if len(mission):
            satellite.append(mission)
        self._xml.append(satellite)
        print(space_obj.name + " added to object catalog.")

    def write(self):
        print("Writing catalog to file: " + self._filename)
        self._file.write(etree.tostring(self._xml,
                                        pretty_print=True,
                                        xml_declaration=True,
                                        encoding='utf-8'))

    def _initXML(self):
        self._E = ElementMaker(
            namespace="http://marble.kde.org/satellitecatalog",
            nsmap={'msc' : "http://marble.kde.org/satellitecatalog"})
        self._xml = self._E.MarbleSatelliteCatalog()

    def _open(self):
        self._file = open(self._filename, 'w+')
        self._file.truncate()

    def _close(self):
        self._file.close()
