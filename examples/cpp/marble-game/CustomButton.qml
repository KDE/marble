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

Button {
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

    text: labelText

    style: ButtonStyle {
        background: Rectangle {
            implicitWidth: control.buttonWidth
            implicitHeight: control.buttonHeight
            border.width: control.hovered ? 2 : 1
            border.color: control.hovered ? control.onHoverColor : control.borderColor
            color: control.pressed ? Qt.darker( control.normalColor, 1.5 ) : control.normalColor
            Behavior on color { ColorAnimation{ duration: 50 } }
            radius: 6
            scale: control.pressed ? 1.1 : 1.0
            Behavior on scale { NumberAnimation{ duration: 50 } }
        }

        label: Text {
            font.pixelSize: labelSize
            color: labelColor
            text: labelText
            wrapMode: Text.WordWrap
        }
    }
    onClicked: {
        buttonClick()
    }
}

