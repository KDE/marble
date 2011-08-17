// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
import com.nokia.meego 1.0

/*
 * Page for configuration activity.
 * 
 * Gives graphical access to the settings of the application.
 */
Page {
    id: configurationPage
    anchors.margins: UiConstants.DefaultMargin
    tools: commonToolBar
    
    // Contains sub-pages for the settings.
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
            height: 88
            width: parent.width
            BorderImage {
                anchors.fill: parent
                visible: mouseArea.pressed
                source: "image://theme/meegotouch-list-background-pressed-center"
            }
            Label {
                text: model.title
                font: UiConstants.TitleFont
                anchors.verticalCenter: parent.verticalCenter
            }
            Image {
                source: "image://theme/icon-m-common-drilldown-arrow" + ( theme.inverted ? "-inverse" : "" )
                anchors.right: parent.right;
                anchors.verticalCenter: parent.verticalCenter
            }
            MouseArea {
                id: mouseArea
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