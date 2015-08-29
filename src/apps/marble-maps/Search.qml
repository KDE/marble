//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//

import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import org.kde.edu.marble 0.20

Item {
    id: root

    property var marbleQuickItem: null
    property var routingManager: null

    signal itemSelected()
    readonly property alias searchResultPlacemark: backend.selectedPlacemark
    readonly property alias searchResultsVisible: searchResults.visible

    onVisibleChanged: {
        if( !visible ) {
            searchResults.visible = false;
            searchField.query = "";
        }
    }

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    SearchResults {
        id: searchResults
        anchors {
            top: searchField.bottom
            left: searchField.left
        }
        width: searchField.width
        visible: false
        onItemSelected: {
            backend.setSelectedPlacemark(index);
            root.itemSelected();
            searchResults.visible = false;
            searchField.focus = true;
            if (routingManager) {
                routingManager.addSearchResultAsPlacemark(backend.selectedPlacemark);
            }
            placemarkDialog.placemark = backend.selectedPlacemark;
            itemStack.state = "place"
        }

        MouseArea{
            anchors.fill: parent
            propagateComposedEvents: true
            onPressed: {
                searchField.focus = true;
                mouse.accepted = false;
            }
        }
    }

    SearchBackend {
        id: backend
        marbleQuickItem: root.marbleQuickItem
        onSearchResultChanged: {
            searchResults.model = model;
            searchResults.visible = true;
            searchField.focus = true;
        }
        onSearchFinished: searchField.busy = false
    }

    SearchField {
        id: searchField
        width: Math.min(700, root.width - 6 * Screen.pixelDensity)
        anchors {
            top: parent.top
            left: parent.left
            margins: Screen.pixelDensity * 3
        }
        completionModel: backend.completionModel
        onSearchRequested: backend.search(query)
        onCompletionRequested: backend.setCompletionPrefix(query)
    }
}
