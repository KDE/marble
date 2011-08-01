// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
import com.nokia.meego 1.0

Page {
    anchors.margins: UiConstants.DefaultMargin
    
    ListModel {
        id: pagesModel
        ListElement {
            page: "ConnectionSettingsPage.qml"
            title: "GPS and Internet"
        }
        ListElement {
            page: "MapThemeSelectionPage.qml"
            title: "Select Map Theme"
        }
        ListElement {
            page: "ViewSettingsPage.qml"
            title: "View"
        }
        ListElement {
            page: "NavigationSettingsPage.qml"
            title: "Navigation"
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: pagesModel

        delegate: Item {
            id: settingsItem
            height: 60
            width: parent.width
            Label {
                anchors.fill: parent
                text: model.title
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    openPage( page )
                }
            }
        }
    }

    function openPage( file ) {
        var page = Qt.createComponent( file )
       // FIXME check necessary?
       // if ( page.status == page.Ready ) {
            pageStack.push( page );
       // }
       // else {
       //     console.log( "Error loading page: ", page.errorString() );
       // }
    }

}