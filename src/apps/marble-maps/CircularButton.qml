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
import QtQuick.Controls.Styles 1.3
import QtQuick.Window 2.2

Item {
    id: root

    readonly property int diameter: Screen.pixelDensity * 9
    property alias iconSource: icon.source

    signal clicked()

    Rectangle {
        id: button
        width: root.diameter
        height: root.diameter
        radius: root.diameter / 2

        anchors {
            horizontalCenter: root.horizontalCenter
            verticalCenter: root.verticalCenter
        }

        Text {
            id: text
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: palette.buttonText
        }

        Image {
            id: icon
            anchors {
                horizontalCenter: button.horizontalCenter
                verticalCenter: button.verticalCenter
            }
            fillMode: Image.PreserveAspectFit
            width: 0.6 * root.diameter
        }

        MouseArea {
            id: touchHandler
            anchors.fill: parent
            onClicked: root.clicked()
            onPressedChanged: {button.color = pressed ? palette.highlight : palette.button;}
        }

        border {
            width: Screen.pixelDensity * 0.2
            color: palette.shadow
        }

        SystemPalette{
            id: palette
            colorGroup: SystemPalette.Active
        }

        color: palette.button
    }

    width: diameter
    height: diameter
}
