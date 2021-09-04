// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.3
import QtQuick.Window 2.2

import org.kde.marble 0.20

Item {
    id: root
    height: text === "" ? 0 : Math.max(icon.height, text.height)

    property alias text: text.text
    property alias icon: icon.source
    property alias font: text.font
    property alias maximumLineCount: text.maximumLineCount
    property alias linkColor: text.linkColor

    signal linkActivated(string link)

    Image {
        id: icon
        sourceSize.height: Screen.pixelDensity * 3
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: text.verticalCenter
    }

    Text {
        id: text
        anchors.left: icon.right
        anchors.right: parent.right
        anchors.leftMargin: icon.width === 0 ? 0 : Screen.pixelDensity * 1
        font.pointSize: 16
        wrapMode: Text.WordWrap
        elide: Text.ElideRight

        onLinkActivated: root.linkActivated(link)

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
        }
    }
}
