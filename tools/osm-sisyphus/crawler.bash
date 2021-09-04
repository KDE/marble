#!/bin/bash

# SPDX-License-Identifier: LGPL-2.1-or-later
#
# SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
#

#
# Retrieves all .osm.pbf links from download.geofabrik.de
# To be used to generate input for regions.bash
#

BASE="http://download.geofabrik.de/osm/"

function descend()
{
  wget "${1}" -O - | grep href | sed -e 's/^.*a href=\"//g' -e 's/\">.*<\/a>.*$//g' | grep -v http | grep -v html | grep -v .shp.zip | grep -v .osm.bz2 | sort | uniq | while read item
  do
    if [[ "${item}" == */ && "${item}" != /* ]]
    then
      sleep 3.0 # Avoid being punished by geofabrik.de server
      descend "${1}${item}"
    elif [[ "${item}" == *.osm.pbf ]]
    then
      echo "${1}${item}" | sed "s@${BASE}@@"
    fi
  done
}

descend "${BASE}" | sed 's@//@/@g' | sort | uniq
