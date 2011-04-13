// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>

// A (gps) tracking example. Shows the current (gps) position on the map
// using a small ghost image. The visibility of the track can be toggled.

import Qt 4.7

Rectangle {
    id: toggle

    property bool active: false
    property alias text: message.text

    signal toggled

    width: 100; height: 20
    radius: 5
    color: active ? "steelblue" : "gray"
    x: 30; y: 30

    Text {
        id: message
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: 10
        color: "white"
        text: ""
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            active = !active
            toggle.toggled()
        }
    }
}
