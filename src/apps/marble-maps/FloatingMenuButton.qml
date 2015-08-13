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

    property alias imageSource: image.source
    signal clicked();

    width: Screen.pixelDensity * 9
    height: Screen.pixelDensity * 9

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle{
        id: background
        radius: 0.5 * root.height
        anchors.fill: parent
        color: touchArea.pressed ? palette.highlight : palette.base
        Image {
            id: image
            anchors {
                fill: parent
                margins: parent.height * 0.2
            }
        }
    }

    MouseArea{
        id: touchArea
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
