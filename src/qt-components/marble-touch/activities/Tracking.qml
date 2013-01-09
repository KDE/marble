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
import QtMobility.systeminfo 1.1

/*
 * Page for geocaching activity.
 */
Page {
    id: trackingActivityPage
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
                    marbleWidget.zoom = 4197500
                }
            }
        }
        ToolButton {
            id: searchButton
            checkable: true
            width: 60
            flat: true
            iconSource: main.icon( "actions/edit-find", 48 );
        }
        MarbleToolIcon {
            id: menuIcon
            iconSource: main.icon( "actions/show-menu", 48 );
            onClicked: {
                if (main.components === "plasma") {
                    pageMenu.visualParent = menuIcon
                }
                pageMenu.open()
            }
        }
    }

    Menu {
        id: pageMenu
        content: MarbleMenuLayout {
            MenuItem {
                text: "Save Track"
                onClicked: {
                    saveTrackDialog.filename = Qt.formatDateTime(new Date(), "yyyy-MM-dd_hh.mm.ss") + ".kml"
                    saveTrackDialog.open()
                }
            }
            MenuItem {
                text: "Open Track"
                onClicked: openTrackDialog.open()
            }
            MenuItemSwitch {
                text: "Auto Center"
                checked: false
                onCheckedChanged: {
                    marbleWidget.tracking.autoCenter = checked
                }
            }
            MenuItemSwitch {
                text: "Auto Zoom"
                checked: false
                onCheckedChanged: {
                    marbleWidget.tracking.autoZoom = checked
                }
            }
        }
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

    FileSaveDialog {
        id: saveTrackDialog
        anchors.fill: parent
        folder: "/home/user/MyDocs"
        filename: ""
        nameFilters: [ "*.kml" ]

        onAccepted: { marbleWidget.tracking.saveTrack( folder + "/" + filename ); }
    }

    FileOpenDialog {
        id: openTrackDialog
        anchors.fill: parent
        folder: "/home/user/MyDocs"
        nameFilters: [ "*.kml", "*.gpx" ]

        onAccepted: { marbleWidget.tracking.openTrack( folder + "/" + filename ); }
    }

    ScreenSaver {
        id: saver
    }

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
            saver.screenSaverDelayed = settings.inhibitScreensaver
        } else if ( status === PageStatus.Deactivating ) {
            saver.screenSaverDelayed = false
        }
    }
}
