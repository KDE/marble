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
 * Page for geocaching activity.
 */
Page {
    id: geocachingActivityPage
    anchors.fill: parent

    tools: ToolBarLayout {
        MarbleToolIcon {
            iconSource: main.icon( "actions/go-previous-view", 48 );
            onClicked: pageStack.pop()
        }
        ToolButton {
            id: searchButton
            checkable: true
            width: 60
            flat: true
            iconSource: main.icon( "actions/edit-find", 48 );
        }
    }

    Column {
        width: parent.width
        height: parent.height

        SearchField {
            id: searchField
            visible: searchButton.checked
            width: parent.width
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
            clip: true
            id: mapContainer
            width: parent.width
            height: parent.height - searchField.height

            function embedMarbleWidget() {
                marbleWidget.parent = mapContainer
                settings.projection = "Mercator"
                var plugins = settings.defaultRenderPlugins
                settings.removeElementsFromArray(plugins, ["coordinate-grid", "sun", "stars", "compass"])
                plugins.push( "opencaching" )
                settings.activeRenderPlugins =  plugins
                settings.mapTheme = settings.streetMapTheme
                settings.gpsTracking = true
                settings.showPositionIndicator = true
                marbleWidget.tracking.positionMarkerType = Tracking.Arrow
                settings.showTrack = true
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
