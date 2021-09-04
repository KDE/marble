// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//


import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

Item {
    id: root

    width: image.width + 10
    height: image.height + 10

    property alias imageSource: image.source
    signal clicked()

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: touchArea.pressed ? palette.button : palette.highlight

        Image {
            id: image
            anchors.centerIn: parent
            anchors.margins: 10

            width: Screen.pixelDensity * 6
            height: width
            fillMode: Image.PreserveAspectFit

            MouseArea {
                id: touchArea
                anchors.fill: parent
                onClicked: {
                    if (root.enabled) {
                        root.clicked();
                    }
                }
            }
        }
    }
}
