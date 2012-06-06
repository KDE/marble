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
    id: trackingPreview
    width: 120
    height: 120

    property bool isActive: marbleWidget.tracking.distance > 0

    Label {
        anchors.fill: parent
        anchors.margins: 10
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter

        property string speed: Math.round( marbleWidget.tracking.positionSource.speed ) + " km/h"
        property string distance: (marbleWidget.tracking.distance / 1000 ).toFixed(1) + " km"

        text: speed + "<br />" + distance
    }
}
