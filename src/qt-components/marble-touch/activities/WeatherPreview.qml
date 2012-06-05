// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11
import org.kde.edu.marble.qtcomponents 0.12

ListView {
    id: weatherPreview
    width: 120
    height: 120

    property bool isActive: count > 0

    clip: true
    orientation: ListView.Horizontal
    keyNavigationWraps: true

    model: marbleWidget.renderPlugin("weather").favoritesModel
    delegate: Item {
        width: weatherPreview.width
        height: weatherPreview.height

        Image {
            id: icon
            anchors.centerIn: parent
            width: 92
            fillMode: Image.PreserveAspectFit
            source: image !== "" ? ("file://" + image) : ""
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            font.bold: true
            font.pixelSize: 24
            text: temperature + " °C"
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
            font.bold: true
            text: station
            width: 120
        }
    }

    Timer {
        interval: 20 * 1000
        repeat: true
        running: true
        onTriggered: parent.incrementCurrentIndex()
    }
}
