// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Dennis Nienhüser <nienhueser@kde.org>
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
