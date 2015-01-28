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

RadioButton {
    id: radioButton

    signal radioButtonClick()

    property int radioButtonWidth: 150
    property int radioButtonHeight: 75

    property real labelSize: radioButtonWidth/8

    property color labelColor: "black"

    property color borderColor: "transparent"
    property color onHoverColor: "crimson"
    property color normalColor: "lightblue"

    property string labelText: qsTr("Radio Button")
    property alias buttonGroup: radioButton.exclusiveGroup

    ExclusiveGroup { id: group }

    text: labelText
    exclusiveGroup: group
    style: RadioButtonStyle {
        indicator: Rectangle {
            implicitWidth: 16
            implicitHeight: 16
            radius: 9
            border.color: ( control.activeFocus || control.checked ) ? "green" : "darkblue"
            border.width: 1
            Rectangle {
                anchors.fill: parent
                visible: control.checked
                color: "green"
                radius: 9
                anchors.margins: 4
            }
        }
        background: Rectangle {
            id: backgroundView
            implicitWidth: radioButtonWidth
            implicitHeight: radioButtonHeight
            border.width: control.hovered ? 2 : 1
            border.color: control.hovered ? radioButton.onHoverColor : radioButton.borderColor
            color: control.pressed ? Qt.darker( radioButton.normalColor, 1.5 ) : radioButton.normalColor
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
        radioButtonClick()
    }
}