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

from SpaceObject import SpaceObject

import sys
import httplib
import urllib
import time
import calendar
import re

"""
A simple http client for ESA's TASC service at http://tasc.esa.int/
"""
class TASCClient(object):

    def __init__(self):
        super(TASCClient, self).__init__()
        self._host = "tasc.esa.int"
        self._port = 80
        self._uri  = '/cgi-bin/query.html'
        self._debug = False
        self._params = {
            'mission':      "{mission}",
            'querytyp':     "data",
            'qtyp':         "sta",
            'staobj':       "{object_name}",
            'starefobj':    "{related_body}",
            'staltc':       "NO",
            'stafrm':       "mean equatorial J2000",
            'tscl':         "TDB",
            'tstart':       "{t_start}",
            'tend':         "{t_end}",
            'tstp':         "{interval_days:03d} 00:00:00.000",
            'orbtyp':       "ops",
        }

    """
    Request state vectors for space_obj between t_start, t_end
    """
    def get_state_vectors_for_object(self, space_obj,
                                     t_start=None, t_end=None):
        print("Requesting data for {0} relative to {1}..." .format(
            space_obj.name, space_obj.related_body))

        if t_start is None:
            t_start = space_obj.data_from
        if t_end is None or t_end <= t_start:
            t_end = space_obj.data_until

        start = time.strftime('%Y/%m/%d %H:%M:%S.000', time.gmtime(t_start))
        end = time.strftime('%Y/%m/%d %H:%M:%S.000', time.gmtime(t_end))
        interval_days = 30

        params = []
        for key in self._params.keys():
            value = urllib.quote_plus(self._params[key].format(
                mission=space_obj.tasc_mission,
                object_name=space_obj.name,
                related_body=space_obj.related_body,
                t_start=start,
                t_end=end,
                interval_days=interval_days))
            params = params + [ key, value ]

        data = self._fetch_from_http(params)
        print("  Parsing...")
        return self._parse(data)

    def _fetch_from_http(self, params):
        con = httplib.HTTPConnection(self._host, self._port)
        p = []
        for i in range(0, len(params), 2):
            p = p + [ params[i] + "=" + params[i+1] ]
        uri = self._uri + '?' + '&'.join(p)
        if self._debug:
            print("Request:", uri)
        con.request('GET', uri)
        response = con.getresponse()
        if response.status != 200:
            print("Failed to load data via HTTP:",
                    response.status, response.reason)
            return ""
        return response.read()

    def _parse(self, data):
        if self._debug:
            print(data)

        dl = [x for x in data.split("\n")
            if x != "" and x.find("#") == -1 and x.find("<") == -1]

        if len(dl) < 1:
            return []

        vecs = []
        for line in dl:
            fields = line.split()
            if len(fields) != 7:
                print("Invalid data line:", line)
                continue;
            vec = [ self._TDB_to_JD(fields[0]),
                float(fields[1]), float(fields[2]), float(fields[3]),
                float(fields[4]), float(fields[5]), float(fields[6])]
            vecs.append(vec)
        return vecs

    def _TDB_to_JD(self, tdb):
        tdb = tdb.split(".")[0] # strip .000
        t = calendar.timegm(time.strptime(tdb, '%Y/%m/%dT%H:%M:%S'))
        return t / 86400 + 2440587.5

