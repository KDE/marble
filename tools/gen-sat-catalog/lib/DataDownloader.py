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

from SpaceObjectCatalog import HorizonsSpaceObject, TASCSpaceObject

import sys
import telnetlib
import time
import httplib
import urllib
import calendar
import re


class HorizonsDownloader(object):

    """A simple telnet client for the horizons system."""

    _body_ids = {
        'Mercury'   : 199,
        'Venus'     : 299,
        'Earth'     : 399,
        'Mars'      : 499,
        'Jupiter'   : 599,
        'Saturn'    : 699,
        'Uranus'    : 799,
        'Neptune'   : 899,
        }

    _commands = { 
        re.escape("$$SOE"): 'NoResponse',
        re.escape("$$EOE"): 'Data',
        re.escape("Horizons> "): 'Done',
        re.escape("Continue [ <cr>=yes, n=no, ? ] : "): "n",
        re.escape("[E]phemeris, [F]tp, [M]ail, [R]edisplay, ?, <cr>: "): "e",
        re.escape("Observe, Elements, Vectors  [o,e,v,?] : "): "v",
        re.escape("Coordinate center [ <id>,coord,geo  ] : "): "500@{body_id}",
        re.escape("Confirm selected station    [ y/n ] --> "): "y",
        re.escape("Reference plane [eclip, frame, body ] : "): "frame",
        re.escape("Output interval [ex: 10m, 1h, 1d, ? ] : "): "{interval_days}d",
        re.escape("Accept default output [ cr=(y), n, ?] : "): "n",
        re.escape("Output reference frame [J2000, B1950] : "): "J2000",
        re.escape("Corrections [ 1=NONE, 2=LT, 3=LT+S ]  : "): "1",
        re.escape("Output units [1=KM-S, 2=AU-D, 3=KM-D] : "): "1",
        re.escape("Spreadsheet CSV format    [ YES, NO ] : "): "YES",
        re.escape("Label cartesian output    [ YES, NO ] : "): "NO",
        re.escape("Select output table type  [ 1-6, ?  ] : "): "2",
        re.escape("[A]gain, [N]ew-case, [F]tp, [K]ermit, [M]ail, [R]edisplay, ? : "): "n",
        re.escape("Use previous center  [ cr=(y), n, ? ] : "): "n",
        re.compile("Starting CT .* : "): "{datetimestart}",
        re.compile("Ending   CT .* : "): "{datetimeend}",
        }

    def __init__(self):
        super(HorizonsDownloader, self).__init__()
        self._host = "horizons.jpl.nasa.gov"
        self._port = 6775
        self._prompt = "Horizons>"
        self._connection = None
        self._debug = False
        self._last_command_index = None

    def connect(self):
        self._connection = telnetlib.Telnet(self._host, self._port)
        self._read_until_prompt()

    def disconnect(self):
        self._connection.close()

    def download_state_vectors(self, hspace_obj, t_srt=None, t_end=None):
        if self._connection is None:
            print("Not connected!")
            return
        print("Requesting data for {0} relative to {1}..." .format(
            hspace_obj.name, hspace_obj.related_body))
        self._send(hspace_obj.horizons_id)

        if t_srt is None:
            t_srt = hspace_obj.data_from
        if t_end is None or t_end <= t_srt:
            t_end = hspace_obj.data_until

        body_id = self._body_ids[hspace_obj.related_body]
        
        cmds = self._commands.keys()
        resp = [y.format(body=hspace_obj.related_body,
                         body_id=body_id,
                         datetimestart=self._unixtime_to_str(t_srt),
                         datetimeend=self._unixtime_to_str(t_end),
                         interval_days=hspace_obj.data_interval_days)
                for y in self._commands.values()]
        
        done = False
        vectors = []

        while(not done):
            r, data = self._next_command(cmds, resp)
            if r == 'Done':
                done = True
            elif r == 'Data':
                print("  Found... Parsing...")
                vectors = self._parse_data(data)
                print("  Success")
            elif r == 'NoResponse':
                pass
            else:
                self._send(r)

        return vectors

    def _next_command(self, cmds, resp):
        idx, match, data = self._connection.expect(cmds, 120)
        if self._debug:
            print(data)
        if self._last_command_index == idx:
            print("Horizons repeated the last message:")
            print(data)
            print("Probably something went wrong. Aborting!")
            raise Exception()
        else:
            self._last_command_index = idx
        return (resp[idx], data)

    def _unixtime_to_str(self, timestamp):
        return time.strftime('%Y-%m-%d %H:%M', time.gmtime(timestamp))
 
    def _jdate_to_unixtime(self, jstamp):
        return (jstamp - 2440587.5) * 86400

    def _parse_data(self, data):
        sdat = data.split(',')
        if len(sdat) < 1:
            raise Exception()
        vectors = []
        for i in range(0, len(sdat) - 1, 8):
            ldat = sdat[i:i+8]
            vectors.append( [ float(ldat[0]),
                     float(ldat[2]), float(ldat[3]), float(ldat[4]),
                     float(ldat[5]), float(ldat[6]), float(ldat[7]) ] )
        return vectors

    def _send(self, data):
        self._connection.write("{0}\n".format(data))

    def _read_until_prompt(self):
        self._connection.read_until(self._prompt)


class TASCDownloader(object):

    """A simple http client for ESA's TASC service at http://tasc.esa.int/."""

    _params = {
        'mission'       : "{mission}",
        'querytyp'      : "data",
        'qtyp'          : "sta",
        'staobj'        : "{object_name}",
        'starefobj'     : "{related_body}",
        'staltc'        : "NO",
        'stafrm'        : "mean equatorial J2000",
        'tscl'          : "TDB",
        'tstart'        : "{t_start}",
        'tend'          : "{t_end}",
        'tstp'          : "{interval_days:03d} 00:00:00.000",
        'orbtyp'        : "ops",
    }

    def __init__(self):
        super(TASCDownloader, self).__init__()
        self._host = "tasc.esa.int"
        self._port = 80
        self._uri  = '/cgi-bin/query.html'
        self._debug = False

    def download_state_vectors(self, space_obj, t_start=None, t_end=None):
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


class DataDownloader(object):

    def __init__(self):
        super(DataDownloader, self).__init__()
        self._horizons = HorizonsDownloader()
        self._horizons.connect()
        self._tasc = TASCDownloader()

    def __del__(self):
        self._horizons.disconnect()

    def download_state_vectors(self, space_obj):
        if isinstance(space_obj, HorizonsSpaceObject):
            return self._horizons.download_state_vectors(space_obj)
        elif isinstance(space_obj, TASCSpaceObject):
            return self._tasc.download_state_vectors(space_obj)
        else:
            raise NotImplementedError(space_obj.__class__.__name__)
