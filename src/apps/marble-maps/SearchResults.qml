// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
// SPDX-FileCopyrightText: 2015 Mikhail Ivchenko <ematirov@gmail.com>
//

import QtQuick 2.3
import QtQuick.Controls
import QtQuick.Window 2.2

Item {
    id: root

    property var model: []
    property color background: palette.base
    property alias count: view.count
    property int delegateHeight: 0

    signal itemSelected(int index, string name)

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: root.background
    }

    ListView {
        id: view
        anchors.fill: parent
        clip: true
        snapMode: ListView.SnapToItem
        model: root.model
        delegate: Item {
            width: view.width
            height: placemarkName.height + 20

            Rectangle {
                id: delegateBackground
                anchors.fill: parent
                color: mouseArea.pressed ? palette.highlight : root.background
            }

            Image {
                id: icon
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                width: height
                height: placemarkName.height
                source: iconPath.substr(0,1) === '/' ? "file://" + iconPath : iconPath
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.Pad
            }

            Text {
                id: placemarkName
                anchors.left: icon.right
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: 18
                color: palette.text
                text: name
                elide: Text.ElideMiddle
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    root.itemSelected(index, name);
                }
            }

            Component.onCompleted: {
                if( root.delegateHeight != height ) {
                    root.delegateHeight = height;
                }
            }
        }
    }

    MarbleScrollBar {
        id: scrollBar
        flickableItem: view
    }
}
