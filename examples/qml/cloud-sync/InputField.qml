//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Dennis Nienhüser <earthwings@gentoo.org>
//

import QtQuick 1.0

Rectangle {
    id: container
    width: buttonRow.width
    height: buttonRow.height

    property alias label: label.text
    property alias text: input.text
    property alias echoMode: input.echoMode

    signal accepted(string text)

    Row {
        id: buttonRow
        spacing: 10

        Text {
            id: label
            y: 2
            font.pointSize: 12
        }

        Rectangle {
            width: Math.max(100, input.width+20)
            height: input.height+4
            radius: 3
            smooth: true

            border {
                width: 1
                color: "#aaaaaa"
            }

            TextInput {
                id: input
                x: 2
                anchors.verticalCenter: parent.verticalCenter

                font.pointSize: 12
                onAccepted: container.accepted(text)
            }
        }
    }
}
