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

    property bool horizontal: width / height > 1.20

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

        Item {}
    }

    Rectangle {
        id: searchResultView

        property bool minimized: !searchButton.checked
        visible: !minimized

        anchors.bottom: searchActivityPage.bottom
        anchors.left: searchActivityPage.left
        width:  searchActivityPage.horizontal ? (minimized ? 0 : searchActivityPage.width / 2) : searchActivityPage.width
        height: searchActivityPage.horizontal ? searchActivityPage.height : (minimized ? 0 : searchActivityPage.height / 2)

        Behavior on height {
            enabled: !searchActivityPage.horizontal;
            NumberAnimation {
                easing.type: Easing.InOutQuad;
                duration: 250
            }
        }

        radius: 10
        color: "#f7f7f7"
        border.width: 2
        border.color: "darkgray"
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

        anchors.left: searchActivityPage.horizontal ? searchResultView.right : searchActivityPage.left
        anchors.bottom: searchActivityPage.horizontal ? searchActivityPage.bottom : searchResultView.top
        anchors.right: searchActivityPage.right
        anchors.top: searchActivityPage.top

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
