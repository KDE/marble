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
 * Page for the weather activity.
 */
Page {
    id: weatherActivityPage
    anchors.fill: parent

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
        ToolButton {
            id: searchButton
            checkable: true
            checked: true
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
                text: "Map Theme"
                onClicked: {
                    pageStack.push( "qrc:/MapThemeSelectionPage.qml" )
                }
            }
            MenuItemSwitch {
                text: "Online"
                checked: !settings.workOffline
                onClicked: settings.workOffline = !settings.workOffline
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
            onSearch: marbleWidget.find( term )
        }

        Item {
            id: mapContainer
            width: parent.width
            height: parent.height - searchField.height
            clip: true

            Component.onCompleted: {
                marbleWidget.parent = mapContainer
                settings.projection = "Spherical"
                var plugins = settings.defaultRenderPlugins
                plugins.push( "weather" )
                settings.activeRenderPlugins =  plugins
                settings.mapTheme = "earth/plain/plain.dgml"
                settings.gpsTracking = true
                settings.showPosition = true
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
