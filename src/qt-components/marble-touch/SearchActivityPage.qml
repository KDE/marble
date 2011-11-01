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

                Label {
                    height: 30
                    id: searchResultDescription
                    anchors.margins: 5
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                    text: "<p><b>" + searchResultView.searchTerm + "</b></p>"
                }

                Image {
                    id: closeImage
                    width: 30
                    anchors.top: searchResultView.top
                    anchors.right: searchResultView.right
                    anchors.margins: 5
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    source: "image://theme/icon-m-toolbar-close"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            searchResultView.visible = false
                        }
                    }
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

                ScrollDecorator {
                    flickableItem: searchResultListView
                }

            }
        }
    }

    Component {
        id: searchResultDelegate

        Item {
            width: parent.width
            height: column.height + 5
            id: searchResultItem
            property bool detailed: ListView.isCurrentItem

            /** @todo: Need access to bookmark manager here */
            property bool isBookmark: false

            Column {
                id: column
                Label {
                    id: searchResultText
                    text: "<font size=\"-2\">" + (index+1) + ". " + description + "</font>"
                    width: searchResultItem.width
                    wrapMode: searchResultItem.detailed ? Text.WrapAtWordBoundaryOrAnywhere : Text.NoWrap
                    clip: true

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            searchResultListView.currentIndex = index
                            marbleWidget.centerOn( longitude, latitude )
                        }
                    }
                }

                Row {
                    spacing: 5

                    Button {
                        id: routeButton
                        width: 120
                        text: "Route"
                        visible: searchResultItem.detailed
                        onClicked: {
                            /** @todo: Enable position tracking and set current position as first via point,
                                switch to driving/cycle/walk activity. Ask user, configure routing accordingly */
                            marbleWidget.getRouting().setVia( 1, longitude, latitude )
                        }
                    }

                    Button  {
                        width: 50
                        iconSource: searchResultItem.isBookmark ? "qrc:/icons/bookmark.png" : "qrc:/icons/bookmark-disabled.png"
                        visible: searchResultItem.detailed
                        onClicked: searchResultItem.isBookmark = !searchResultItem.isBookmark
                    }
                }
            }
        }
    }
}
