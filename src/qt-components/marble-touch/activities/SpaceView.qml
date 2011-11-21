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
                onCheckedChanged: {
                    if ( checked ) {
                        settings.mapTheme = "earth/bluemarble/bluemarble.dgml"
                        marbleWidget.setGeoSceneProperty( "clouds_data", cloudsSwitch.checked )
                    }
                }
            }
            Button {
                text: "Moon"
                onCheckedChanged: { if ( checked ) settings.mapTheme = "moon/clementine/clementine.dgml" }
            }
            Button {
                text: "Mars"
                onCheckedChanged: { if ( checked ) settings.mapTheme = "mars/viking/viking.dgml" }
            }
            Button {
                text: "Venus"
                onCheckedChanged: { if ( checked ) settings.mapTheme = "venus/magellan/magellan.dgml" }
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
                text: "Satellites"
                checked: false
                onCheckedChanged: {
                    var plugins = settings.activeRenderPlugins
                    if ( checked ) {
                        plugins.push( "satellites" )
                        settings.activeRenderPlugins = plugins
                    } else {
                        settings.removeElementsFromArray( plugins, ["satellites"] )
                        settings.activeRenderPlugins = plugins
                    }
                }
            }

            MenuItemSwitch {
                id: cloudsSwitch
                text: "Clouds"
                checked: false
                onCheckedChanged: {
                    marbleWidget.setGeoSceneProperty( "clouds_data", checked )
                }
            }
        }
    }

    Item {
        id: mapContainer
        anchors.fill: parent

        Component.onCompleted: {
            marbleWidget.parent = mapContainer
            settings.projection = "Spherical"
            var plugins = settings.defaultRenderPlugins
            //plugins.push( "satellites" )
            settings.activeRenderPlugins =  plugins
            settings.mapTheme = "earth/bluemarble/bluemarble.dgml"
            settings.gpsTracking = false
            settings.showPosition = false
            settings.showTrack = false
            marbleWidget.visible = true
        }

        Component.onDestruction: {
            marbleWidget.parent = null
            marbleWidget.visible = false
        }
    }
}
