//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Imran Tatriev <itatriev@gmail.com>
//

import QtQuick 2.3

Item {
    id: scrollBarItem

    property Flickable flickableItem: null

    width: 12; height: flickableItem.height
    anchors.right: flickableItem.right

    opacity: flickableItem.flicking ? 1 : 0.6

    Rectangle {
        id: indicator

        width: parent.width-2
        height: scrollBarItem.height * flickableItem.visibleArea.heightRatio

        y: scrollBarItem.height * flickableItem.visibleArea.yPosition

        color: "black"
        radius: 10
    }
}
