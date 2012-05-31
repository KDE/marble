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
import org.kde.edu.marble.qtcomponents 0.12

/*
 * Page for the weather activity.
 */
Page {
    id: weatherActivityPage
    anchors.fill: parent

    tools: ToolBarLayout {
        MarbleToolIcon {
            iconSource: main.icon( "actions/go-previous-view", 48 );
            onClicked: pageStack.pop()
        }
        MarbleToolIcon {
            iconSource: main.icon( "places/user-identity", 48 );
            onClicked: {
                marbleWidget.centerOn( marbleWidget.tracking.lastKnownPosition.longitude, marbleWidget.tracking.lastKnownPosition.latitude )
                if (marbleWidget.zoom < 403 ) {
                    marbleWidget.zoom = 22026
                }
            }
        }
        ToolButton {
            id: searchButton
            checkable: true
            checked: true
            width: 60
            flat: true
            iconSource: main.icon( "actions/edit-find", 48 );
        }
    }

    SearchField {
        id: searchField
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        visible: searchButton.checked
        onSearch: {
            searchField.busy = true
            marbleWidget.find( term )
        }

        Component.onCompleted: {
            marbleWidget.search.searchFinished.connect( searchFinished )
        }

        function searchFinished() {
            searchField.busy = false
        }
    }

    Item {
        id: mapContainer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: searchButton.checked ? searchField.bottom : parent.top
        anchors.bottom: parent.bottom
        clip: true

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Spherical"
            var plugins = settings.defaultRenderPlugins
            plugins.push( "weather" )
            settings.activeRenderPlugins =  plugins
            settings.mapTheme = settings.streetMapTheme
            settings.gpsTracking = true
            settings.showPositionIndicator = false
            settings.showTrack = false
            marbleWidget.tracking.positionMarkerType = Tracking.Circle
            marbleWidget.visible = true
        }

        Component.onDestruction: {
            if ( marbleWidget.parent === mapContainer ) {
                marbleWidget.parent = null
                marbleWidget.visible = false
            }
        }
    }

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
        }
    }
}
