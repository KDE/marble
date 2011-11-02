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
            iconId: "toolbar-view-menu" }
    }

    Rectangle {
        id: searchResultView
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        radius: 10
        color: "lightsteelblue"
        border.width: 1
        border.color: "lightgray"
        z: 10
        opacity: 0.9
        property string searchTerm: ""

        Label {
            height: 30
            id: searchResultDescription
            anchors.margins: 5
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere

            text: searchResultView.searchTerm === "" ? "<p>Example search terms<ul><li>London</li><li>Baker Street, London</li><li>Baker Street 221b, London</li><li>Restaurant, London</li></ul></p>" : "<p><b>" + searchResultView.searchTerm + "</b></p>"
        }

        ListView {
            id: searchResultListView
            anchors.top: searchResultDescription.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 5
            model: marbleWidget.getSearch().searchResultModel()
            delegate: searchResultDelegate
            highlight: Rectangle { color: "white"; radius: 5 }
            focus: true
        }

        ScrollDecorator {
            flickableItem: searchResultListView
        }
    }

    Column {
        id: mapColumn

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
        }
    }

    StateGroup {
        states: [
            State { // Horizontal
                when: (searchActivityPage.width / searchActivityPage.height) > 1.20
                AnchorChanges { target: mapColumn; anchors.left: searchResultView.right; anchors.bottom: searchActivityPage.bottom }
                PropertyChanges { target: mapColumn; width: searchActivityPage.width / 2; height: searchActivityPage.height }
                AnchorChanges { target: searchResultView; anchors.right: mapColumn.left; anchors.top: searchActivityPage.top }
                PropertyChanges { target: searchResultView; width: searchActivityPage.width / 2; height: searchActivityPage.height }
            },
            State { // Vertical
                when: (true)
                AnchorChanges { target: mapColumn; anchors.left: searchActivityPage.left; anchors.top: searchActivityPage.top }
                PropertyChanges { target: mapColumn; width: searchActivityPage.width; height: searchActivityPage.height / 2 }
                AnchorChanges { target: searchResultView; anchors.right: searchActivityPage.right; anchors.top: mapColumn.bottom }
                PropertyChanges { target: searchResultView; width: searchActivityPage.width; height: searchActivityPage.height / 2 }
            }
        ]
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
