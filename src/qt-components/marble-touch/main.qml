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
        visible: !searchBar.visible
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
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                MouseArea {
                    anchors.fill: parent
                    onClicked: { settingsPage.visible = !settingsPage.visible }
                }
            }
        }
    }
    
    states: [
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