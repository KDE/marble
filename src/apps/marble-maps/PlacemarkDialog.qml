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

import org.kde.edu.marble 0.20

Item {
    id: root

    property var placemark: null
    property bool condensed: true
    property string actionIconSource: routeEditor.currentProfileIcon

    height: placemark === null ? 0 : Screen.pixelDensity * 6 + infoLayout.height

    function addToRoute() {
        ensureRouteHasDeparture()
        routing.addViaByPlacemarkAtIndex(routing.waypointCount(), placemark)
        routing.clearSearchResultPlacemarks()
        placemark = null
        itemStack.state = "routing"
    }

    onPlacemarkChanged: {
        itemStack.state = placemark ? "place" : ""
    }

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        anchors.fill: parent
        color: palette.base
    }

    Column {
        id: infoLayout
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: Screen.pixelDensity * 2
        }

        IconText {
            id: name
            width: parent.width
            text: placemark === null ? "" : placemark.name
            maximumLineCount: 2
            font.pointSize: 20
        }

        IconText {
            width: parent.width
            text: placemark === null ? "" : placemark.description
            maximumLineCount: condensed ? 4 : undefined

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    condensed = !condensed
                }
            }
        }

        IconText {
            width: parent.width
            visible: text.length > 0 && (!condensed || name.text === "")
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
            text: placemark === null ? "" : placemark.fuelDetails
            icon: "qrc:/material/gas_station.svg"
        }

        IconText {
            width: parent.width
            visible: text.length > 0
            text: placemark === null ? "" : placemark.openingHours
            icon: "qrc:/material/access_time.svg"
        }

        IconText {
            width: parent.width
            visible: text.length > 0 && (!condensed || name.text === "")
            text: placemark === null ? "" : placemark.coordinates
            icon: "qrc:/material/place.svg"
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
