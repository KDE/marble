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
 * Page for friends (social, opendesktop) activity.
 */
Page {
    id: friendsActivityPage
    anchors.fill: parent

    tools: ToolBarLayout {
        MarbleToolIcon {
            iconSource: main.icon( "actions/go-home", 48 );
            onClicked: main.showNavigation()
        }
        MarbleToolIcon {
            iconSource: main.icon( "places/user-identity", 48 );
            onClicked: {
                marbleWidget.centerOn( marbleWidget.tracking.lastKnownPosition.longitude, marbleWidget.tracking.lastKnownPosition.latitude )
                if (marbleWidget.zoom < 22026 ) {
                    marbleWidget.zoom = 86250
                }
            }
        }

        Item {}
    }

    Item {
        clip: true
        id: mapContainer
        anchors.fill: parent

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Mercator"
            var plugins = settings.defaultRenderPlugins
            settings.removeElementsFromArray(plugins, ["coordinate-grid", "sun", "stars", "compass"])
            plugins.push( "opendesktop" )

            settings.activeRenderPlugins =  plugins
            settings.mapTheme = settings.streetMapTheme
            settings.gpsTracking = true
            settings.showPositionIndicator = true
            marbleWidget.tracking.positionMarkerType = Tracking.Circle
            settings.showTrack = false
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
