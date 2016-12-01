//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

import org.kde.marble 0.20

Item {
    id: root

    property var map
    property var navigationManager

    onNavigationManagerChanged: navigationModeSwitch.checked = navigationManager ? navigationManager.guidanceModeEnabled : false

    height: visible ? Screen.pixelDensity * 4 + column.height : 0

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        anchors.fill: parent
        color: palette.base
    }

    Column {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Screen.pixelDensity * 2
        spacing: Screen.pixelDensity * 2

        Text {
            font.pointSize: 18
            text: root.map ? (root.map.speed * 3.6).toFixed(1) + " km/h – " + root.map.currentPosition.coordinates : ""
        }

        Row {
            id: row
            width: root.width
            spacing: Screen.pixelDensity * 2

            Switch {
                id: navigationModeSwitch
                anchors.verticalCenter: parent.verticalCenter
                onClicked: {
                    if (root.navigationManager) {
                        root.navigationManager.guidanceModeEnabled = checked
                    }
                }
            }

            Text {
                width: row.width - row.spacing - navigationModeSwitch.width
                height: navigationModeSwitch.height
                verticalAlignment: Text.AlignVCenter
                text: qsTr("Follow Current Position")
                font.pointSize: 18
            }
        }
    }
}
