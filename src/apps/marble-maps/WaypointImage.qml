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
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2

import org.kde.edu.marble 0.20

Item {
    id: root

    // Can be 'departure', 'waypoint' and 'destination'
    property alias type: image.state

    width: Screen.pixelDensity * 5
    height: width

    Image {
        id: image
        anchors.fill: parent

        states: [
            State {
                name: "departure"
                PropertyChanges {
                    target: image
                    source: "qrc:///place_green.png"
                }
            },

            State {
                name: "waypoint"
                PropertyChanges {
                    target: image
                    source: "qrc:///place_orange.png"
                }
            },

            State {
                name: "destination"
                PropertyChanges {
                    target: image
                    source: "qrc:///place_red.png"
                }
            }
        ]

    }
}
