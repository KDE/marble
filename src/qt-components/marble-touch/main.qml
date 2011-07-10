// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import org.kde.edu.marble.qtcomponents 0.12
import QtQuick 1.1
import com.nokia.meego 1.0

Rectangle {
    id: main
    width: 800
    height: 480

    SearchBar {
        id: searchBar
        visible: false
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
    
        Keys.onPressed: {
            if ( event.key == Qt.Key_Enter ) {
                console.log( "search: ", text );
                mainWidget.find( text )
                event.accepted = true;
            }
        }
    }

    MainWidget {
        id: mainWidget
        anchors.top: parent.top
        anchors.bottom: toolBar.top
        anchors.left: parent.left
        anchors.right: parent.right
        
        function find( text ) {
            mainWidget.screen.map.search.find( text )
        }
    }
    
    SettingsPage {
        id: settingsPage
        visible: false
        anchors.top: parent.top
        anchors.bottom: toolBar.top
        anchors.left: parent.left
        anchors.right: parent.right
    }
    
    Rectangle {
        id: toolBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 60
        color: "white"
        ToolBarLayout {
            id: toolBarLayout
            anchors.fill: parent
            ToolIcon {
                id: backButton
                iconId: "toolbar-back"
                visible: settingsPage.visible
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                onClicked: { 
                    if( settingsPage.pageStack.depth == 1 ) {
                        mainWidget.visible = true
                        settingsPage.visible = false
                    }
                    else {
                        settingsPage.pageStack.pop();
                    }
                }
            }
            ToolIcon {
                id: searchButton
                iconId: "toolbar-search"
                anchors.right: settingsButton.left
                anchors.bottom: parent.bottom
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        searchBar.visible = !searchBar.visible
                        if( searchBar.visible ) {
                            mainWidget.anchors.top = searchBar.bottom
                            searchBar.focus = true
                        }
                        else {
                            mainWidget.anchors.top = main.top;
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
                        mainWidget.visible = !mainWidget.visible
                        settingsPage.visible = !settingsPage.visible
                    }
                }
            }
        }
    }

}