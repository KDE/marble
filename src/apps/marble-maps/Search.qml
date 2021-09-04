//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.2

import org.kde.marble 0.20

Item {
    id: root

    property var marbleQuickItem: null

    signal itemSelected(var suggestedPlacemark)
    signal menuButtonClicked()
    readonly property alias searchResultPlacemark: backend.selectedPlacemark
    readonly property alias searchResultsVisible: searchResults.visible

    onVisibleChanged: {
        if( !visible ) {
            searchResults.visible = false;
            searchField.query = "";
        }
    }

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: paletteDisabled
        colorGroup: SystemPalette.Disabled
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
            root.itemSelected(backend.selectedPlacemark);
            searchResults.visible = false;
        }
    }

    Rectangle {
        id: background
        visible: searchField.hasFocus && searchField.query === ""
        anchors.top: searchField.bottom
        anchors.left: searchField.left
        width: searchField.width
        height: childrenRect.height
        color: palette.base

        property int delegateHeight: 0

        property double itemSpacing: Screen.pixelDensity * 1

        Column {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: background.itemSpacing

            ListView {
                id: bookmarksView
                anchors.left: parent.left
                anchors.right: parent.right
                height: background.delegateHeight * Math.min(6, model.count) + 2 * background.itemSpacing
                clip: true
                ScrollIndicator.vertical: ScrollIndicator { }

                model: bookmarks.model

                MouseArea {
                    anchors.bottom: parent.bottom
                    height: 2 * background.itemSpacing
                    width: parent.width
                    visible: bookmarks.model.count <= 6
                    onClicked: {
                        app.selectedPlacemark = bookmarks.placemark(bookmarks.model.count - 1)
                        itemSelected(bookmarks.placemark(bookmarks.model.count - 1))
                        marbleMaps.centerOn(selectedPlacemark.longitude, selectedPlacemark.latitude)
                        dialogLoader.focus = true
                    }
                }

                delegate: Row {
                    width: bookmarksView.width
                    height: background.itemSpacing + Math.max(bookmarkIcon.height, bookmarkText.height)
                    spacing: background.itemSpacing

                    leftPadding: 10
                    rightPadding: 10

                    Image {
                        id: bookmarkIcon
                        anchors.verticalCenter: parent.verticalCenter
                        source: iconPath.substr(0,1) === '/' ? "file://" + iconPath : iconPath
                        width: Screen.pixelDensity * 4
                        height: width
                        sourceSize.width: width
                        sourceSize.height: height
                    }

                    Text {
                        id: bookmarkText
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.leftMargin: Screen.pixelDensity * 2
                        width: bookmarksView.width - bookmarksView.spacing - bookmarkIcon.width
                        text: display
                        font.pointSize: 14
                        color: palette.text
                        elide: Text.ElideMiddle

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                bookmarksView.currentIndex = index
                                app.selectedPlacemark = bookmarks.placemark(index);
                                itemSelected(bookmarks.placemark(index))
                                marbleMaps.centerOn(selectedPlacemark.longitude, selectedPlacemark.latitude)
                                dialogLoader.focus = true
                            }
                        }
                    }

                    onHeightChanged: {
                        if( background.delegateHeight !== height ) {
                            background.delegateHeight = height;
                        }
                    }
                }
            }

            Row {
                visible: bookmarksView.model.count === 0
                width: parent.width
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: Screen.pixelDensity * 2
                anchors.margins: Screen.pixelDensity * 2

                Text {
                    anchors.bottom: parent.bottom
                    leftPadding: 10
                    bottomPadding: 3
                    width: parent.width - Screen.pixelDensity * 2 - emptyImage.width
                    font.pointSize: 14
                    color: paletteDisabled.text
                    text: qsTr("Your bookmarks will appear here.")

                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    elide: Text.ElideRight
                }

                Image {
                    id: emptyImage
                    anchors.bottom: parent.bottom
                    width: Screen.pixelDensity* 10

                    fillMode: Image.PreserveAspectFit
                    source: "qrc:/konqi/books.png"
                }
            }
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
        width: parent.width
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        completionModel: backend.completionModel
        onSearchRequested: backend.search(query)
        onCompletionRequested: backend.setCompletionPrefix(query)
        onCleared: searchResults.visible = false
        onMenuButtonClicked: root.menuButtonClicked()
    }

    Bookmarks {
        id: bookmarks
        map: root.marbleQuickItem
    }
}
