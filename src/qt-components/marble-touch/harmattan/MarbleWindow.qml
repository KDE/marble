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
    property string components: "harmattan"

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

    function icon( name, size ) {
        if ( name === "actions/go-previous-view" ) {
            return "image://theme/icon-m-toolbar-back"
        } else if ( name === "actions/edit-find" ) {
            return "image://theme/icon-m-toolbar-search"
        } else if ( name === "actions/configure" ) {
            return "image://theme/icon-m-toolbar-settings"
        } else if ( name === "actions/go-up" ) {
            return "image://theme/icon-m-toolbar-up"
        } else if ( name === "places/user-identity" ) {
            return "image://theme/icon-s-common-location"
        } else if ( name === "devices/network-wireless" ) {
            return "qrc:/marble/wireless.svg"
        } else if ( name === "actions/show-menu" ) {
            return "image://theme/icon-m-toolbar-view-menu"
        } else if ( name === "actions/document-edit" ) {
            return "image://theme/icon-m-toolbar-edit"
        } else if ( name === "actions/edit-clear-locationbar-rtl" ) {
            return "image://theme/icon-m-input-clear"
        } else if ( name === "actions/text-speak" ) {
            return "image://theme/icon-m-toolbar-volume"
        } else if ( name === "status/task-attention" ) {
            return "image://theme/icon-l-error"
        } else if ( name === "actions/media-playback-start" ) {
            return "image://theme/icon-m-toolbar-mediacontrol-play"
        } else if ( name === "actions/dialog-cancel" ) {
            return "image://theme/icon-s-cancel"
        } else if ( name === "actions/dialog-close" ) {
            return "image://theme/icon-s-cancel"
        } else if ( name === "places/folder" ) {
            return "image://theme/icon-m-common-directory"
        } else if ( name === "mimetypes/unknown" ) {
            return "image://theme/icon-m-content-document"
        }

        return name
    }

    Connections { target: pageStack; onDepthChanged: resetLastActivity() }
}
