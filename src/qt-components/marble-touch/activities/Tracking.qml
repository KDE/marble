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
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
        ToolIcon {
            iconId: "toolbar-home"
            onClicked: {
                marbleWidget.centerOn( marbleWidget.getTracking().lastKnownPosition.longitude, marbleWidget.getTracking().lastKnownPosition.latitude )
                if (marbleWidget.radius < 22026 ) {
                    marbleWidget.radius = 4197500
                }
            }
        }
        ToolButton {
            id: searchButton
            checkable: true
            width: 60
            iconSource: "image://theme/icon-m-toolbar-search";
        }
        ToolIcon {
            iconId: "toolbar-view-menu"
            onClicked: pageMenu.open()
        }
    }

    Menu {
        id: pageMenu
        content: MenuLayout {
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
                    marbleWidget.getTracking().autoCenter = checked
                }
            }
            MenuItemSwitch {
                text: "Auto Zoom"
                checked: false
                onCheckedChanged: {
                    marbleWidget.getTracking().autoZoom = checked
                }
            }
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
                marbleWidget.getSearch().searchFinished.connect( searchFinished )
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
                settings.projection = "Mercator"
                var plugins = settings.defaultRenderPlugins
                settings.removeElementsFromArray(plugins, ["coordinate-grid", "sun", "stars", "compass"])
                plugins.push( "speedometer" )
                settings.activeRenderPlugins =  plugins
                settings.mapTheme = settings.streetMapTheme
                settings.gpsTracking = true
                settings.showPosition = true
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

    FileSaveDialog {
        id: saveTrackDialog
        anchors.fill: parent
        folder: "/home/user/MyDocs"
        filename: ""
        nameFilters: [ "*.kml" ]

        onAccepted: { marbleWidget.getTracking().saveTrack( folder + "/" + filename ); }
    }

    FileOpenDialog {
        id: openTrackDialog
        anchors.fill: parent
        folder: "/home/user/MyDocs"
        nameFilters: [ "*.kml", "*.gpx" ]

        onAccepted: { marbleWidget.getTracking().openTrack( folder + "/" + filename ); }
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
