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
import QtQuick.Controls 1.3
import QtQuick.Window 2.2

Item {
    id: root

    property real distance: 0
    property real destinationDistance: 0
    property alias instructionIcon: instructionImage.source

    function formatDistance(distance)
    {
        if (distance > 1000) {
            return qsTr("%1 km").arg((0.001 * distance).toFixed(1))
        } else if (distance > 100) {
            return qsTr("%1 m").arg((distance*0.01).toFixed(0)*100)
        } else {
            return qsTr("%1 m").arg((distance*0.1).toFixed(0)*10)
        }
    }

    height: instructionImage.height

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        anchors.fill: parent
        color: palette.window
    }

    Item {
        id: nextInstructionItem
        anchors.verticalCenter: parent.verticalCenter

        Image {
            id: instructionImage
            anchors.verticalCenter: parent.verticalCenter
            width: Screen.pixelDensity * 15
            height: width
            sourceSize.height: height
            sourceSize.width: width
        }

        Text {
            id: distanceUntilInstruction
            anchors.left: instructionImage.right
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 24
            text: root.formatDistance(root.distance)
        }
    }

    Item {
        id: targetItem
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        Text {
            id: targetText
            anchors.right: targetImage.left
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 16
            text: root.formatDistance(root.destinationDistance)
        }

        Image {
            id: targetImage
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            width: Screen.pixelDensity * 10
            height: width
            source: "qrc:///ic_place_arrival.png"
        }
    }
}
