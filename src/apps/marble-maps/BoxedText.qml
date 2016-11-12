//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//

import QtQuick 2.3
import QtQuick.Window 2.2

Item {
    id: root
    width: 1.1 * label.width
    height: 1.1 * label.height
    property alias color: background.color
    property alias textColor: label.color
    property alias text: label.text

    Rectangle {
        id: background
        anchors.fill: parent
        radius: Screen.pixelDensity*0.5
        color: "black"
        opacity: 0.63
    }

    Text {
        id: label
        color: "white"
        anchors {
            centerIn: background
        }
    }
}
