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
 * Page for the virtual globe activity.
 */
Item {
    id: virtualGlobeActivityPage
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
            text: "Search"
            checkable: true
            width: 60
            iconSource: main.icon( "actions/edit-find", 48 );
        }
        ToolButton {
            id: themeButton
            width: 60
            text: "Map Theme"
            iconSource: main.icon( "actions/configure", 48 );
            onClicked: themeDialog.visible = true

            MapThemeModel {
                id: mapThemeModel
                mapThemeFilter: MapThemeModel.Extraterrestrial
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

    SelectionDialog {
        id: themeDialog
        anchors.fill: parent
        titleText: "Select Map Theme"
        currentIndex: mapThemeModel.indexOf(settings.mapTheme)
        model: mapThemeModel
        delegate:
            Rectangle {
            id: delegate
            width: row.width
            height: row.height

            color: index === themeDialog.currentIndex ? "lightsteelblue" : "white"

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
                    width: 400
                    text: display
                    color: index === themeDialog.currentIndex ? "black" : "gray"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    themeDialog.currentIndex = index
                    themeDialog.visible = false
                    delayedMapThemeSwitch.theme = mapThemeId
                    delayedMapThemeSwitch.start()
                }
            }
        }
    }


    Component.onCompleted: {
        mapContainer.embedMarbleWidget()
        mainWindow.toolBar.replaceWith(toolBar)
    }
}
