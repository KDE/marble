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

Item {
    id: navigationPreview
    width: 120
    height: 120

    property bool isActive: marbleWidget.tracking.hasLastKnownPosition &&
                            marbleWidget.routing.hasRoute &&
                            marbleWidget.navigation.destinationDistance > 0

    Image {
        anchors.bottom: distanceLabel.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 10
        source: "qrc:/flag.png"
        smooth: true
    }

    Label {
        id: distanceLabel
        anchors.bottom: parent.bottom
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: (marbleWidget.navigation.destinationDistance / 1000 ).toFixed(1) + " km"
    }
}
