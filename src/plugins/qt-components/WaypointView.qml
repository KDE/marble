// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

/*
 * View that displays points on the way of a route.
 */
ListView {
    id: mapListView
    z: 1
    anchors.margins: UiConstants.DefaultMargin
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
                text: display
            }
        }

}