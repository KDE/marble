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

from HorizonsClient import HorizonsClient
import time

def now():
    t = time.gmtime(time.time())
    return "{0:04d}-{1:02d}-{2:02d}".format(t.tm_year, t.tm_mon, t.tm_mday)

# define all interesting objects
#            name, planet, planet_id, start date, end date
objects = [ ('Mars Express', 'Mars', '499', '2003-12-23', now()),
            ('Venus Express', 'Venus', '299', '2006-04-11', now()) ]

## request data via HorizonsClient
currentTime = time.gmtime(time.time())
client = HorizonsClient()
client.connect()
for obj in objects:
    data = client.getStateVector(*obj)
    print("  Writing data to file...");
    f = open("{0}.txt".format(obj[1]), 'a+')
    f.write("# {0} ({1} to {2})\n".format(obj[0], obj[3], obj[4]))
    f.write(data)
    f.write("\n")
    f.close()
    print("  Done.")
client.disconnect()

