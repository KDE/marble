// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import com.nokia.meego 1.0

Item {
    id: root
    height: row.height

    property string text: "Point in map" /** @todo: Reverse geocoding */
    property bool isCurrentPosition: false

    signal positionChanged(int index, real lon, real lat)

    property bool _editing: false

    function retrieveInput( lon, lat ) {
        if ( _editing ) {
            _editing = false
            positionChanged(index, lon, lat)
        }
    }

    Item {
        id: row
        width: parent.width
        height: Math.max(waypointIcon.height, destinationInputLabel.height)

        Rectangle {
            id: waypointIcon
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            width: 32
            height: 32
            radius: 16
            color: "#37A42C"
            border.width: 1
            border.color: "black"

            Text {
                anchors.centerIn: parent
                text: String.fromCharCode(65+index)
                font.pixelSize: 24
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Label {
            id: destinationInputLabel
            anchors.left: waypointIcon.right
            anchors.right: editIcon.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 10
            text: root._editing ? "Select a point" : ( root.isCurrentPosition ? "Current position" : root.text )
        }

        MouseArea {
            anchors.fill: parent
            onClicked: marbleWidget.centerOn(longitude, latitude)
        }

        ToolIcon {
            id: editIcon
            iconId: "toolbar-edit"
            anchors.right: parent.right
            anchors.margins: 5
            anchors.verticalCenter: parent.verticalCenter
            onClicked: {
                inputSelectionDialog.open()
            }
        }
    }

    SelectionDialog {
        id: inputSelectionDialog
        titleText: "Select via point"
        selectedIndex: -1
        model: ListModel {
            ListElement { name: "Current Position" }
            ListElement { name: "Choose Bookmark" }
            ListElement { name: "Select from map" }
        }

        onAccepted: {
            if ( selectedIndex === 0 ) {
                root.isCurrentPosition = true
                root.positionChanged(index, marbleWidget.tracking.lastKnownPosition.longitude, marbleWidget.tracking.lastKnownPosition.latitude)
            } else if ( selectedIndex === 1 ) {
                bookmarkSelectionDialog.open()
            } else {
                root.isCurrentPosition = false
                root._editing = true
            }
        }
    }

    SelectionDialog {
        id: bookmarkSelectionDialog
        titleText: "Select Bookmark"
        model: marbleWidget.bookmarks.model
        onAccepted: {
            root.text = marbleWidget.bookmarks.model.name(selectedIndex)
            root.positionChanged(index, marbleWidget.bookmarks.model.longitude(selectedIndex), marbleWidget.bookmarks.model.latitude(selectedIndex))
        }
    }

    onIsCurrentPositionChanged: {
        inputSelectionDialog.selectedIndex = isCurrentPosition ? 0 : 1
    }
}
