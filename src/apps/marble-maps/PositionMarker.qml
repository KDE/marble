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

    property real angle: 0
    property bool showAccuracy: true
    property real radius: 100
    property bool allowRadiusAnimation: true
    property bool allowPositionAnimation: true

    Behavior on radius {
        enabled: allowRadiusAnimation
        NumberAnimation { duration: 200 }
    }

    Behavior on angle {
        RotationAnimation {
          duration: 200
          direction: RotationAnimation.Shortest
        }
    }
    Behavior on x {
        enabled: allowPositionAnimation
        SmoothedAnimation { duration: 200 }
    }
    Behavior on y {
        enabled: allowPositionAnimation
        SmoothedAnimation { duration: 200 }
    }

    Rectangle {
        width: 2 * root.radius
        height: 2 * root.radius
        anchors.centerIn: parent
        visible: root.showAccuracy
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
        rotation: root.angle
    }
}
