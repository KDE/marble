// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

import QtQuick 2.3
import QtQuick.Window 2.2

Item {
    id: root

    property alias imageSource: image.source
    signal clicked();

    width: Screen.pixelDensity * 9
    height: Screen.pixelDensity * 9

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle{
        id: background
        radius: 0.5 * root.height
        anchors.fill: parent
        color: touchArea.pressed ? palette.highlight : palette.base
        Image {
            id: image
            anchors {
                fill: parent
                margins: parent.height * 0.2
            }
        }
    }

    MouseArea{
        id: touchArea
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
