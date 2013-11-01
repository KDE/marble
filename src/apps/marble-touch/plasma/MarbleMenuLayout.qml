// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import com.nokia.meego 1.0

Item {
    id: root
    anchors.left: parent === undefined ? undefined : parent.left
    anchors.right: parent === undefined ? undefined : parent.right
    height: column.height

    default property alias menuChildren: column.children

    Column {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        height: childrenRect.height
    }
}
