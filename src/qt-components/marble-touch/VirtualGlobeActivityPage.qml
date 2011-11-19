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
        ToolIcon {
            iconId: "toolbar-search";
            onClicked: { searchField.visible = !searchField.visible }
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
                text: "Map Theme"
                onClicked: {
                    pageStack.push( "qrc:/MapThemeSelectionPage.qml" )
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
            onSearch: marbleWidget.find( term )
        }

        Item {
            id: mapContainer
            width: parent.width
            height: parent.height - searchField.height

            Component.onCompleted: {
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
                marbleWidget.parent = null
                marbleWidget.visible = false
            }
        }
    }
}
