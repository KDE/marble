// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 1.0

Rectangle {
    id: container

    property alias label: buttonLabel.text
    property alias color: buttonLabel.color
    signal clicked

    width: buttonLabel.width + 20;
    height: buttonLabel.height + 6
    smooth: true
    radius: 3
    border {
        width: 1
        color: "#aaaaaa"
    }

    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: mouseArea.pressed ? "#cccccc" : "#ffffff"
        }
        GradientStop {
            position: 1.0;
            color: "#cccccc"
        }
    }

    MouseArea {
        id: mouseArea;
        anchors.fill: parent;
        onClicked: container.clicked()
    }

    Text {
        id: buttonLabel;
        text: container.label;
        anchors.centerIn: container
    }
}
