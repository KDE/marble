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
        color: "white"
        border.width: 1
        border.color: "lightgray"
        z: 10
        opacity: 0.9
        property string searchTerm: ""

        SearchField {
            id: searchField
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            height: 60
            onSearch: {
                searchResultView.searchTerm = term
                searchField.busy = true
                marbleWidget.find( term )
                searchResultListView.model = marbleWidget.getSearch().searchResultModel()
            }

            Component.onCompleted: {
                marbleWidget.getSearch().searchFinished.connect( searchFinished )
            }

            function searchFinished() {
                searchField.busy = false
            }
        }

        Label {
            height: 30
            id: searchResultDescription
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            anchors.top: searchField.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10

            text: "<p>Example search terms<ul><li>London</li><li>Baker Street, London</li><li>Baker Street 221b, London</li><li>Restaurant, London</li></ul></p>"
            visible: searchResultView.searchTerm === ""
        }

        ListView {
            id: searchResultListView
            anchors.top: searchField.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            model: marbleWidget.getSearch().searchResultModel()
            delegate: searchResultDelegate
            highlight: Rectangle { color: "lightgray"; radius: 5 }
            focus: true
            clip: true
        }

        ScrollDecorator {
            flickableItem: searchResultListView
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
            var plugins = settings.defaultRenderPlugins
            settings.removeElementsFromArray(plugins, ["coordinate-grid", "sun", "stars", "compass"])
            settings.activeRenderPlugins = plugins
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

    StateGroup {
        states: [
            State { // Horizontal
                when: (searchActivityPage.width / searchActivityPage.height) > 1.20
                AnchorChanges { target: mapContainer; anchors.left: searchResultView.right; anchors.bottom: searchActivityPage.bottom }
                PropertyChanges { target: mapContainer; width: searchActivityPage.width / 2; height: searchActivityPage.height }
                AnchorChanges { target: searchResultView; anchors.right: mapContainer.left; anchors.top: searchActivityPage.top }
                PropertyChanges { target: searchResultView; width: searchActivityPage.width / 2; height: searchActivityPage.height }
            },
            State { // Vertical
                when: (true)
                AnchorChanges { target: mapContainer; anchors.left: searchActivityPage.left; anchors.top: searchActivityPage.top }
                PropertyChanges { target: mapContainer; width: searchActivityPage.width; height: searchActivityPage.height / 2 }
                AnchorChanges { target: searchResultView; anchors.right: searchActivityPage.right; anchors.top: mapContainer.bottom }
                PropertyChanges { target: searchResultView; width: searchActivityPage.width; height: searchActivityPage.height / 2 }
            }
        ]
    }

    Component {
        id: searchResultDelegate

        Item {
            id: searchResultItem
            width: parent.width - 20
            height: column.height + 10
            x: 10
            y: 5
            property bool detailed: ListView.isCurrentItem

            /** @todo: Need access to bookmark manager here */
            property bool isBookmark: false

            Column {
                id: column

                Label {
                    id: searchResultText
                    text: "<font size=\"-1\">" + (index+1) + ". " + description + "</font>"
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
                        visible: false // searchResultItem.detailed
                        onClicked: searchResultItem.isBookmark = !searchResultItem.isBookmark
                    }
                }
            }
        }
    }
}
