// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>

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
