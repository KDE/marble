// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

import org.kde.marble

RowLayout {
    id: root
    height: text === "" ? 0 : Math.max(icon.height, text.height)

    property alias text: text.text
    property alias icon: icon.source
    property alias font: text.font
    property alias maximumLineCount: text.maximumLineCount

    signal linkActivated(string link)

    Kirigami.Icon {
        id: icon
        Layout.preferredWidth: Kirigami.Units.iconSizes.smallMedium
        Layout.preferredHeight: Kirigami.Units.iconSizes.smallMedium
    }

    Controls.Label {
        id: text
        wrapMode: Text.WordWrap
        elide: Text.ElideRight
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter

        onLinkActivated: root.linkActivated(link)

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
        }
    }
}
