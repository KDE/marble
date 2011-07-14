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
                        id: textField
                        width: 200
                        height: 35
                        Keys.onPressed: {
                            console.log( "text changed: ", text )
                            routingModel.get( index ).destinationText = text
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
                    console.log( "routing started..." )
                    for( var i = 0; i < routingView.count; i++ ) {
                        console.log( "search: ", i, routingView.model.get( i ).destinationText )
                        main.getSearch().find( routingView.model.get( i ).destinationText )
                        //console.log( "results: ", main.getSearch().searchResultModel().rowCount )
                        //if( main.getSearch().searchResultModel().rowCount > 1 ) {
                            resultSelectionDialog.visible = true
                        //}
                    }
                }
            }
            Button {
                text: "Add destination"
                width: 200
                platformStyle: ButtonStyle { fontPixelSize: 20 }
                onClicked: {
                    console.log( "adding input field" )
                    routingModel.append( { "destinationText": "" } )
                }
            }
        }
    }

    SearchResultSelectionDialog {
        id: resultSelectionDialog
        visible: false
        anchors.fill: parent
    }
    
}