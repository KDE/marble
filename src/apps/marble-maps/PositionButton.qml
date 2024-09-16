// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//


import QtQuick
import QtQuick.Controls
import QtQuick.Window

Item {
    id: root

    property alias diameter: button.radius
    property alias iconSource: button.icon.source
    property real angle: 0
    property alias showDirection: indicator.visible

    signal clicked()

    Image {
        id: indicator
        anchors {
            horizontalCenter: button.horizontalCenter
            verticalCenter: button.verticalCenter
        }
        width: root.diameter
        height: root.diameter
        source: "qrc:///backdrop.png"
        transform: Rotation {
            origin {
                x: indicator.width / 2
                y: indicator.height / 2
            }
            axis {
                x: 0
                y: 0
                z: 1
            }
            angle: root.angle + 45
        }
    }

    RoundButton {
        id: button
        onClicked: root.clicked()
        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
    }

    width: 1.41 * diameter
    height: width
}
