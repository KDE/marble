// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import org.kde.edu.marble.qtcomponents 0.12
import org.kde.edu.marble 0.11
import QtQuick 1.1
import com.nokia.meego 1.0

Rectangle {
    id: main
    width: 800
    height: 480

    MarbleSettings {
        id: settings
    }
    
    ActivitySelectionView {
        id: activitySelection
        anchors.fill: parent
    }

    SearchBar {
        id: searchBar
        visible: false
        anchors.top: parent.top
        anchors.left: mainWidget.left
        anchors.right: parent.right
        height: 35
        property bool activated: false
        Keys.onPressed: {
            if( event.key == Qt.Key_Return || event.key == Qt.Key_Enter ) {
                console.log( "search triggered: ", text )
                mainWidget.find( text )
            }
        }
    }

    MainWidget {
        id: mainWidget
        visible: false
        anchors.top: parent.top
        anchors.bottom: mainToolBar.top
        anchors.left: routingDialog.right
        anchors.right: parent.right
    }
    
    SettingsPage {
        id: settingsPage
        visible: false
        anchors.top: parent.top
        anchors.bottom: mainToolBar.top
        anchors.left: parent.left
        anchors.right: parent.right
    }
    
    RouteRequestView {
        id: routeRequestView
        visible: false
        anchors.top: parent.top
        anchors.bottom: mainToolBar.top
        anchors.left: parent.left
        anchors.right: parent.right
    }
    
    WaypointView {
        id: waypointView
        visible: false
        anchors.top: parent.top
        anchors.bottom: mainToolBar.top
        anchors.left: parent.left
        anchors.right: parent.right
    }
    
    RoutingDialog {
        id: routingDialog
        visible: false
        anchors.top: main.top
        anchors.left: main.left
        anchors.bottom: mainToolBar.top
        width: visible ? 240 : 0
    }
    
    Rectangle {
        color: "grey"
        opacity: 0.5
        visible: !searchBar.visible && mainWidget.visible
        anchors.top: routingDialog.top
        anchors.left: routingDialog.right
        width: routingButton.width
        height: routingButton.height
        radius: 5
        Image {
            id: routingButton
            opacity: 1.0
            source: "image://theme/icon-m-common-drilldown-arrow"
            smooth: true
            transform: Rotation { origin.x: 16; origin.y: 16; angle: routingDialog.visible ? 180 : 0 }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    routingDialog.visible = !routingDialog.visible
                }
            }
        }
    }

    ToolBar {
        id: mainToolBar
        visible: false
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        tools: ToolBarLayout {
            id: toolBarLayout
            anchors.fill: parent
            // FIXME icons
            Row {
                ToolIcon {
                    id: backButton
                    iconId: "toolbar-back"
                    onClicked: { settingsPage.back() }
                }
                ToolIcon {
                    id: waypointButton
                    iconId: "common-location-picker"
                    onClicked: { waypointView.visible = !waypointView.visible }
                }
                ToolIcon {
                    id: routeRequestButton
                    iconId: "toolbar-callhistory"
                    onClicked: { routeRequestView.visible = !routeRequestView.visible }
                }
                ToolIcon {
                    id: virtualGlobeButton
                    iconId: "ovi-service-maps"
                    onClicked: { activitySelection.setCurrentActivity( "Virtual Globe" ) }
                }
                ToolIcon {
                    id: driveButton
                    iconId: "content-automobile"
                    visible: !settingsPage.visible
                    onClicked: { activitySelection.setCurrentActivity( "Drive" ) }
                }
                ToolIcon {
                    id: cycleButton
                    iconId: "common-clock"
                    onClicked: { activitySelection.setCurrentActivity( "Cycle" ) }
                }
                ToolIcon {
                    id: walkButton
                    iconId: "camera-scene-sports-screen"
                    onClicked: { activitySelection.setCurrentActivity( "Walk" ) }
                }
                ToolIcon {
                    id: guidanceButton
                    iconId: "telephony-content-sms-dimmed"
                    onClicked: { activitySelection.setCurrentActivity( "Guidance" ) }
                }
                ToolIcon {
                    id: bookmarksButton
                    iconId: "content-bookmark"
                    onClicked: { activitySelection.setCurrentActivity( "Bookmarks" ) }
                }
                ToolIcon {
                    id: aroundMeButton
                    iconId: "transfer-sync"
                    onClicked: { activitySelection.setCurrentActivity( "Around Me" ) }
                }
                ToolIcon {
                    id: weatherButton
                    iconId: "weather-sunny-thunder"
                    onClicked: { activitySelection.setCurrentActivity( "Weather" ) }
                }
                ToolIcon {
                    id: trackingButton
                    iconId: "content-feed"
                    onClicked: { activitySelection.setCurrentActivity( "Tracking" ) }
                }
                ToolIcon {
                    id: geocachingButton
                    iconId: "email-combined-mailbox"
                    onClicked: { activitySelection.setCurrentActivity( "Geocaching" ) }
                }
                ToolIcon {
                    id: friendsButton
                    iconId: "conversation-group-chat"
                    onClicked: { activitySelection.setCurrentActivity( "Friends" ) }
                }
                ToolIcon {
                    id: downloadButton
                    iconId: "transfer-download"
                    onClicked: { activitySelection.setCurrentActivity( "Download" ) }
                }
                ToolIcon {
                    id: searchButton
                    iconId: "toolbar-search"
                    onClicked: { activitySelection.setCurrentActivity( "Search" ) }
                }
                ToolIcon {
                    id: wikipediaButton
                    iconId: "content-wikipedia"
                    onClicked: { main.togglePlugin( "wikipedia" ) }
                }
                ToolIcon {
                    id: photoButton
                    iconId: "content-photoalbum"
                    onClicked: { main.togglePlugin( "photo" ) }
                }
                ToolIcon {
                    id: settingsButton
                    iconId: "toolbar-settings"
                    onClicked: { activitySelection.setCurrentActivity( "Configuration" ) }
                }
                ToolIcon {
                    id: activityButton
                    iconId: "toolbar-view-menu-dimmed-white"
                    onClicked: { activitySelection.visible = true; activitySelection.activity = -1 }
                }
            }
        }
        states: [
            State {
                name: "activitySelection"
                when: main.state == name
            },
            State {
                name: "Virtual Globe"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: false }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: false }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: true }
                PropertyChanges { target: wikipediaButton; visible: true }
                PropertyChanges { target: photoButton; visible: true }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Drive"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: true }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: false }
                PropertyChanges { target: wikipediaButton; visible: false }
                PropertyChanges { target: photoButton; visible: false }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Cycle"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: true }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: false }
                PropertyChanges { target: wikipediaButton; visible: false }
                PropertyChanges { target: photoButton; visible: false }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Walk"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: true }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: false }
                PropertyChanges { target: wikipediaButton; visible: false }
                PropertyChanges { target: photoButton; visible: false }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Guidance"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: true }
                PropertyChanges { target: cycleButton; visible: true }
                PropertyChanges { target: walkButton; visible: true }
                PropertyChanges { target: guidanceButton; visible: false }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: false }
                PropertyChanges { target: wikipediaButton; visible: false }
                PropertyChanges { target: photoButton; visible: false }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Search"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: false }
                PropertyChanges { target: bookmarksButton; visible: true }
                PropertyChanges { target: aroundMeButton; visible: true }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: false }
                PropertyChanges { target: wikipediaButton; visible: true }
                PropertyChanges { target: photoButton; visible: true }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Bookmarks"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: false }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: true }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: true }
                PropertyChanges { target: wikipediaButton; visible: false }
                PropertyChanges { target: photoButton; visible: false }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Around Me"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: false }
                PropertyChanges { target: bookmarksButton; visible: true }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: true }
                PropertyChanges { target: wikipediaButton; visible: true }
                PropertyChanges { target: photoButton; visible: true }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Weather"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: false }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: false }
                PropertyChanges { target: wikipediaButton; visible: false }
                PropertyChanges { target: photoButton; visible: false }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Tracking"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: false }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: false }
                PropertyChanges { target: wikipediaButton; visible: false }
                PropertyChanges { target: photoButton; visible: false }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Geocaching"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: true }
                PropertyChanges { target: walkButton; visible: true }
                PropertyChanges { target: guidanceButton; visible: true }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: false }
                PropertyChanges { target: wikipediaButton; visible: true }
                PropertyChanges { target: photoButton; visible: true }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Friends"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: false }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: false }
                PropertyChanges { target: wikipediaButton; visible: false }
                PropertyChanges { target: photoButton; visible: false }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Download"
                when: main.state == name
                PropertyChanges { target: backButton; visible: false }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: false }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: false }
                PropertyChanges { target: wikipediaButton; visible: false }
                PropertyChanges { target: photoButton; visible: false }
                PropertyChanges { target: settingsButton; visible: true }
                PropertyChanges { target: activityButton; visible: true }
            },
            State {
                name: "Configuration"
                when: main.state == name
                PropertyChanges { target: backButton; visible: true }
                PropertyChanges { target: waypointButton; visible: false }
                PropertyChanges { target: routeRequestButton; visible: false }
                PropertyChanges { target: virtualGlobeButton; visible: true }
                PropertyChanges { target: driveButton; visible: false }
                PropertyChanges { target: cycleButton; visible: false }
                PropertyChanges { target: walkButton; visible: false }
                PropertyChanges { target: guidanceButton; visible: false }
                PropertyChanges { target: bookmarksButton; visible: false }
                PropertyChanges { target: aroundMeButton; visible: false }
                PropertyChanges { target: weatherButton; visible: false }
                PropertyChanges { target: trackingButton; visible: false }
                PropertyChanges { target: geocachingButton; visible: false }
                PropertyChanges { target: friendsButton; visible: false }
                PropertyChanges { target: downloadButton; visible: false }
                PropertyChanges { target: searchButton; visible: false }
                PropertyChanges { target: wikipediaButton; visible: false }
                PropertyChanges { target: photoButton; visible: false }
                PropertyChanges { target: settingsButton; visible: false }
                PropertyChanges { target: activityButton; visible: true }
            }
        ]
    }

    // see http://techbase.kde.org/Projects/Marble/Devices_and_Use_Cases
    states: [
        State {
            name: "activitySelection"
            when: activitySelection.visible && activitySelection.activity == -1
            PropertyChanges { target: mainWidget; visible: false }
            PropertyChanges { target: searchBar;  visible: false }
            PropertyChanges { target: settingsPage; visible: false }
            PropertyChanges { target: waypointView;  visible: false }
            PropertyChanges { target: routeRequestView; visible: false }
            PropertyChanges { target: routingDialog;  visible: false }
            PropertyChanges { target: mainToolBar; visible: false }
            // FIXME disable all plugins?
        },
        State {
            name: "Virtual Globe"
            when: activitySelection.activity == 0
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainWidget; visible: true }
            PropertyChanges { target: mainToolBar; visible: true }
            PropertyChanges { target: settings; projection: "Spherical" }
            PropertyChanges { target: settings; mapTheme: "earth/bluemarble/bluemarble.dgml" }
        },
        State {
            name: "Drive"
            when: activitySelection.activity == 1
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainWidget; visible: true }
            PropertyChanges { target: mainToolBar; visible: true }
            PropertyChanges { target: routingDialog; visible: true }
            PropertyChanges { target: settings; projection: "Mercator" }
            PropertyChanges { target: settings; mapTheme: "earth/openstreetmap/openstreetmap.dgml" }
        },
        State {
            name: "Cycle"
            when: activitySelection.activity == 2
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainWidget; visible: true }
            PropertyChanges { target: mainToolBar; visible: true }
            PropertyChanges { target: routingDialog; visible: true }
            PropertyChanges { target: settings; projection: "Mercator" }
            PropertyChanges { target: settings; mapTheme: "earth/openstreetmap/openstreetmap.dgml" }
        },
        State {
            name: "Walk"
            when: activitySelection.activity == 3
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainWidget; visible: true }
            PropertyChanges { target: mainToolBar; visible: true }
            PropertyChanges { target: routingDialog; visible: true }
            PropertyChanges { target: settings; projection: "Mercator" }
            PropertyChanges { target: settings; mapTheme: "earth/openstreetmap/openstreetmap.dgml" }
        },
        State {
            name: "Guidance"
            when: activitySelection.activity == 4
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainWidget; visible: true }
            PropertyChanges { target: mainToolBar; visible: true }
            PropertyChanges { target: routingDialog; visible: true }
        },
        State {
            name: "Search"
            when: activitySelection.activity == 5
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainWidget; visible: true }
            PropertyChanges { target: mainWidget; anchors.top: searchBar.bottom }
            PropertyChanges { target: mainToolBar; visible: true }
            PropertyChanges { target: searchBar; visible: true }
        },
        State {
            name: "Bookmarks"
            when: activitySelection.activity == 6
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainToolBar; visible: true }
        },
        State {
            name: "Around Me"
            when: activitySelection.activity == 7
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainToolBar; visible: true }
        },
        State {
            name: "Weather"
            when: activitySelection.activity == 8
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainWidget; visible: true }
            PropertyChanges { target: mainToolBar; visible: true }
            PropertyChanges { target: settings; projection: "Spherical" }
            PropertyChanges { target: settings; mapTheme: "earth/bluemarble/bluemarble.dgml" }
            StateChangeScript { script: enablePlugin( "weather" ) }
        },
        State {
            name: "Tracking"
            when: activitySelection.activity == 9
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainWidget; visible: true }
            PropertyChanges { target: mainToolBar; visible: true }
            PropertyChanges { target: settings; projection: "Mercator" }
            PropertyChanges { target: settings; mapTheme: "earth/openstreetmap/openstreetmap.dgml" }
            PropertyChanges { target: settings; gpsTracking: true }
            PropertyChanges { target: settings; showPosition: true }
            PropertyChanges { target: settings; showTrack: true }
            PropertyChanges { target: settings; autoCenter: true }
        },
        State {
            name: "Geocaching"
            when: activitySelection.activity == 10
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainWidget; visible: true }
            PropertyChanges { target: mainToolBar; visible: true }
            PropertyChanges { target: settings; projection: "Mercator" }
            PropertyChanges { target: settings; mapTheme: "earth/openstreetmap/openstreetmap.dgml" }
        },
        State {
            name: "Friends"
            when: activitySelection.activity == 11
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainToolBar; visible: true }
        },
        State {
            name: "Download"
            when: activitySelection.activity == 12
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainToolBar; visible: true }
        },
        State {
            name: "Configuration"
            when: activitySelection.activity == 13
            PropertyChanges { target: activitySelection; visible: false }
            PropertyChanges { target: mainToolBar; visible: true }
            PropertyChanges { target: mainWidget; visible: false }
            PropertyChanges { target: settingsPage; visible: true }
        }
    ]
    
    transitions: [
        Transition {
            from: "*"; to: "*"
            ScriptAction { 
                script: {
                    console.log( "switching from ", activitySelection.previousActivity, " to ", activitySelection.activity )
                    if( activitySelection.activity != -1 ) {
                        var related = undefined
                        var enable = activitySelection.model.get( activitySelection.activity, "enablePlugins" )
                        var disable = activitySelection.model.get( activitySelection.activity, "disablePlugins" )
                        if( activitySelection.previousActivity != -1 ) {
                           related = activitySelection.model.get( activitySelection.previousActivity, "relatedActivities" )
                        }
                        adjustPlugins( enable, disable, related )
                    }
                }
            }
        }
    ]

    function routeRequestModel() {
        return mainWidget.routeRequestModel()
    }
    
    function waypointModel() {
        return mainWidget.waypointModel()
    }
    
    function getRouting() {
        return mainWidget.getRouting()
    }

    function getSearch() {
        return mainWidget.getSearch()
    }
    
    function enablePlugin( name ) {
        console.log( "trying to enable ", name )
        var tmp = settings.activeRenderPlugins
        if( tmp.indexOf( name ) == -1 ) {
            console.log( "- enabling: ", name )
            tmp.push( name )
            settings.activeRenderPlugins = tmp
        }
        else {
            console.log( "- ", name, " is already enabled" )
        }
        console.log( "finished enablePlugin ", name )
    }
    
    function disablePlugin( name ) {
        console.log( "trying to disable ", name )
        var tmp = new Array()
        for( var i = 0; i < settings.activeRenderPlugins.length; i++ ) {
            if( settings.activeRenderPlugins[i] != name ) {
                tmp.push( settings.activeRenderPlugins[i] )
            }
            else {
                console.log( "- disabled: ", name )
            }
        }
        settings.activeRenderPlugins = tmp
        console.log( "finished disablePlugin ", name )
    }
    
    function togglePlugin( name ) {
        if( settings.activeRenderPlugins.indexOf( name ) == -1 ) {
            enablePlugin( name )
        }
        else {
            disablePlugin( name )
        }
    }
    
    function adjustPlugins( enable, disable, preserve ) {
        console.log( "adjustinPlugins ", enable, disable, preserve )
        if( preserve != undefined ) {
            for( var i in preserve ) {
                console.log( "- preserved: ", preserve[i] )
            }
        }
        if( enable != undefined ) {
            for( var i = 0; i < enable.length; i++ ) {
                if( preserve == undefined || preserve.indexOf( enable[i] ) == -1 ) {
                    enablePlugin( enable[i] )
                }
            }
        }
        if( disable != undefined ) {
            for( var i = 0; i < disable.length; i++ ) {
                if( preserve == undefined || preserve.indexOf( disable[i] ) == -1 ) {
                    disablePlugin( disable[i] )
                }
            }
        }
        console.log( "finished adjusting plugins" )
    }

}