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
            searchField.query = "";
        }
    }

    SearchResults {
        id: searchResults
        anchors {
            top: searchField.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        visible: false
        onItemSelected: {
            searchField.query = name;
            backend.updateMap(index);
            searchResults.visible = false;
            searchField.focus = true;
        }
    }

    SearchBackend {
        id: backend
        marbleQuickItem: root.marbleQuickItem
        onUpdateSearchResults: {
            searchResults.model = model;
            searchResults.visible = true;
        }
    }

    SearchField {
        id: searchField
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        onSearchRequested: backend.search(query)
    }
}
