// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 2.3
import org.kde.edu.marble 0.20
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import ".."

/*
 * Page for geocaching activity.
 */
Item {
    id: geocachingActivityPage
    anchors.fill: parent

    RowLayout {
        id: toolBar
        anchors.fill: parent
        ToolButton {
            text: "Home"
            onClicked: activitySelection.showActivities()
        }
        ToolButton {
            id: searchButton
            checkable: true
            width: 60
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

    Component.onCompleted: {
            mapContainer.embedMarbleWidget()
            mainWindow.toolBar.replaceWith(toolBar)
    }
}
