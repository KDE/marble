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

import sys
import string

class ObjectCatalogue(object):

    def __init__(self, filename, baseURL):
        super(ObjectCatalogue, self).__init__()
        self._filename = filename;
        self._baseURL = baseURL
        self._file = None
        self._open()

    def __del__(self):
        self._close()

    def add_object(self, name, planet, missionStart, missionEnd, dataFile):
        url = self._baseURL + "/" + dataFile
        if(missionEnd is None):
            missionEnd = ''
        e = string.join([name, planet, str(missionStart), str(missionEnd), url], ', ')
        self._file.write(e + "\n")

    def _open(self):
        self._file = open(self._filename, 'w+')
        self._file.truncate()

    def _close(self):
        self._file.close()
 
