//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

import org.kde.marble 0.20

Item {
    id: root

    property var placemark: null
    property string actionIconSource: routeEditor.currentProfileIcon
    property alias map: bookmarks.map

    height: placemark === null ? 0 : Screen.pixelDensity * 6 +
                                 (infoLayout.height > bookmarkButton.height ? infoLayout.height : bookmarkButton.height)

    function addToRoute() {
        ensureRouteHasDeparture()
        routing.addViaByPlacemarkAtIndex(routing.waypointCount(), placemark)
        routing.clearSearchResultPlacemarks()
        placemark = null
        dialogContainer.currentIndex = dialogContainer.routing
    }

    onPlacemarkChanged: {
        if (placemark) {
            bookmarkButton.bookmark = bookmarks.isBookmark(placemark.longitude, placemark.latitude)
            dialogContainer.currentIndex = dialogContainer.place
        } else {
            dialogContainer.currentIndex = dialogContainer.none
        }
    }

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        anchors.fill: parent
        color: palette.base
    }

    Bookmarks {
        id: bookmarks
    }

    Column {
        id: infoLayout
        anchors {
            top: parent.top
            left: parent.left
            right: bookmarkButton.left
            margins: Screen.pixelDensity * 2
        }

        IconText {
            id: name
            width: parent.width
            visible: text.length > 0
            text: placemark === null ? "" : placemark.name
            maximumLineCount: 2
            font.pointSize: 20
        }

        IconText {
            width: parent.width
            visible: text.length > 0
            text: placemark === null ? "" : placemark.description
        }

        IconText {
            width: parent.width
            visible: text.length > 0
            text: placemark === null ? "" : placemark.address
            maximumLineCount: 4
        }

        IconText {
            width: parent.width
            visible: url.length > 0
            property string url: placemark === null ? "" : placemark.website
            text: "<a href=\"" + url + "\">" + url + "</a>"
            icon: "qrc:/material/browser.svg"
            maximumLineCount: 4
        }

        IconText {
            width: parent.width
            visible: url.length > 0
            property string url: placemark === null ? "" : placemark.wikipedia
            text:  "<a href=\"" + url + "\">Wikipedia</a>"
            icon: "qrc:/material/browser.svg"
            maximumLineCount: 4
        }

        IconText {
            width: parent.width
            visible: text.length > 0
            text: placemark === null ? "" : placemark.openingHours
            icon: "qrc:/material/access_time.svg"
        }

        IconText {
            width: parent.width
            visible: text.length > 0
            text: placemark === null ? "" : placemark.coordinates
            icon: "qrc:/material/place.svg"
        }
    }

    Image {
        id: bookmarkButton
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: Screen.pixelDensity * 2
        visible: root.height > 0

        property bool bookmark: false

        width: Screen.pixelDensity * 6
        height: width
        sourceSize.height: height
        sourceSize.width: width
        source: bookmark ? "qrc:/material/star.svg" : "qrc:/material/star_border.svg"

        MouseArea {
            id: touchArea
            anchors.fill: parent
            onClicked: {
                if (bookmarkButton.bookmark) {
                    bookmarks.removeBookmark(root.placemark.longitude, root.placemark.latitude)
                } else {
                    bookmarks.addBookmark(root.placemark, "Default")
                }
                bookmarkButton.bookmark = !bookmarkButton.bookmark
            }
        }
    }

    function ensureRouteHasDeparture() {
        if (routing.routeRequestModel.count === 0) {
            if (marbleMaps.positionAvailable) {
                routing.addViaByPlacemark(marbleMaps.currentPosition)
            }
        }
    }
}
