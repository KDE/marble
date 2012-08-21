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

from __future__ import print_function

import sys
import telnetlib
import time
import calendar
import string
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
        self._data = []
        self._connection = None
        self._debug = False
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
                ( re.escape("Output interval [ex: 10m, 1h, 1d, ? ] : "), "1h" ),
                ( re.escape("Accept default output [ cr=(y), n, ?] : "), "n" ),
                ( re.escape("Output reference frame [J2000, B1950] : "), "J2000" ),
                ( re.escape("Corrections [ 1=NONE, 2=LT, 3=LT+S ]  : "), "1" ),
                ( re.escape("Output units [1=KM-S, 2=AU-D, 3=KM-D] : "), "1" ),
                ( re.escape("Spreadsheet CSV format    [ YES, NO ] : "), "NO" ),
                ( re.escape("Label cartesian output    [ YES, NO ] : "), "NO" ),
                ( re.escape("Select output table type  [ 1-6, ?  ] : "), "2" ),
                ( re.escape("[A]gain, [N]ew-case, [F]tp, [K]ermit, [M]ail, [R]edisplay, ? : "), "n" ),
                ( re.escape("Use previous center  [ cr=(y), n, ? ] : "), "n" ),
                ( re.compile("Starting CT .* : "), "{datetimestart}"),
                ( re.compile("Ending   CT .* : "), "{datetimeend}")
            ]

    def connect(self):
        self._connection = telnetlib.Telnet(self._host, self._port)
        self._readUntilPrompt()

    def disconnect(self):
        self._connection.close()

    def getStateVector(self, object_name, planet, planetId, theTime=None):
        if(self._connection is None):
            print("Not connect!")
            return
        print("Requesting data for {0} relative to {1}..."
              .format(object_name, planet))
        self._send(object_name)
        
        if(theTime is None):
            theTime = time.gmtime(time.time())
        dateTimeStart = time.strftime("%Y-%m-%d %H:%M", theTime)
        theTime = time.gmtime(calendar.timegm(theTime) + 60)
        dateTimeEnd   = time.strftime("%Y-%m-%d %H:%M", theTime)
        
        cmds = [x for (x,y) in self._commands]
        resp = [y.format(planet=planet,
                         planet_id=planetId,
                         datetimestart=dateTimeStart,
                         datetimeend=dateTimeEnd) for (x,y) in self._commands]
        
        done = False
        while(not done):
            r, data = self._nextCommand(cmds, resp)
            if(r == self.DONE):
                done = True
            elif(r == self.DATA):
                print("  Found... Parsing...")
                parsed = self._parseData(data)
                self._data.append( ( object_name, planet,
                                     parsed[0], parsed[1], parsed[2] ) )
                print("  Success")
            elif(r == self.NORESPONSE):
                pass
            else:
                self._send(r)

    def _nextCommand(self, cmds, resp):
        idx, match, data = self._connection.expect(cmds, 30)
        if(self._debug):
            print(data)
        return (resp[idx], data)
 
    def _jdatetounix(self, jstamp):
        return (jstamp - 2440587.5) * 86400

    def _parseData(self, data):
        sdat = string.split(data)
        jdate = sdat[0]
        rx = string.atof(sdat[6])
        ry = string.atof(sdat[7])
        rz = string.atof(sdat[8])
        vx = string.atof(sdat[9])
        vy = string.atof(sdat[10])
        vz = string.atof(sdat[11])
        t = time.gmtime(self._jdatetounix(string.atof(jdate)))
        return ( t, (rx,ry,rz), (vx,vy,vz) )

    def _send(self, data):
        self._connection.write("{0}\n".format(data))

    def _readUntilPrompt(self):
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

