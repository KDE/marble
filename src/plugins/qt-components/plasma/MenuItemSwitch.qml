//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

import Qt 4.7
import QtQuick 1.1
import com.nokia.meego 1.0

Switch {
    id: root

    property alias switchText: root.text

    signal clicked

    checked: true

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: { if (parent.enabled) parent.clicked(); }
    }

    onClicked: checked = !checked
}
