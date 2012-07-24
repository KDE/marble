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
 * Page for the virtual globe activity.
 */
Page {
    id: virtualGlobeActivityPage
    anchors.fill: parent

    tools: ToolBarLayout {
        MarbleToolIcon {
            iconSource: main.icon( "actions/go-home", 48 );
            onClicked: main.navigationMenu.open()
        }
        ToolButton {
            id: searchButton
            checkable: true
            width: 60
            flat: true
            iconSource: main.icon( "actions/edit-find", 48 );
        }
        ToolButton {
            id: themeButton
            width: 60
            iconSource: main.icon( "actions/configure", 48 );
            flat: true
            onClicked: themeDialog.open()

            MapThemeModel {
                id: mapThemeModel
                mapThemeFilter: MapThemeModel.Extraterrestrial
            }

            SelectionDialog {
                id: themeDialog
                titleText: "Select Map Theme"
                selectedIndex: mapThemeModel.indexOf(settings.mapTheme)
                model: mapThemeModel
                delegate:
                    Rectangle {
                    id: delegate
                    width: row.width
                    height: row.height

                    color: index === themeDialog.selectedIndex ? "lightsteelblue" : "#00ffffff"

                    Row {
                        id: row
                        Image {
                            id: mapImage
                            source: "image://maptheme/" + mapThemeId
                            smooth: true
                            width: 68
                            height: 68
                        }
                        Label {
                            id: themeLabel
                            text: display
                            color: index === themeDialog.selectedIndex ? "black" : "white"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            themeDialog.selectedIndex = index
                            themeDialog.accept()
                            delayedMapThemeSwitch.theme = mapThemeId
                            delayedMapThemeSwitch.start()
                        }
                    }
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

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Spherical"
            settings.activeRenderPlugins = settings.defaultRenderPlugins
            settings.mapTheme = "earth/srtm/srtm.dgml"
            settings.gpsTracking = false
            settings.showPositionIndicator = false
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

    Timer {
        id: delayedMapThemeSwitch
        property string theme: "earth/srtm/srtm.dgml"
        interval: 10; running: false; repeat: false
        onTriggered: settings.mapTheme = theme
    }

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
        }
    }
}
