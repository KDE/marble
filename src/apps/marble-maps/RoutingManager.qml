// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.marble

Routing {
    id: root

    required property MainScreen app
    required property MarbleItem marbleItem
    required property PlacemarkDialog placemarkDialog

    marbleMap: marbleItem.marbleMap
    waypointDelegate: Waypoint {
        id: waypoint
        visible: false
        app: root.app
        placemarkDialog: root.placemarkDialog
    }
    onRoutingProfileChanged: { updateRoute(); }
}
