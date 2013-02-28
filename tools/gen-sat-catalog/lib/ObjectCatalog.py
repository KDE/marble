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

from lxml import etree
from lxml.builder import ElementMaker

from SpaceObject import SpaceObject

class ObjectCatalog(object):

    def __init__(self, filename, baseURL):
        super(ObjectCatalog, self).__init__()
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
 
