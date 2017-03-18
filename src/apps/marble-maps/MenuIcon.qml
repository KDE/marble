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
import QtQuick.Layouts 1.3

Item {
    id: root
    anchors.left: parent.left
    anchors.right: parent.right
    height: container.height + 2 * Screen.pixelDensity

    property bool checkable: false
    property bool checked: false
    property bool hasSettings: false
    property alias text: text.text
    property alias icon: image.source

    signal triggered()
    signal settingsTriggered()

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
        height: row.height
        anchors.margins: padding

        RowLayout {
            id: row
            anchors.left: parent.left
            anchors.right: settingsButton.left
            spacing: Screen.pixelDensity * 2

            Image {
                id: image
                sourceSize.height: text.height
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                id: text
                Layout.fillWidth: true
                font.pointSize: 18
                color: root.checkable && root.checked ? palette.highlightedText : palette.text
                elide: Text.ElideRight
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

        Image {
            id: settingsButton
            visible: root.hasSettings
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            sourceSize.height: 0.67 * text.height
            fillMode: Image.PreserveAspectFit

            source: "qrc:/settings.png"
            MouseArea {
                anchors.fill: parent
                onClicked: root.settingsTriggered()
            }
        }
    }
}
