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
        ListModel {
            id: routingModel
        }
        ListView {
            id: routingView
            anchors.fill: parent
            model: routingModel

            delegate:
                Row {
                    TextField { 
                        id: textField
                        width: 200
                        height: 35
                        Keys.onPressed: {
                            if( event.key == Qt.Key_Return || event.key == Qt.Key_Enter ) {
                                if( text.trim() != "" ) {
                                    routingModel.get( index ).destinationText = text
                                    main.getSearch().find( text )
                                    resultSelectionDialog.searchIndex = index
                                    resultSelectionDialog.load()
                                }
                            }
                        }
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
                                calculateRoute()
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
        anchors.leftMargin: 10
        Column {
            Button {
                text: "Go!"
                width: 200
                platformStyle: ButtonStyle { fontPixelSize: 20 }
                onClicked: {
                    calculateRoute()
                }
            }
            Button {
                text: "Add destination"
                width: 200
                platformStyle: ButtonStyle { fontPixelSize: 20 }
                onClicked: {
                    routingModel.append( { "destinationText": "" } )
                }
            }
        }
    }

    SearchResultSelectionDialog {
        id: resultSelectionDialog
        visible: false
        anchors.fill: parent
        Component.onCompleted: {
            resultSelectionDialog.selected.connect( setRoutingPoint )
        }
        function setRoutingPoint( index, text, lon, lat ) {
            main.getSearch().find( "" )
            console.log( "setRoutingPoint: ", index, text, lon, lat )
            routingModel.get( index ).display = text
            routingModel.get( index ).longitude = lon
            routingModel.get( index ).latitude = lat
        }
    }
    
    function calculateRoute() {
        console.log( "routing started..." )
        main.getRouting().clearRoute()
        for( var i = 0; i < routingView.count; i++ ) {
            console.log( "search: ", i, routingView.model.get( i ).destinationText, routingView.model.get( i ).longitude, routingView.model.get( i ).latitude
            )
            main.getRouting().setVia( i, routingView.model.get( i ).longitude, routingView.model.get( i ).latitude )
        }
    }
    
}