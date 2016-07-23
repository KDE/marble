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
import QtQuick.Controls 1.3
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
        height: delegateHeight * Math.min(10,count)

        visible: false
        onItemSelected: {
            backend.setSelectedPlacemark(index);
            root.itemSelected();
            searchResults.visible = false;
            if (routingManager) {
                routingManager.addSearchResultAsPlacemark(backend.selectedPlacemark);
            }
            placemarkDialog.placemark = backend.selectedPlacemark;
        }
    }

    Rectangle {
        id: background
        visible: searchField.hasFocus && searchField.query === "" && bookmarks.model.count > 0
        anchors.top: searchField.bottom
        anchors.left: searchField.left
        width: searchField.width
        height: 2 * background.itemSpacing + (delegateHeight) * Math.min(4, bookmarksView.model.count)
        color: palette.base

        property int delegateHeight: 0
        property double itemSpacing: Screen.pixelDensity * 1

        ListView {
            id: bookmarksView
            anchors.fill: parent
            anchors.margins: background.itemSpacing
            clip: true

            model: bookmarks.model
            delegate: Row {
                spacing: background.itemSpacing

                Image {
                    anchors.verticalCenter: parent.verticalCenter
                    source: iconPath.substr(0,1) === '/' ? "file://" + iconPath : iconPath
                    width: Screen.pixelDensity * 4
                    height: width
                    sourceSize.width: width
                    sourceSize.height: height
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: Screen.pixelDensity * 2
                    text: display
                    font.pointSize: 18
                    color: palette.text
                    elide: Text.ElideMiddle

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            bookmarksView.currentIndex = index
                            placemarkDialog.focus = true
                            placemarkDialog.placemark = bookmarks.placemark(index);
                            marbleMaps.centerOn(placemarkDialog.placemark.longitude, placemarkDialog.placemark.latitude)
                        }
                    }
                }

                Component.onCompleted: {
                    if( background.delegateHeight !== height ) {
                        background.delegateHeight = height;
                    }
                }
            }
        }

        ScrollBar {
            flickableItem: bookmarksView
        }
    }


    SearchBackend {
        id: backend
        marbleQuickItem: root.marbleQuickItem
        onSearchResultChanged: {
            searchResults.model = model;
            searchResults.visible = true;
        }
        onSearchFinished: searchField.busy = false
    }

    SearchField {
        id: searchField
        width: parent.width - 2 * anchors.margins <= Screen.pixelDensity * 70 ?
                   parent.width - 2 * anchors.margins
                 : Screen.pixelDensity * 50
        anchors {
            top: parent.top
            left: parent.left
            margins: Screen.pixelDensity * 3
        }
        completionModel: backend.completionModel
        onSearchRequested: backend.search(query)
        onCompletionRequested: backend.setCompletionPrefix(query)
        onCleared: searchResults.visible = false
    }

    Bookmarks {
        id: bookmarks
        map: root.marbleQuickItem
    }
}
