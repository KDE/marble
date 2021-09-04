// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>

import QtQuick 1.1

XmlListModel {
    property real north
    property real south
    property real east
    property real west

    function update() {
      source =  "http://api.geonames.org/earthquakes?username=marble&style=full"
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
