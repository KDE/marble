// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.1
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11
import org.kde.edu.marble.qtcomponents 0.12

/*
 * Page for the space view activity.
 */
Page {
    id: spaceViewActivityPage
    anchors.fill: parent

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }

        ButtonRow {
            Button {
                id: earthButton
                text: "Earth"
                onCheckedChanged: { if ( checked ) spaceViewActivityPage.setEarthSettings() }
            }
            Button {
                text: "Moon"
                onCheckedChanged: { if ( checked ) settings.mapTheme = "moon/clementine/clementine.dgml" }
            }
            Button {
                text: "More"
                onCheckedChanged: { if ( checked ) themeDialog.open() }

                MapThemeModel {
                    id: mapThemeModel
                    mapThemeFilter: MapThemeModel.Terrestrial
                }

                SelectionDialog {
                    id: themeDialog
                    titleText: "Select Map Theme"
                    selectedIndex: mapThemeModel.indexOf(settings.mapTheme)
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
                                delayedMapThemeSwitch.theme = mapThemeId
                                delayedMapThemeSwitch.start()
                            }
                        }
                    }
                }
            }
        }

        ToolIcon {
            iconId: "toolbar-view-menu"
            visible: earthButton.checked
            onClicked: pageMenu.open()
        }
    }

    Menu {
        id: pageMenu
        content: MenuLayout {
            MenuItemSwitch {
                id: satellitesSwitch
                text: "Satellites"
                checked: false
                onCheckedChanged: spaceViewActivityPage.setEarthSettings()
            }

            MenuItemSwitch {
                id: cloudsSwitch
                text: "Clouds"
                checked: false
                onCheckedChanged: spaceViewActivityPage.setEarthSettings()
            }

            MenuItem {
                id: dayNightMode
                text: "Day/Night"
                onClicked: dayNightInput.open()

                SelectionDialog {
                    id: dayNightInput
                    titleText: "Select sun light view"
                    selectedIndex: 0
                    model: ListModel {
                        ListElement { name: "Day" }
                        ListElement { name: "Night" }
                        ListElement { name: "Realistic" }
                    }

                    onAccepted: spaceViewActivityPage.setEarthSettings()
                }
            }
        }
    }

    Item {
        id: mapContainer
        anchors.fill: parent

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Spherical"
            settings.activeRenderPlugins =  settings.defaultRenderPlugins
            spaceViewActivityPage.setEarthSettings()
            if (marbleWidget.zoom > 1300 ) {
                marbleWidget.zoom = 1027
            }
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
        property string theme: "earth/bluemarble/bluemarble.dgml"
        interval: 10; running: false; repeat: false
        onTriggered: settings.mapTheme = theme
    }

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
        }
    }

    function setEarthSettings() {
        if (dayNightInput === null || dayNightInput.selectedIndex === 0) {
            settings.mapTheme = "earth/bluemarble/bluemarble.dgml"
            marbleWidget.setGeoSceneProperty( "citylights", false )
            marbleWidget.setGeoSceneProperty( "clouds_data", cloudsSwitch === null || cloudsSwitch.checked )
        } else if (dayNightInput.selectedIndex === 1) {
            settings.mapTheme = "earth/citylights/citylights.dgml"
            marbleWidget.setGeoSceneProperty( "citylights", false )
            marbleWidget.setGeoSceneProperty( "clouds_data", cloudsSwitch.checked )
        } else {
            settings.mapTheme = "earth/bluemarble/bluemarble.dgml"
            marbleWidget.setGeoSceneProperty( "citylights", true )
            marbleWidget.setGeoSceneProperty( "clouds_data", cloudsSwitch.checked )
        }

        var plugins = settings.activeRenderPlugins
        if ( satellitesSwitch.checked ) {
            plugins.push( "satellites" )
        } else {
            settings.removeElementsFromArray( plugins, ["satellites"] )
        }
        settings.activeRenderPlugins = plugins
    }
}
