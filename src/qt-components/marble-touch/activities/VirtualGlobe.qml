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
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
        ToolButton {
            id: searchButton
            checkable: true
            width: 60
            iconSource: "image://theme/icon-m-toolbar-search";
        }
        ToolButton {
            id: themeButton
            width: 60
            iconSource: "image://theme/icon-m-toolbar-settings";
            onClicked: themeDialog.open()

            MapThemeModel {
                id: mapThemeModel
                mapThemeFilter: MapThemeModel.Extraterrestrial
            }

            SelectionDialog {
                id: themeDialog
                titleText: "Select Map Theme"
                selectedIndex: mapThemeModel.indexOf(settings.streetMapTheme)
                model: mapThemeModel
                delegate:
                    Rectangle {
                    id: delegate
                    width: root.width
                    height: mapImage.height

                    color: index === themeDialog.selectedIndex ? root.platformStyle.itemSelectedBackgroundColor : root.platformStyle.itemBackgroundColor

                    Row {
                        anchors.verticalCenter: parent.verticalCenter
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
                            color: delegate.index === themeDialog.selectedIndex ? root.platformStyle.itemSelectedTextColor : root.platformStyle.itemTextColor
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            themeDialog.selectedIndex = index
                            themeDialog.accept()
                            settings.mapTheme = mapThemeId
                        }
                    }
                }
            }
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

            function embedMarbleWidget() {
                marbleWidget.parent = mapContainer
                settings.projection = "Spherical"
                settings.activeRenderPlugins = settings.defaultRenderPlugins
                settings.mapTheme = "earth/srtm/srtm.dgml"
                settings.gpsTracking = false
                settings.showPosition = false
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
    }

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
        }
    }
}
