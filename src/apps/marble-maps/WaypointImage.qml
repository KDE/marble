// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

import QtQuick
import QtQuick.Window

import org.kde.marble

Item {
    id: root

    // Can be 'departure', 'waypoint', 'destination', 'searchResult'
    property alias type: image.state

    signal clicked()

    width: Screen.pixelDensity * 6
    height: width

    Image {
        id: image
        anchors.fill: parent

        states: [
            State {
                name: "departure"
                PropertyChanges {
                    target: image
                    source: "qrc:///ic_place_departure.png"
                }
            },

            State {
                name: "waypoint"
                PropertyChanges {
                    target: image
                    source: "qrc:///ic_place_via.png"
                }
            },

            State {
                name: "destination"
                PropertyChanges {
                    target: image
                    source: "qrc:///ic_place_arrival.png"
                }
            },

            State {
                name: "searchResult"
                PropertyChanges {
                    target: image
                    source: "qrc:///ic_place.png"
                }
            }
        ]

        MouseArea {
            id: touchArea
            anchors.fill: parent
            onClicked: root.clicked()
        }
    }
}
