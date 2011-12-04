// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
import com.nokia.meego 1.0

/*
     * A textfield for searching locations.
     */
TextField {
    signal search( string term )
    property bool busy: false

    id: searchField

    /** @todo: What's the exact height of a text field? Should be defined somewhere, use it here */
    height: visible ? 50 : 0

    placeholderText: "Search..."
    // Icon to clear text in the textfield.
    ToolIcon {
        id: clearButton
        iconId: "input-clear"
        anchors.top: searchField.top
        anchors.right: searchField.right
        anchors.rightMargin: 5
        height: parent.height - 2
        width: parent.height - 2
        visible: !parent.busy
        // Reset text and clear search.
        onClicked: {
            searchField.text = ""
            searchField.search( text )
        }
    }

    BusyIndicator {
        anchors.verticalCenter: searchField.verticalCenter
        anchors.right: searchField.right
        anchors.rightMargin: 15
        visible: searchField.busy
        running: searchField.busy
    }

    Keys.onPressed: {
        if( event.key === Qt.Key_Return || event.key === Qt.Key_Enter ) {
            searchField.search( text )
        }
    }
}
