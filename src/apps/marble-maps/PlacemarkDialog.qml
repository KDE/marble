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
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2

import org.kde.edu.marble 0.20

Item {
    id: root

    property var placemark: null
    property string actionIconSource: viaGroup.current === addDestinationButton ? routeEditor.currentProfileIcon : "qrc:///add.png"

    height: placemark === null ? 0 : Screen.pixelDensity * 4 + Math.max(infoLayout.height, actionsLayout.height)

    function addToRoute() {
        viaGroup.current.execute()
        itemStack.state = "routing"
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
        clip: true
        anchors {
            top: parent.top
            left: parent.left
            right: actionsLayout.left
            margins: Screen.pixelDensity * 2
        }
        height: nameLabel.height + (addressLabel.text == "" ? 0 : addressLabel.height)

        Text {
            id: nameLabel
            width: parent.width
            text: placemark === null ? "" : placemark.name
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
            maximumLineCount: 2
            font.pointSize: 20
        }

        Text {
            id: addressLabel
            width: parent.width
            text: placemark === null ? "" : placemark.address
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
            maximumLineCount: 4
            font.pointSize: 14
        }
    }

    Column {
        id: actionsLayout
        width: rowLayout.width
        height: spacer.height + rowLayout.height
        spacing: Screen.pixelDensity * 2
        anchors {
            right: parent.right
            rightMargin: Screen.pixelDensity * 2
        }

        Item {
            id: spacer
            height: Screen.pixelDensity * 5
            width: 1
        }

        Row {
            id: rowLayout
            anchors.right: parent.right
            spacing: 0

            ExclusiveGroup {
                id: viaGroup
            }

            RouteProfileRadioButton {
                anchors.margins: 0
                visible: routing.routeRequestModel.count === 0
                exclusiveGroup: viaGroup
                imageSource: "qrc:///ic_place_departure.png"
                function execute() {
                    routing.addViaByPlacemarkAtIndex(0, placemark);
                    routing.removeSearchResultPlacemark(placemark);
                    placemark = null;
                }
            }
            RouteProfileRadioButton {
                anchors.margins: 0
                exclusiveGroup: viaGroup
                imageSource: "qrc:///ic_place_via.png"
                function execute() {
                    ensureRouteHasDeparture()
                    routing.addViaByPlacemark(placemark)
                    routing.clearSearchResultPlacemarks();
                    placemark = null;
                }
            }
            RouteProfileRadioButton {
                id: addDestinationButton
                anchors.margins: 0
                checked: true
                exclusiveGroup: viaGroup
                imageSource: "qrc:///ic_place_arrival.png"
                function execute() {
                    ensureRouteHasDeparture()
                    routing.addViaByPlacemarkAtIndex(routing.waypointCount(), placemark)
                    routing.clearSearchResultPlacemarks();
                    placemark = null;
                }
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
