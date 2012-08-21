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

# set the target filename
filename = 'planetarysats.txt'

# define all interesting objects
#            name               planet      planet_id (horizons)
objects = [ ('Mars Express',    'Mars',     '499'),
            ('Venus Express',   'Venus',    '299') ]

## request data via HorizonsClient
currentTime = time.gmtime(time.time())
client = HorizonsClient()
client.connect()
for obj in objects:
    client.getStateVector(obj[0], obj[1],
        obj[2],time.strptime("{0:04d} {1:02d} {2:02d}"
        .format(currentTime.tm_year, currentTime.tm_mon, currentTime.tm_mday),
        '%Y %m %d'))
client.disconnect()

## write to file
print("Writing data to {}...".format(filename))
f = open(filename, 'w+')
for entry in client.data:
    print("  Writing entry for {0}...".format(entry[0]))
    f.write("#{0}\n".format(entry[0]))
    f.write("{0}\n".format(entry[1]))
    f.write("{year:04d} {month:02d} {mday:02d} {hours} {mins} {secs}.000 0\n"
            .format(year=entry[2].tm_year, month=entry[2].tm_mon,
                    mday=entry[2].tm_mday, hours=entry[2].tm_hour,
                    mins=entry[2].tm_min,  secs=entry[2].tm_sec))
    f.write("{0} {1} {2}\n" .format(*entry[3]))
    f.write("{0} {1} {2}\n" .format(*entry[4]))
f.close()
print("Done.")

