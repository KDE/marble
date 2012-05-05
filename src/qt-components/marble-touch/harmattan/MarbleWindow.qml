// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11
import org.kde.edu.marble.qtcomponents 0.12

PageStackWindow {
    id: main

    width: screen.displayWidth
    height: screen.displayHeight

    property Item marbleWidget: null
    property bool inPortrait: width < height

    initialPage: activitySelection

    // Stores the settings of the application.
    MarbleSettings {
        id: settings
    }

    // Displays all available activities and starts them if the user clicks on them.
    ActivitySelectionView {
        id: activitySelection
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
        if ( settings.lastActivity === "" ) {
            activitySelection.initializeDelayed()
        } else {
            activitySelection.openActivity( settings.lastActivity )
        }
    }

    Connections { target: pageStack; onDepthChanged: resetLastActivity() }
}
