// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.1
import com.nokia.meego 1.0

ListView {
    id: listView
    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin
    model: TextFieldModel { }
    spacing: 10
    
    delegate:
        Rectangle {
            id: listItem
            width: listView.width
            height: 50
            TextField {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 15
            }
        }

}