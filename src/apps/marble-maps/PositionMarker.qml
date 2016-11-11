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

    property alias angle: rotation.angle
    property int posX: 0
    property int posY: 0
    property bool showAccuracy: true
    property real radius: 100

    x: posX - width * 0.5
    y: posY - height * 0.5

    width: radius
    height: radius

    Rectangle {
      anchors.fill: parent
      color: "#40ff0000"
      border.color: "#ff0000"
      border.width: 2
      radius: root.radius
    }

    Image {
        id: image
        source: "qrc:///navigation_blue.svg"
        sourceSize.width: width
        sourceSize.height: height
        width: Screen.pixelDensity * 6
        height: width
        anchors.centerIn: parent
        smooth: true
        transform: Rotation {
            id: rotation
            origin {
                x: image.width * 0.5
                y: image.height * 0.5
            }
        }
    }
}
