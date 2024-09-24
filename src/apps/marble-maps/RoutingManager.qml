// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

import QtQuick

import org.kde.marble

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
