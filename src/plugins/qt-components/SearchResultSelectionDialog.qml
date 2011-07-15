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
                console.log( "current index: ", resultView.currentIndex )
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
        Label {
            text: "Searching..."
        }
        // FIXME fancy search animation ;)
    }
    Component.onCompleted: {
        main.getSearch().searchFinished.connect( hideLoadRect )
    }
    function hideLoadRect() {
        resultView.model = main.getSearch().searchResultModel()
        loadRect.visible = false
    }
}