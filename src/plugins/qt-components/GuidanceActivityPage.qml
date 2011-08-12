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
    id: guidanceActivityPage
    tools: ToolBarLayout {
        ToolIcon { iconId: "toolbar-back"; onClicked: { pageStack.pop() } }
        ToolIcon { iconId: "toolbar-search"; onClicked: { pageStack.replace( main.activityModel.get( "Search", "path" ) ); main.changeActivity( "Geocaching", "Search" ) } }
        ToolIcon { iconId: "toolbar-view-menu" }
    }

    MainWidget {
        id: mainWidget
        anchors.fill: parent
    }
}