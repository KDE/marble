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
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import org.kde.edu.marble 0.20

Item {
    id: root

    property var model: []

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: palette.base

        ListView {
            id: instructionList
            anchors.fill: parent
            model: root.model
            delegate: Rectangle {
                id: item
                width: background.width

                color: palette.base

                Image {
                    id: sign
                    source: turnTypeIcon.replace(':/', 'qrc:///')

                    height: Screen.pixelDensity * 10
                    width: height

                    anchors.left: parent.left
                }

                Text {
                    anchors {
                        right: parent.right
                        left: sign.right
                    }
                    color: palette.text
                    wrapMode: Text.Wrap

                    text: display
                }

                height: sign.height
            }
        }
    }
}
