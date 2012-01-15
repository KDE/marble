// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

/*
 * Dialog to select routing points.
 */
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

            // Shows a text input field for the search term and a cross to remove it.
            delegate:
                Row {
                    TextField { 
                        id: textField
                        width: 200
                        height: 35
                        Keys.onPressed: {
                            routingModel.get( index ).destinationText = text
                            if( event.key == Qt.Key_Return || event.key == Qt.Key_Enter ) {
                                if( text.trim() != "" ) {
                                    main.search.find( text )
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
    // Two buttons to start routing and add another input field for an additional
    // point on the route.
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

    // If there are multiple locations matching the term entered in the
    // textfield, show a dialog to choose one of them.
    SearchResultSelectionDialog {
        id: resultSelectionDialog
        visible: false
        anchors.fill: parent
        Component.onCompleted: {
            resultSelectionDialog.selected.connect( setRoutingPoint )
        }
        function setRoutingPoint( index, text, lon, lat ) {
            main.search.find( "" )
            console.log( "setRoutingPoint: ", index, text, lon, lat )
            routingModel.get( index ).display = text
            routingModel.get( index ).longitude = lon
            routingModel.get( index ).latitude = lat
        }
    }
    
    // Calculates the route for the entered points.
    function calculateRoute() {
        console.log( "routing started..." )
        main.routing.clearRoute()
        for( var i = 0; i < routingView.count; i++ ) {
            console.log( "search: ", i, routingView.model.get( i ).destinationText, routingView.model.get( i ).longitude, routingView.model.get( i ).latitude
            )
            main.routing.setVia( i, routingView.model.get( i ).longitude, routingView.model.get( i ).latitude )
        }
    }
    
}
