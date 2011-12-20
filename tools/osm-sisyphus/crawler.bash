#!/bin/bash

#
# This file is part of the Marble Virtual Globe.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
#

#
# Retrieves all .osm.pbf links from download.geofabrik.de
# To be used to generate input for regions.bash
#

BASE="http://download.geofabrik.de/osm/"

function descend()
{
  wget -q "${1}" -O - | grep href | sed -e 's/^.*a href=\"//g' -e 's/\">.*<\/a>.*$//g' | grep -v http | grep -v html | grep -v .shp.zip | grep -v .osm.bz2 | sort | uniq | while read item
  do
    if [[ "${item}" == */ ]]
    then
      sleep 1.5 # Avoid being punished by geofabrik.de server
      descend "${1}${item}"
    elif [[ "${item}" == *.osm.pbf ]]
    then
      echo "${1}${item}" | sed "s@${BASE}@@"
    fi
  done
}

descend "${BASE}" | sed 's@//@/@g' | sort | uniq
