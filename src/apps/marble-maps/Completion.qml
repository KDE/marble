//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Mikhail Ivchenko <ematirov@gmail.com>
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
