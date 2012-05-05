// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>

import org.kde.edu.marble.qtcomponents 0.12
import org.kde.edu.marble 0.11
import QtQuick 1.0
import com.nokia.meego 1.0

Window {
    id: main

    property variant initialPage: activitySelection
    property Item marbleWidget: null

    Item {
        id: contentArea
        anchors.fill: parent

        PageStack {
            id: pageStack
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: toolBar.top
            toolBar: toolBar
            clip: true
        }

        ToolBar {
            id: toolBar
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }

    // Stores the settings of the application.
    MarbleSettings {
        id: settings
    }

    // Displays all available activities and starts them if the user clicks on them.
    ActivitySelectionView {
        id: activitySelection
        tools: Item {} // FIXME: Plasma ToolBar internalToolsChanged() should handle null
    }

    function resetLastActivity() {
        if ( marbleWidget !== null && pageStack.depth < 2 ) {
            settings.lastActivity = ""
        }
    }

    function openActivity( activity ) {
        activitySelection.openActivity( activity )
    }

    function openPage( path ) {
        pageStack.push( path )
    }

    Component.onCompleted: {
        pageStack.push(main.initialPage)
        if ( settings.lastActivity === "" ) {
            activitySelection.initializeDelayed()
        } else {
            activitySelection.openActivity( settings.lastActivity )
        }
    }

    Connections { target: pageStack; onDepthChanged: resetLastActivity() }
}
