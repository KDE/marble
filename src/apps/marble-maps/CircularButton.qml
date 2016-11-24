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
    property bool highlight: false

    signal clicked()

    Rectangle {
        id: button
        width: root.diameter
        height: root.diameter
        radius: root.diameter / 2

        property color idleColor: root.highlight ? palette.highlight : palette.button
        property color activeColor: root.highlight ? palette.button : palette.highlight
        color: touchHandler.pressed && root.enabled ? activeColor : idleColor

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
            sourceSize.width: 0.6 * root.diameter
            opacity: root.enabled ? 1 : 0.5
        }

        MouseArea {
            id: touchHandler
            anchors.fill: parent
            onClicked: root.clicked()
        }

        border {
            width: Screen.pixelDensity * 0.2
            color: palette.shadow
        }

        SystemPalette{
            id: palette
            colorGroup: SystemPalette.Active
        }
    }

    width: diameter
    height: diameter
}
