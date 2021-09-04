//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Imran Tatriev <itatriev@gmail.com>
//

import QtQuick 2.3
import QtQuick.Window 2.2

Item {
    id: scrollBarItem

    property Flickable flickableItem: null
    property real position: flickableItem.visibleArea.yPosition
    property real pageSize: flickableItem.visibleArea.heightRatio

    width: Screen.pixelDensity * 1.5
    height: flickableItem.height
    anchors.right: flickableItem.right

    opacity: flickableItem.movingVertically ? 1 : 0
    clip: true

    Rectangle {
        id: indicator
        y: scrollBarItem.position * (scrollBarItem.height-2) + 1
        width: parent.width-2
        height: scrollBarItem.pageSize * (scrollBarItem.height-2)
        radius: width/2 - 1
        color: "lightgray"
    }

    Behavior on opacity { NumberAnimation { duration: 125 } }
}
