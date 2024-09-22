// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts

import org.kde.marble

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

    ColumnLayout {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Screen.pixelDensity * 2
        spacing: Screen.pixelDensity * 2

        Label {
            text: root.map ? (root.map.speed * 3.6).toFixed(1) + " km/h – " + root.map.currentPosition.coordinates : ""
            Layout.fillWidth: true
        }

        Switch {
            id: navigationModeSwitch
            Layout.fillWidth: true
            onClicked: {
                if (root.navigationManager) {
                    root.navigationManager.guidanceModeEnabled = checked
                }
            }
            text: i18nc("@option:check", "Follow Current Position")
        }
    }
}
