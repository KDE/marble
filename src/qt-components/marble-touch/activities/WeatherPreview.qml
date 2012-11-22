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
    height: 48
    width: 140

    property bool isActive: count > 0

    clip: true
    keyNavigationWraps: true

    model: marbleWidget.renderPlugin("weather").favoritesModel
    delegate: Item {
        width: weatherPreview.width
        height: weatherPreview.height

        Text {
            id: temperatureLabel
            font.pixelSize: 24
            text: temperature + " °C"
        }

        Image {
            id: icon
            anchors.left: temperatureLabel.right
            anchors.leftMargin: 10
            width: 32
            height: 32
            fillMode: Image.PreserveAspectFit
            source: image !== "" ? ("file://" + image) : ""
        }

        Text {
            anchors.bottom: parent.bottom
            font.pixelSize: 14
            text: station
            width: weatherPreview.width
        }
    }

    Timer {
        interval: 2 * 1000
        repeat: true
        running: true
        onTriggered: parent.incrementCurrentIndex()
    }
}
