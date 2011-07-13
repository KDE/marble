// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.1
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

Rectangle {
    Rectangle {
        id: routingRect
        anchors.top: parent.top
        anchors.left: parent.left
        height: 310
        width: parent.width
        clip: true
        ListView {
            id: routingView
            anchors.fill: parent
            model: ListModel {
                id: routingModel
            }

            delegate:
                Row {
                    TextField { 
                        width: 200
                        height: 35
                    }
                    Image {
                        width: 30
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                        source: "image://theme/icon-m-toolbar-close"
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                routingModel.remove( index )
                            }
                        }
                    }
                }
        }
    }
    Rectangle {
        id: buttonRect
        anchors.top: routingRect.bottom
        anchors.left: parent.left
        anchors.topMargin: 10
        Column {
            Button {
                text: "Go!"
                width: 200
                platformStyle: ButtonStyle { fontPixelSize: 20 }
                onClicked: {
                    for( var i = 0; i < routingView.count; i++ ) {
                        main.getRouting().setVia( i, 0.0, 0.0 )  // FIXME
                    }
                }
            }
            Button {
                text: "Add destination"
                width: 200
                platformStyle: ButtonStyle { fontPixelSize: 20 }
                onClicked: {
                    routingModel.append( {} )
                }
            }
        }
    }
}