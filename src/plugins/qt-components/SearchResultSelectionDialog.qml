// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.1
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

Rectangle {
    id: searchResultSelectionDialog
    color: "white"
    property int searchIndex
    signal selected( int index, string text, real lat, real lon )
    ListView {
        id: resultView
        anchors.fill: parent
        anchors.margins: UiConstants.DefaultMargin
        model: main.getSearch().searchResultModel()
        spacing: 10

        delegate:
            Rectangle {
                id: result
                property string destinationText: display
                property real lon: longitude
                property real lat: latitude
                border.width: 1
                border.color: "blue"
                radius: 15
                width: resultView.width
                height: resultLabel.height + 20
                Label {
                    id: resultLabel
                    text: display
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 15
                    anchors.rightMargin: 15
                    platformStyle: LabelStyle { fontPixelSize: 15 }
                }
            }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                var x = mouseX + resultView.contentX
                var y = mouseY + resultView.contentY
                // FIXME better way to query data?
                resultView.currentIndex = resultView.indexAt( x, y )
                var item = resultView.currentItem
                searchResultSelectionDialog.visible = false
                searchResultSelectionDialog.selected( searchResultSelectionDialog.searchIndex, item.destinationText, item.lon, item.lat )
            }
        }
    }
    Rectangle {
        id: loadRect
        z: 1
        anchors.fill: parent
        color: "white"
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Searching..."
            }
            BusyIndicator {
                anchors.horizontalCenter: parent.horizontalCenter
                platformStyle: BusyIndicatorStyle {
                    size: "medium"
                    period: 800
                    numberOfFrames: 5
                }
                running: loadRect.visible
            }
        }
    }
    Rectangle {
        id: noResultsRect
        visible: false
        z: 1
        anchors.fill: parent
        color: "white"
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "No search results."
            }
            Button {
                text: "Back"
                width: parent.width
                onClicked: {
                    noResultsRect.visible = false
                    searchResultSelectionDialog.visible = false
                }
            }
        }
    }
    Component.onCompleted: {
        main.getSearch().searchFinished.connect( showSearchResults )
    }
    function load() {
        searchResultSelectionDialog.visible = true
        loadRect.visible = true
    }
    function showSearchResults() {
        resultView.model = main.getSearch().searchResultModel()
        noResultsRect.visible = loadRect.visible = false
        console.log( "numResults: ", resultView.model.numResults )
        if( resultView.model.numResults < 1 ) {
            noResultsRect.visible = true
        }
    }
}