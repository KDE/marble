// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <nienhueser@kde.org>

import QtQuick 2.3
import org.kde.edu.marble 0.20
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import ".."

/*
 * Page for the space view activity.
 */
Item {
    id: spaceViewActivityPage
    anchors.fill: parent

    RowLayout {
        id: toolBar
        anchors.fill: parent
        ToolButton {
            text: "Home"
            onClicked: activitySelection.showActivities()
        }

        RowLayout {
            ExclusiveGroup { id: planetGroup }

            RadioButton {
                id: earthButton
                exclusiveGroup: planetGroup
                text: "Earth"
                checked: true
                onCheckedChanged: { if ( checked ) spaceViewActivityPage.setEarthSettings() }
            }
            RadioButton {
                exclusiveGroup: planetGroup
                text: "Moon"
                onCheckedChanged: { if ( checked ) settings.mapTheme = "moon/clementine/clementine.dgml" }
            }
            RadioButton {
                exclusiveGroup: planetGroup
                text: "Other..."
                onCheckedChanged: { if ( checked ) themeDialog.open() }
            }
        }

        ToolButton {
            id: satellitesSwitch
            text: "Satellites"
            checkable: true
            checked: false
            onCheckedChanged: spaceViewActivityPage.setEarthSettings()
        }

        ToolButton {
            id: cloudsSwitch
            text: "Clouds"
            checkable: true
            checked: false
            onCheckedChanged: spaceViewActivityPage.setEarthSettings()
        }

        RowLayout {
            ExclusiveGroup { id: lightingGroup }

            RadioButton {
                id: dayView
                text: "Day"
                checked: true
                exclusiveGroup: lightingGroup
                onCheckedChanged: spaceViewActivityPage.setEarthSettings()
            }

            RadioButton {
                id: nightView
                text: "Night"
                checked: false
                exclusiveGroup: lightingGroup
                onCheckedChanged: spaceViewActivityPage.setEarthSettings()
            }

            RadioButton {
                id: realtimeView
                text: "Realtime"
                checked: false
                exclusiveGroup: lightingGroup
                onCheckedChanged: spaceViewActivityPage.setEarthSettings()
            }
        }
        Item { Layout.fillWidth: true }
    }

    MapThemeModel {
        id: mapThemeModel
        mapThemeFilter: MapThemeModel.Terrestrial
    }

    Item {
        id: mapContainer
        anchors.fill: parent

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Spherical"
            settings.activeRenderPlugins =  settings.defaultRenderPlugins
            spaceViewActivityPage.setEarthSettings()
            if (marbleWidget.radius > 655 ) {
                marbleWidget.radius = 170
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

    SelectionDialog {
        id: themeDialog
        titleText: "Select Map Theme"
        currentIndex: mapThemeModel.indexOf(settings.mapTheme)
        model: mapThemeModel
        delegate:
            Rectangle {
            id: delegate
            width: row.width
            height: row.height

            color: index === themeDialog.currentIndex ? "lightsteelblue" : "#00ffffff"

            Row {
                id: row
                spacing: 5
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
                    font.pointSize: 18
                    color: "black"
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

    Timer {
        id: delayedMapThemeSwitch
        property string theme: "earth/bluemarble/bluemarble.dgml"
        interval: 10; running: false; repeat: false
        onTriggered: settings.mapTheme = theme
    }

    Component.onCompleted: {
        mapContainer.embedMarbleWidget()
        mainWindow.toolBar.replaceWith(toolBar)
    }

    function setEarthSettings() {
        if (dayView.checked) {
            settings.mapTheme = "earth/bluemarble/bluemarble.dgml"
            marbleWidget.setGeoSceneProperty( "citylights", false )
            marbleWidget.setGeoSceneProperty( "clouds_data", cloudsSwitch.checked )
        } else if (nightView.checked) {
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
