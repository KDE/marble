import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import org.kde.edu.marble 0.20

Item {
    id: root

    visible: false

    property var marbleQuickItem: null

    onVisibleChanged: {
        if( !visible ) {
            searchResults.visible = false;
            background.visible = false;
            searchField.query = "";
        }
    }

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        visible: false
        color: palette.window
    }

    SearchResults {
        id: searchResults
        anchors {
            top: searchField.bottom
            bottom: parent.bottom
            left: searchField.left
            right: parent.right
        }
        visible: false
        onItemSelected: {
            searchField.query = name;
            backend.updateMap(index);
            searchResults.visible = false;
            background.visible = false;
            searchField.focus = true;
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
        onUpdateSearchResults: {
            searchResults.model = model;
            searchResults.visible = true;
            background.visible = true;
            searchField.focus = true;
        }
    }

    SearchField {
        id: searchField
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: Screen.pixelDensity * 3
        }
        completionModel: backend.completionModel
        onSearchRequested: backend.search(query)
        onCompletionRequested: backend.setCompletionPrefix(query)
    }
}
