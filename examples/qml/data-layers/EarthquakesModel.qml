// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.1

XmlListModel {
    property real north
    property real south
    property real east
    property real west

    function update() {
      source =  "http://ws.geonames.org/earthquakes?style=full"
                 + "&north=" + north
                 + "&south=" + south
                 +  "&east=" + east
                 +  "&west=" + west
    }

    query: "/geonames/earthquake"

    XmlRole { name: "lat"; query: "lat/number()" }
    XmlRole { name: "lon"; query: "lng/number()" }
    XmlRole { name: "magnitude"; query: "magnitude/number()" }

    // You can also activate those properties:
    // XmlRole { name: "source"; query: "src/string()" }
    // XmlRole { name: "eqid"; query: "eqid/string()" }
    // XmlRole { name: "datetime"; query: "datetime/string()" }
    // XmlRole { name: "depth"; query: "depth/number()" }
}
