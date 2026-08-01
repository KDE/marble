// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import org.kde.kirigamiaddons.components as Components

Components.FloatingButton {
    id: root

    property real angle: 0
    property alias showDirection: indicator.visible

    Image {
        id: indicator
        anchors {
            horizontalCenter: root.horizontalCenter
            verticalCenter: root.verticalCenter
        }
        width: root.implicitContentWidth
        height: root.implicitContentHeight
        source: Qt.resolvedUrl("images/backdrop.png")
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
}
