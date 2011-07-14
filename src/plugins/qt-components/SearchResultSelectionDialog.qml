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
    ListView {
        id: resultView
        anchors.fill: parent
        anchors.margins: UiConstants.DefaultMargin
        model: main.getSearch().searchResultModel()
        spacing: 10
        signal selected( string text, real lat, real lon )

        delegate:
            Rectangle {
                id: result
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
                var item = resultView.model.index( resultView.indexAt( x, y ) )
                searchResultSelectionDialog.visible = false
                selected( item.display, item.coordinate.longitude, item.coordinate.latitude )
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