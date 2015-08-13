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
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

Item {
    id: root

    property string selectedProfile: "Car (fastest)"

    height: Screen.pixelDensity * 9 //Like circular button
    width: height * 3
    visible: false

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        radius: 0.5 * root.height
        anchors.fill: parent
        color: palette.base

        FloatingMenuButton {
            id: firsButton
            anchors {
                right: secondButton.left
                verticalCenter: parent.verticalCenter
            }

            imageSource: "qrc:///walk.png"

            onClicked: {selectedProfile = qsTr("Pedestrian"); root.focus = false}
        }

        FloatingMenuButton {
            id: secondButton
            anchors {
                right: thirdButton.left
                verticalCenter: parent.verticalCenter
            }

            imageSource: "qrc:///bike.png"

            onClicked: {selectedProfile = qsTr("Bicycle"); root.focus = false}
        }

        FloatingMenuButton {
            id: thirdButton
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            imageSource: "qrc:///car.png"

            onClicked: {selectedProfile = qsTr("Car (fastest)"); root.focus = false}
        }
    }
}
