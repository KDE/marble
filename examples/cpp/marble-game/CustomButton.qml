//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//


import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
    signal buttonClick()

    property int buttonWidth: 150
    property int buttonHeight: 75
    property string labelText: qsTr("Button")
    property color labelColor: "black"
    property color normalColor: "lightblue"
    property color onHoverColor: "crimson"
    property color borderColor: "transparent"

    property real labelSize: buttonWidth/10

    id: button
    width: buttonWidth
    height: buttonHeight
    border.width: 1
    border.color: borderColor
    radius: 6
    scale: clickArea.pressed ? 1.1 : 1.0
    color: clickArea.pressed ? Qt.darker( normalColor, 1.5 ) : normalColor

    Behavior on color { ColorAnimation{ duration: 50 } }
    Behavior on scale { NumberAnimation{ duration: 50 } }

    Text {
        id: buttonLabel

        text: labelText
        color: labelColor
        font.pixelSize: labelSize
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        anchors.verticalCenter: parent.verticalCenter
        wrapMode: Text.WordWrap
    }

    MouseArea {
        id: clickArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            buttonClick()
        }
        onEntered: {
            button.border.color = onHoverColor
            button.border.width = 2
        }
        onExited: {
            button.border.color = borderColor
            button.border.width = 1
        }
    }
}