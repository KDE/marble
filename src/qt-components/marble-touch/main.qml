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
        anchors.left: parent.left
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
        anchors.left: parent.left
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
    
    RoutingDialog {
        id: routingDialog
        anchors.fill: parent
        visible: false
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
                onClicked: {
                    settingsPage.back()
                }
            }
            ToolIcon {
                id: routingButton
                iconId: "toolbar-callhistory"
                visible: !settingsPage.visible
                anchors.right: searchButton.left
                anchors.bottom: parent.bottom
                onClicked: {
                    routingDialog.visible = !routingDialog.visible
                }
            }
            ToolIcon {
                id: searchButton
                iconId: "toolbar-search"
                visible: !settingsPage.visible
                anchors.right: settingsButton.left
                anchors.bottom: parent.bottom
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        searchBar.visible = !searchBar.visible
                        if( searchBar.visible ) {
                            mainWidget.anchors.top = searchBar.bottom
                            searchBar.focus = true
                            searchBar.activated = true
                        }
                        else {
                            mainWidget.anchors.top = main.top;
                            searchBar.activated = false
                        }
                    }
                }
            }
            ToolIcon {
                id: settingsButton
                iconId: "toolbar-settings"
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        settingsPage.visible = !settingsPage.visible
                        if( settingsPage.visible ) {
                            mainWidget.visible = searchBar.visible = false
                        }
                        else {
                            main.hideSettings()
                        }
                    }
                }
            }
        }
    }
    
    function hideSettings() {
        settingsPage.visible = false
        mainWidget.visible = true
        if( searchBar.activated ) {
            searchBar.visible = true
            mainWidget.anchors.top = searchBar.bottom
        }
        else {
            mainWidget.anchors.top = main.top
        }
    }

}