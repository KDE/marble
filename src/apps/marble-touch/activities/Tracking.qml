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
import QtQuick.Dialogs 1.2
import ".."

/*
 * Page for geocaching activity.
 */
Item {
    id: trackingActivityPage
    anchors.fill: parent

    RowLayout {
        id: toolBar
        anchors.fill: parent
        ToolButton {
            text: "Home"
            onClicked: activitySelection.showActivities()
        }
        ToolButton {
            text: "My Position"
            iconSource: main.icon( "places/user-identity", 48 );
            onClicked: {
                marbleWidget.centerOn( marbleWidget.tracking.lastKnownPosition.longitude, marbleWidget.tracking.lastKnownPosition.latitude )
                if (marbleWidget.zoom < 22026 ) {
                    marbleWidget.zoom = 4197500
                }
            }
        }
        ToolButton {
            id: searchButton
            text: "Search"
            checkable: true
            width: 60
            iconSource: main.icon( "actions/edit-find", 48 );
        }
        ToolButton {
            text: "Save Track"
            onClicked: {
                //saveTrackDialog.fileUrl = Qt.formatDateTime(new Date(), "yyyy-MM-dd_hh.mm.ss") + ".kml"
                saveTrackDialog.open()
            }
        }
        ToolButton {
            text: "Open Track"
            onClicked: openTrackDialog.open()
        }
        ToolButton {
            text: "Auto Center"
            checkable: true
            checked: false
            onCheckedChanged: {
                marbleWidget.tracking.autoCenter = checked
            }
        }
        ToolButton {
            text: "Auto Zoom"
            checked: false
            checkable: true
            onCheckedChanged: {
                marbleWidget.tracking.autoZoom = checked
            }
        }
        Item { Layout.fillWidth: true }
    }


    SearchField {
        id: searchField
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
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
        id: mapContainer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: searchButton.checked ? searchField.bottom : parent.top
        anchors.bottom: parent.bottom
        clip: true

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Mercator"
            var plugins = settings.defaultRenderPlugins
            settings.removeElementsFromArray(plugins, ["coordinate-grid", "sun", "stars", "compass"])
            plugins.push( "speedometer" )
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

    FileDialog {
        id: saveTrackDialog
        folder: "/home/user/MyDocs"
        nameFilters: [ "*.kml" ]

        onAccepted: { marbleWidget.tracking.saveTrack( folder + "/" + filename ); }
    }

    FileDialog {
        id: openTrackDialog
        folder: "/home/user/MyDocs"
        nameFilters: [ "*.kml", "*.gpx" ]

        onAccepted: { marbleWidget.tracking.openTrack( folder + "/" + filename ); }
    }

    Component.onCompleted: {
        mapContainer.embedMarbleWidget()
        mainWindow.toolBar.replaceWith(toolBar)
    }
}
