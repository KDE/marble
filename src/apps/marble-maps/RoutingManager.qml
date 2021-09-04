// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
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
