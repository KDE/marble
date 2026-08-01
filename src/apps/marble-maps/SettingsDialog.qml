// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Window

import org.kde.ki18n
import org.kde.marble

Item {
    id: root

    required property MarbleItem marbleMaps

    height: column.height + Screen.pixelDensity * 4

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    Settings {
        id: settings

        Component.onDestruction: {
            settings.setValue("Network", "wlanOnly", wlanOnly.checked ? "true" : "false")
        }
    }

    Rectangle {
        anchors.fill: parent
        color: palette.base
    }

    Column {
        id: column
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: Screen.pixelDensity * 2
        }

        spacing: Screen.pixelDensity * 2

        Text {
            id: text
            text: KI18n.i18n("Network Settings")
        }

        CheckBox {
            id: wlanOnly
            text: KI18n.i18n("Download Maps via WLAN only")
            checked: settings.value("Network", "wlanOnly") === "true"
            onCheckedChanged: root.marbleMaps.wlanOnly = checked
        }
    }
}
