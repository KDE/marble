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
                searchResultListView.model = marbleWidget.getSearch().searchResultModel()
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
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 30
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                    text: "<p>Search results for <b>" + searchResultView.searchTerm + "</b></p>"
                }

                ListView {
                    id: searchResultListView
                    anchors.top: searchResultDescription.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 5
                    width: parent.width
                    height: parent.height - searchResultDescription.height
                    model: marbleWidget.getSearch().searchResultModel()
                    delegate: searchResultDelegate
                    highlight: Rectangle { color: "white"; radius: 5 }
                    focus: true
                }
            }
        }
    }

    Component {
        id: searchResultDelegate

        Item {
            width: parent.width
            height: detailed ? 80 : 20
            id: searchResultItem
            property bool detailed: ListView.isCurrentItem

            /** @todo: Need access to bookmark manager here */
            property bool isBookmark: false

            Column {
                Image  {
                    anchors.right: parent.right
                    source: searchResultItem.isBookmark ? "qrc:/icons/bookmark.png" : "qrc:/icons/bookmark-disabled.png"
                    visible: searchResultItem.detailed
                    width: 16
                    height: 16

                    MouseArea {
                        anchors.fill: parent
                        onClicked: searchResultItem.isBookmark = !searchResultItem.isBookmark
                    }
                }

                Text {
                    text: (index+1) + ". " + description
                    width: searchResultItem.width
                    wrapMode: searchResultItem.detailed ? Text.WrapAtWordBoundaryOrAnywhere : Text.NoWrap
                    clip: true
                }
            }
        }
    }
}
