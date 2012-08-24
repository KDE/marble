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
import telnetlib
import time
import re

"""
A simple telnet client for the horizons system.
"""
class HorizonsClient(object):

    def __init__(self):
        super(HorizonsClient, self).__init__()
        self._host = "horizons.jpl.nasa.gov"
        self._port = 6775
        self._prompt = "Horizons>"
        self._connection = None
        self._debug = False
        self._body_ids = {
            'Mercur'    : 199,
            'Venus'     : 299,
            'Earth'     : 399,
            'Mars'      : 499,
            'Jupiter'   : 599,
            'Saturn'    : 699,
            'Uranus'    : 799,
            'Neptun'    : 899 }
        self._last_command_index = None
        self._commands = { 
            re.escape("$$SOE"): self.NORESPONSE,
            re.escape("$$EOE"): self.DATA,
            re.escape("Horizons> "): self.DONE,
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
            re.compile("Ending   CT .* : "): "{datetimeend}" }

    def connect(self):
        self._connection = telnetlib.Telnet(self._host, self._port)
        self._read_until_prompt()

    def disconnect(self):
        self._connection.close()

    """
    Request state vectors for space_obj between t_start, t_end
    """
    def get_state_vectors_for_object(self, space_obj,
                                     t_start=None, t_end=None):
        if(self._connection is None):
            print("Not connected!")
            return
        print("Requesting data for {0} relative to {1}..." .format(
            space_obj.name, space_obj.related_body))
        self._send(space_obj.horizons_id)

        if(t_start is None):
            t_start = time.time() - (time.time() % (3600*24)) + 1
        datetime_start = self._unixtime_to_str(t_start)
        if(t_end is None or t_end <= t_start):
            t_end = t_start + 60
        datetime_end   = self._unixtime_to_str(t_end)

        body_id = self._body_ids[space_obj.related_body]
        
        cmds = self._commands.keys()
        resp = [y.format(body=space_obj.related_body,
                         body_id=body_id,
                         datetimestart=datetime_start,
                         datetimeend=datetime_end,
                         interval_days=space_obj.data_interval_days)
                for y in self._commands.values()]
        
        done = False
        vectors = []

        while(not done):
            r, data = self._next_command(cmds, resp)
            if(r == self.DONE):
                done = True
            elif(r == self.DATA):
                print("  Found... Parsing...")
                vectors = self._parse_data(data)
                print("  Success")
            elif(r == self.NORESPONSE):
                pass
            else:
                self._send(r)

        return vectors

    def _next_command(self, cmds, resp):
        idx, match, data = self._connection.expect(cmds, 120)
        if(self._debug):
            print(data)
        if(self._last_command_index == idx):
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
        if(len(sdat) < 1):
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

    @property
    def DATA(self):
        return "Data"

    @property
    def NORESPONSE(self):
        return "NoResponse"

    @property
    def DONE(self):
        return "Done"

