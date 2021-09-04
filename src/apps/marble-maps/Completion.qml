// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Mikhail Ivchenko <ematirov@gmail.com>
//

import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
Item {
    id: root

    property alias model: results.model

    property alias count: results.count

    property alias delegateHeight: results.delegateHeight

    signal itemSelected(int index, string name)

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        border.color: palette.shadow
        border.width: 1
    }

    SearchResults {
        id: results
        anchors.fill: background
        anchors.margins: background.border.width
        clip: true
        background: palette.base
        onItemSelected: root.itemSelected(index, name)
    }
}
