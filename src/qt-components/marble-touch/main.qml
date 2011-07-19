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
            ToolIcon {
                id: backButton
                iconId: "toolbar-back"
                visible: settingsPage.visible
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                onClicked: { settingsPage.back() }
            }
            ToolIcon {
                id: waypointButton
                iconId: "common-location-picker"
                visible: !settingsPage.visible
                anchors.right: routeRequestButton.left
                anchors.bottom: parent.bottom
                onClicked: { waypointView.visible = !waypointView.visible }
            }
            ToolIcon {
                id: routeRequestButton
                iconId: "toolbar-callhistory"
                visible: !settingsPage.visible
                anchors.right: searchButton.left
                anchors.bottom: parent.bottom
                onClicked: { routeRequestView.visible = !routeRequestView.visible }
            }
            ToolIcon {
                id: searchButton
                iconId: "toolbar-search"
                visible: !settingsPage.visible && !routingDialog.visible
                anchors.right: settingsButton.left
                anchors.bottom: parent.bottom
                MouseArea {
                    anchors.fill: parent
                    onClicked: { searchBar.visible = searchBar.activated = !searchBar.visible }
                }
            }
            ToolIcon {
                id: settingsButton
                iconId: "toolbar-settings"
                anchors.right: activityButton.left
                anchors.bottom: parent.bottom
                MouseArea {
                    anchors.fill: parent
                    onClicked: { settingsPage.visible = !settingsPage.visible }
                }
            }
            ToolIcon {
                id: activityButton
                iconId: "toolbar-view-menu-dimmed-white"
                visible: !settingsPage.visible
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                MouseArea {
                    anchors.fill: parent
                    onClicked: { activitySelection.visible = true; activitySelection.activity = -1 }
                }
            }
        }
    }
    
    /*states: [
        State {
            name: "settingsVisible"
            when: settingsPage.visible
            PropertyChanges { target: mainWidget; visible: false }
            PropertyChanges { target: searchBar;  visible: false }
        },
        State {
            name: "mainWithoutSearch"
            when: !settingsPage.visible && !searchBar.activated
            PropertyChanges { target: mainWidget; anchors.top: main.top }
        },
        State {
            name: "mainWithSearch"
            when: !settingsPage.visible && searchBar.activated
            PropertyChanges { target: mainWidget; anchors.top: searchBar.bottom }
            PropertyChanges { target: searchBar;  focus: true }
        }
    ]*/

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
            PropertyChanges { target: mainToolBar; visible: true }
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
            PropertyChanges { target: mainToolBar; visible: true }
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
            PropertyChanges { target: mainToolBar; visible: true }
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

}