// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.1
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

ListView {
    id: mapListView
    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin
    highlightFollowsCurrentItem: true
    model: main.waypointModel()
    spacing: 10
    
    delegate:
        Rectangle {
            id: waypointItem
            width: mapListView.width
            height: 50
            Label {
                id: themeLabel
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 15
            }
        }

}