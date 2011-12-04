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

    property real longitude: 0.0
    property real latitude: 0.0

    property string name
    property bool isCurrentPosition: true

    signal positionChanged()

    function retrieveInput( lon, lat ) {
        if ( destinationInputLabel.editing ) {
            longitude = lon
            latitude = lat
            destinationInputLabel.editing = false
            root.positionChanged()
        }
    }

    ToolBarLayout {
        width: parent.width
        height: 40

        Label {
            id: destinationInputLabel

            property bool editing: false

            text: editing ? name + ": Select a point" : ( root.isCurrentPosition ? name + ": Current position" : name + ": Point in map" )
        }

        ToolIcon {
            iconId: "toolbar-edit"
            onClicked: {
                inputSelectionDialog.open()
            }
        }
    }

    SelectionDialog {
        id: inputSelectionDialog
        titleText: "Select via point"
        selectedIndex: 0
        model: ListModel {
            ListElement { name: "Current Position" }
            ListElement { name: "Select in map" }
        }

        onAccepted: {
            if ( selectedIndex === 0 ) {
                root.isCurrentPosition = true
                root.positionChanged()
            } else {
                root.isCurrentPosition = false
                destinationInputLabel.editing = true
            }
        }
    }

    onIsCurrentPositionChanged: {
        inputSelectionDialog.selectedIndex = isCurrentPosition ? 0 : 1
    }
}
