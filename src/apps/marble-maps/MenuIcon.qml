//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.3
import QtQuick.Window 2.2

Item {
    id: root
    anchors.left: parent.left
    anchors.right: parent.right
    height: childrenRect.height

    property bool checkable: false
    property bool checked: false
    property alias text: text.text
    property alias icon: image.source

    signal triggered()

    Rectangle {
        anchors.fill: parent
        visible: root.checkable && root.checked
        color: palette.highlight
        radius: 3
    }

    Item {
        id: container
        property real padding: Screen.pixelDensity * 1
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: childrenRect.height + padding
        anchors.margins: padding

        Row {
            anchors.leftMargin: Screen.pixelDensity * 4
            spacing: Screen.pixelDensity * 2
            id: row

            Image {
                id: image
                sourceSize.height: text.height
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: text.verticalCenter
            }

            Text {
                id: text
                font.pointSize: 18
                color: root.checkable && root.checked ? palette.highlightedText : palette.text
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (root.checkable) {
                root.checked = !root.checked
            }
            root.triggered()
        }
    }
}
