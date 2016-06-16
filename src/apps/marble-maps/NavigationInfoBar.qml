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
import QtQuick.Controls 1.3
import QtQuick.Window 2.2

Item {
    id: root

    property real distance: 0
    property alias instructionIcon: image.source

    width: parent.width
    height: iconBackground.height

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: textBackground
        color: palette.window
        anchors{
            left: iconBackground.right
            right: parent.right
        }
        height: distanceUntilInstruction.height * 2

        Text {
            id: distanceUntilInstruction
            text: root.distance > 1000 ? "%1 km".arg((0.001 * root.distance).toFixed(1)) : "%1 m".arg(distance > 100 ? (root.distance*0.01).toFixed(0)*100 : (root.distance*0.1).toFixed(0)*10)
            anchors.centerIn: parent
        }
    }

    Rectangle {
        anchors{
            left: parent.left
            right: textBackground.left
            top: textBackground.top
            bottom: textBackground.bottom
        }

        color: palette.window
    }

    Rectangle {
        id: iconBackground
        color: palette.window
        width: Screen.pixelDensity * 12.5
        height: width
        radius: 0.1 * width

        Image {
            id: image
            anchors.centerIn: parent
            width: parent.width * 0.8
            height: width
            sourceSize.height: height
            sourceSize.width: width
        }
    }
}
