// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 2.3
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
import org.kde.edu.marble 0.20
import ".."

/*
 * Page for friends (social, opendesktop) activity.
 */
Item {
    id: friendsActivityPage

    width: 600
    height: 800

    RowLayout {
        id: toolBar
        anchors.fill: parent
        ToolButton {
            text: "Home"
            onClicked: activitySelection.showActivities()
        }
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

    Component.onCompleted: {
        mapContainer.embedMarbleWidget()
        mainWindow.toolBar.replaceWith(toolBar)
    }
}
