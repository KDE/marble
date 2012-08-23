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
import telnetlib
import time
import calendar
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
        self._planetIds = {
            'Mercur'    : 199,
            'Venus'     : 299,
            'Earth'     : 399,
            'Mars'      : 499,
            'Jupiter'   : 599,
            'Saturn'    : 699,
            'Uranus'    : 799,
            'Neptun'    : 899 }
        self._commands = [
                ( re.escape("$$SOE"), self.NORESPONSE ),
                ( re.escape("$$EOE"), self.DATA),
                ( re.escape("Horizons> "), self.DONE ),
                ( re.escape("Continue [ <cr>=yes, n=no, ? ] : "), "n" ),
                ( re.escape("[E]phemeris, [F]tp, [M]ail, [R]edisplay, ?, <cr>: "), "e" ),
                ( re.escape("Observe, Elements, Vectors  [o,e,v,?] : "), "v" ),
                ( re.escape("Coordinate center [ <id>,coord,geo  ] : "), "500@{planet_id}" ),
                ( re.escape("Confirm selected station    [ y/n ] --> "), "y" ),
                ( re.escape("Reference plane [eclip, frame, body ] : "), "frame" ),
                ( re.escape("Output interval [ex: 10m, 1h, 1d, ? ] : "), "{interval_days}d" ),
                ( re.escape("Accept default output [ cr=(y), n, ?] : "), "n" ),
                ( re.escape("Output reference frame [J2000, B1950] : "), "J2000" ),
                ( re.escape("Corrections [ 1=NONE, 2=LT, 3=LT+S ]  : "), "1" ),
                ( re.escape("Output units [1=KM-S, 2=AU-D, 3=KM-D] : "), "1" ),
                ( re.escape("Spreadsheet CSV format    [ YES, NO ] : "), "YES" ),
                ( re.escape("Label cartesian output    [ YES, NO ] : "), "NO" ),
                ( re.escape("Select output table type  [ 1-6, ?  ] : "), "2" ),
                ( re.escape("[A]gain, [N]ew-case, [F]tp, [K]ermit, [M]ail, [R]edisplay, ? : "), "n" ),
                ( re.escape("Use previous center  [ cr=(y), n, ? ] : "), "n" ),
                ( re.compile("Starting CT .* : "), "{datetimestart}"),
                ( re.compile("Ending   CT .* : "), "{datetimeend}")
            ]

    def connect(self):
        self._connection = telnetlib.Telnet(self._host, self._port)
        self._read_until_prompt()

    def disconnect(self):
        self._connection.close()

    def get_state_vector(self, objectName, planet, tStart, tEnd, intervalDays):
        if(self._connection is None):
            print("Not connect!")
            return
        print("Requesting data for {0} relative to {1}..."
              .format(objectName, planet))
        self._send(objectName)
        
        dateTimeStart = time.strftime("%Y-%m-%d %H:%M", time.gmtime(tStart))
        dateTimeEnd   = time.strftime("%Y-%m-%d %H:%M", time.gmtime(tEnd))

        planetId = self._planetIds[planet]
        
        cmds = [x for (x,y) in self._commands]
        resp = [y.format(planet=planet,
                         planet_id=planetId,
                         datetimestart=dateTimeStart,
                         datetimeend=dateTimeEnd,
                         interval_days=intervalDays) for (x,y) in self._commands]
        
        done = False
        parsed = ""

        while(not done):
            r, data = self._next_command(cmds, resp)
            if(r == self.DONE):
                done = True
            elif(r == self.DATA):
                print("  Found... Parsing...")
                parsed = self._parse_data(data)
                print("  Success")
            elif(r == self.NORESPONSE):
                pass
            else:
                self._send(r)

        return parsed

    def _next_command(self, cmds, resp):
        idx, match, data = self._connection.expect(cmds, 120)
        if(self._debug):
            print(data)
        return (resp[idx], data)
 
    def _jdate_to_unix(self, jstamp):
        return (jstamp - 2440587.5) * 86400

    def _parse_data(self, data):
        sdat = data.split(',')
        dstr = ""
        if(len(sdat) < 1):
            raise Exception()
        for i in range(0, len(sdat) - 1, 8):
            ldat = sdat[i:i+8]
            elms = [ ldat[0], str(float(ldat[2])), str(float(ldat[3])),
                              str(float(ldat[4])), str(float(ldat[5])),
                              str(float(ldat[6])), str(float(ldat[7])) ]
            dstr = dstr + ' '.join(elms).strip() + "\n"
        return dstr

    def _send(self, data):
        self._connection.write("{0}\n".format(data))

    def _read_until_prompt(self):
        self._connection.read_until(self._prompt)

    @property
    def data(self):
        return self._data

    @property
    def DATA(self):
        return "Data"

    @property
    def NORESPONSE(self):
        return "NoResponse"

    @property
    def DONE(self):
        return "Done"

