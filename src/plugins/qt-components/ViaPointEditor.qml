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

    property string name: "Point in map" /** @todo: Reverse geocoding */
    property bool isCurrentPosition: false

    signal positionChanged()

    property bool _editing: false

    function retrieveInput( lon, lat ) {
        if ( root._editing ) {
            ListView.view.model.setPosition(index, lon, lat)
            root._editing = false
            root.positionChanged()
        }
    }

    Item {
        width: parent.width
        height: 40

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

            MouseArea {
                anchors.fill: parent
                onClicked: marbleWidget.centerOn(longitude, latitude)
            }
        }

        Label {
            id: destinationInputLabel
            anchors.left: waypointIcon.right
            anchors.right: editIcon.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 10
            text: root._editing ? "Select a point" : ( root.isCurrentPosition ? "Current position" : name )
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
        selectedIndex: 1
        model: ListModel {
            ListElement { name: "Current Position" }
            ListElement { name: "Select in map" }
        }

        onAccepted: {
            if ( selectedIndex === 0 ) {
                root.isCurrentPosition = true
                root.ListView.view.model.setPosition(index, marbleWidget.getTracking().lastKnownPosition.longitude, marbleWidget.getTracking().lastKnownPosition.latitude)
                root.positionChanged()
            } else {
                root.isCurrentPosition = false
                root._editing = true
            }
        }
    }

    onIsCurrentPositionChanged: {
        inputSelectionDialog.selectedIndex = isCurrentPosition ? 0 : 1
    }
}
