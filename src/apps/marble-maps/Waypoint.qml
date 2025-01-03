// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

import QtQuick
import QtQuick.Window

import org.kde.marble

Item {
    id: root

    property alias type: image.type
    property int xPos: 0
    property int yPos: 0
    property var placemark: null
    property int index: -1

    width: Screen.pixelDensity * 15
    height: width
    x: xPos - 0.5 * width
    y: yPos - 0.5 * height

    WaypointImage {
        id: image
        onClicked: {
            if (type == "searchResult") {
                if (placemarkDialog.placemark === placemark) {
                    placemarkDialog.placemark = null
                    app.state = "none"
                } else {
                    placemarkDialog.placemark = placemark
                    app.state = "place"
                }
            } else {
                app.currentWaypointIndex = index
                app.state = "route"
            }
        }
        anchors {
            bottom: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }

        x: 0.5 * parent.width - 0.5 * width
        y: 0.5 * parent.height - height
    }
}
