//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

Item {
    id: root
    height: column.height + Screen.pixelDensity * 4

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
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: Screen.pixelDensity * 2
        }

        spacing: Screen.pixelDensity * 2

        Text {
            id: text
            text: "Developer Settings"
        }

        CheckBox {
            text: "Show Render Performance"
            onCheckedChanged: marbleMaps.setShowRuntimeTrace(checked)
        }

        CheckBox {
            text: "Show OSM Bitmap Tiles"
            checked: true
            onCheckedChanged: marbleMaps.setPropertyEnabled("mapnik", checked)
        }

        Button {
            text: "Close"
            onClicked: root.visible = false
        }
    }
}
