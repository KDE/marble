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
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
        ToolIcon {
            iconId: "common-location"
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
            iconSource: "image://theme/icon-m-toolbar-search";
        }
    }

    Column {
        width: parent.width
        height: parent.height

        SearchField {
            id: searchField
            width: parent.width
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
            width: parent.width
            height: parent.height - searchField.height
            clip: true

            function embedMarbleWidget() {
                marbleWidget.parent = mapContainer
                settings.projection = "Spherical"
                var plugins = settings.defaultRenderPlugins
                plugins.push( "weather" )
                settings.activeRenderPlugins =  plugins
                settings.mapTheme = "earth/plain/plain.dgml"
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
    }

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
        }
    }
}
