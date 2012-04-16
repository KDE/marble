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
    }

    Rectangle {
        id: searchResultView
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
                searchResultListView.model = marbleWidget.search.searchResultModel()
            }

            Component.onCompleted: {
                marbleWidget.search.searchFinished.connect( searchFinished )
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
            visible: searchResultView.searchTerm === "" && searchResultListView.count === 0
        }

        ListView {
            id: searchResultListView
            property int count: model === undefined ? 0 : model.count

            anchors.top: searchField.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            model: marbleWidget.search.searchResultModel()
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

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Mercator"
            var plugins = settings.defaultRenderPlugins
            settings.removeElementsFromArray(plugins, ["coordinate-grid", "sun", "stars", "compass"])
            settings.activeRenderPlugins = plugins
            settings.mapTheme = settings.streetMapTheme
            settings.gpsTracking = false
            settings.showPositionIndicator = true
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

    StateGroup {
        states: [
            State { // Horizontal
                when: (searchActivityPage.width / searchActivityPage.height) > 1.20
                AnchorChanges { target: searchResultView; anchors.top: searchActivityPage.top }
                PropertyChanges { target: searchResultView; width: searchActivityPage.width / 2; height: searchActivityPage.height }
                AnchorChanges { target: mapContainer; anchors.left: searchResultView.right; anchors.bottom: searchActivityPage.bottom; anchors.top: searchActivityPage.top }
                PropertyChanges { target: mapContainer; width: searchActivityPage.width / 2; height: searchActivityPage.height }
            },
            State { // Vertical
                when: (true)
                AnchorChanges { target: mapContainer; anchors.left: searchActivityPage.left; anchors.bottom: searchResultListView.top; anchors.top: searchActivityPage.top }
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

            Column {
                id: column

                Row {
                    spacing: 5

                    Label {
                        id: searchResultText
                        anchors.verticalCenter: parent.verticalCenter
                        text: "<font size=\"-1\">" + (index+1) + ". " + display + "</font>"
                        width: searchResultItem.width - bookmarkButton.width - parent.spacing - 10
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

                    ToolButton  {
                        id: bookmarkButton
                        anchors.verticalCenter: parent.verticalCenter
                        flat: true
                        height: 38
                        width: 38
                        property bool isBookmark: marbleWidget.bookmarks.isBookmark(longitude,latitude)
                        iconSource: isBookmark ? "qrc:/icons/bookmark.png" : "qrc:/icons/bookmark-disabled.png"
                        visible: searchResultItem.detailed
                        onClicked: {
                            if ( isBookmark ) {
                                marbleWidget.bookmarks.removeBookmark(longitude,latitude)
                            } else {
                                marbleWidget.bookmarks.addBookmark(longitude,latitude, display, "Default")
                            }
                            isBookmark = marbleWidget.bookmarks.isBookmark(longitude,latitude)
                        }
                    }
                }

                Row {
                    spacing: 5
                    visible: searchResultItem.detailed

                    Button {
                        id: carButton
                        width: 120
                        iconSource: "qrc:/icons/routing-motorcar.svg"
                        onClicked: {
                            marbleWidget.routing.routingProfile = "Motorcar"
                            searchActivityPage.startRouting(longitude, latitude)
                        }
                    }

                    Button {
                        id: bikeButton
                        width: 80
                        iconSource: "qrc:/icons/routing-bike.svg"
                        onClicked: {
                            marbleWidget.routing.routingProfile = "Bicycle"
                            searchActivityPage.startRouting(longitude, latitude)
                        }
                    }

                    Button {
                        id: pedButton
                        width: 60
                        iconSource: "qrc:/icons/routing-pedestrian.svg"
                        onClicked: {
                            marbleWidget.routing.routingProfile = "Pedestrian"
                            searchActivityPage.startRouting(longitude, latitude)
                        }
                    }
                }
            }
        }
    }

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
        }
    }

    function startRouting(longitude, latitude)
    {
        settings.gpsTracking = true
        marbleWidget.routing.clearRoute()
        marbleWidget.routing.setVia( 0, marbleWidget.tracking.lastKnownPosition.longitude, marbleWidget.tracking.lastKnownPosition.latitude )
        marbleWidget.routing.setVia( 1, longitude, latitude )
        openActivity( "Routing" )
    }
}
