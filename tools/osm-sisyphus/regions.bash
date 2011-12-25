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
# Creates a .xml file suitable to pass to osm-sisyphus
#

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
echo "<regions>"

cat "${1}" | while read pbf
do
    file="$(basename $pbf)"

    count=$(echo $pbf | awk -F"/" '{ print NF-1}')
    if [[ $count -eq 1 ]]
    then
      continent="$(dirname $pbf)"
      country=""
    elif [[ $count -eq 2 ]]
    then
      continent="$(dirname $(dirname $pbf))"
      country="$(basename $(dirname $pbf))"
    elif [[ $count -eq 3 ]]
    then
      continent="$(dirname $(dirname $(dirname $pbf)))"
      country="$(basename $(dirname $(dirname $pbf)))"
    fi

    name="${file/.osm.pbf/}"
    name="${name//-/ }"
    name="${name//_/ }"
    name=( $name )
    name="${name[@]^}"

    country="${country/_/ }"
    country=( ${country/_/ } )

    continent="${continent/us-west/usa}"
    continent="${continent/us-midwest/usa}"
    continent="${continent/us-northeast/usa}"
    continent="${continent/us-south/usa}"
    continent="${continent/us-pacific/usa}"

    if [[ "${continent}" == "usa" ]]
    then
      continent="north-america"
      country="USA"
      pbf="${pbf/us-west/north-america/us}"
      pbf="${pbf/us-midwest/north-america/us}"
      pbf="${pbf/us-northeast/north-america/us}"
      pbf="${pbf/us-south/north-america/us}"
      pbf="${pbf/us-pacific/north-america/us}"
    fi

    continent="${continent/-/ }"
    continent=( ${continent/-/ } )

    echo "  <region>"
    echo "    <id>${file/.osm.pbf/}</id>"
    echo "    <continent>${continent[@]^}</continent>"
    echo "    <country>${country[@]^}</country>"
    echo "    <name>${name}</name>"
    echo "    <path>${pbf/.osm.pbf/}</path>"
    echo "    <pbf>${pbf}</pbf>"
    echo "  </region>"
done

echo "</regions>"
