//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//

import QtQuick 2.3

import org.kde.marble 0.20

Routing {
    id: root

    property var marbleItem: null

    marbleMap: marbleItem.marbleMap
    waypointDelegate: Waypoint {visible: false}
    onRoutingProfileChanged: { updateRoute(); }

    function addSearchResultAsPlacemark(placemark)
    {
        if (marbleItem) {
            root.addSearchResultPlacemark(placemark);
        }
    }
}
