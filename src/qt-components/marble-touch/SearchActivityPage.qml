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
 * Page for geocaching activity.
 */
Page {
    id: searchActivityPage
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
            iconId: "toolbar-view-menu" }
    }


    Column {
        width: parent.width
        height: parent.height

        SearchField {
            id: searchField
            width: parent.width
            onSearch: {
                searchResultView.searchTerm = term
                marbleWidget.find( term )
            }
        }

        Item {
            id: mapContainer
            clip: true
            width: parent.width
            height: parent.height - searchField.height

            Component.onCompleted: {
                marbleWidget.parent = mapContainer
                settings.projection = "Mercator"
                settings.activeRenderPlugins = settings.defaultRenderPlugins
                settings.mapTheme = "earth/openstreetmap/openstreetmap.dgml"
                settings.gpsTracking = false
                settings.showPosition = false
                settings.showTrack = false
                marbleWidget.visible = true
            }

            Component.onDestruction: {
                marbleWidget.parent = null
                marbleWidget.visible = false
            }

            Rectangle {
                id: searchResultView
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 40
                radius: 10
                color: "lightsteelblue"
                border.width: 1
                border.color: "lightgray"
                width: 200
                height: parent.height
                z: 10
                opacity: 0.9
                visible: searchTerm != ""
                property string searchTerm: ""

                Text {
                    id: searchResultDescription
                    anchors.margins: 5
                    anchors.fill: parent
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                    text: "<p>Search results for <b>" + searchResultView.searchTerm + "</b></p><p>TODO: Here is a nice place for a search result view, allowing the user to see all results, bookmark them immediately, jump to selected results etc.</p>"
                }

                /** @todo: Insert a result list / overview here */
            }
        }
    }
}
