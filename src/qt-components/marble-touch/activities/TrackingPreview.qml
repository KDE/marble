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
    width: childrenRect.width
    height: 48

    property bool isActive: marbleWidget.tracking.distance > 0

    Label {
        anchors.verticalCenter: parent.verticalCenter
        text: (marbleWidget.tracking.distance / 1000).toFixed(1) + " km"
    }
}
