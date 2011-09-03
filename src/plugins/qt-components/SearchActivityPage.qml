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

/*
 * Page for the search activity.
 * 
 * Displays a search bar on top of the map.
 * 
 * FIXME Text not visible on the desktop.
 */
Page {
    id: searchActivityPage
    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: { pageStack.pop() } }
    }

    SearchBar {
        id: searchBar
        height: 35
        anchors.left: parent.left
        anchors.right: parent.right
        Keys.onPressed: {
            if( event.key == Qt.Key_Return || event.key == Qt.Key_Enter ) {
                mainWidget.find( text )
            }
        }
    }
    MainWidget {
        id: mainWidget
        anchors.top: searchBar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }
}
