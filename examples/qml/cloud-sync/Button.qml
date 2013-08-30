//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

import QtQuick 1.0

Rectangle {
    id: container

    property alias label: buttonLabel.text
    property alias font: buttonLabel.font
    property alias color: buttonLabel.color
    property color tint: "#FFFFFFFF"
    signal clicked

    width: buttonLabel.width + 20; height: buttonLabel.height + 6
    smooth: true
    border { width: 1; }
    radius: 8

    gradient: Gradient {
        GradientStop {
            position: 0.0
        }
        GradientStop { position: 1.0; }
    }

    MouseArea { id: mouseArea; anchors.fill: parent; onClicked: container.clicked() }

    Text {
        id: buttonLabel; text: container.label; anchors.centerIn: container
    }
}
