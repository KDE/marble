// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

Page {
    id: searchActivityPage
    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: { pageStack.pop() } }
        ToolIcon { iconId: "toolbar-view-menu"; }
    }

    MainWidget {
        id: mainWidget
        anchors.fill: parent
    }
    SearchBar {
        id: searchBar
        anchors.top: parent.top
        anchors.left: mainWidget.left
        anchors.right: parent.right
        height: 35
        property bool activated: false
        Keys.onPressed: {
            if( event.key == Qt.Key_Return || event.key == Qt.Key_Enter ) {
                mainWidget.find( text )
            }
        }
    }
}