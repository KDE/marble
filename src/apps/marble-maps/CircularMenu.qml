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

Item {
    id: root

    signal selected(var newType)
    property int xPos: 0
    property int yPos: 0
    property int menuObjectsCount: 0
    property bool showAsDestination: false
    property bool showAsDeparture: false
    property bool showAsWaypoint: false
    property bool showAsPlacemark: false

    width: Screen.pixelDensity * 15
    height: width

    Image {
        id: background
        anchors.fill: parent
        source: "qrc:///circular_menu_backdrop.png"

        WaypointImage {
            id: destination
            visible: showAsDestination
            onClicked: root.selected(type);
            type: 'destination'
        }

        WaypointImage {
            id: departure
            visible: showAsDeparture
            onClicked: root.selected(type);
            type: 'departure'
        }

        WaypointImage {
            id: waypoint
            visible: showAsWaypoint
            onClicked: root.selected(type);
            type: 'waypoint'
        }

        WaypointImage {
            id: searchResult
            visible: showAsPlacemark
            onClicked: root.selected(type);
            type: 'searchResult'
        }

        Image {
            id: remove
            width: searchResult.width * 0.7
            height: width
            source: "qrc:///delete_white.png"

            MouseArea {
                id: touchArea
                anchors.fill: parent
                onClicked: root.selected("remove")
            }
        }
    }

    function organizeMenu() {
        if (menuObjectsCount == 3) {
            if (!showAsPlacemark) {
                remove.x = 0.73 * width - 0.5 * remove.width;
                remove.y = 0.25 * height - 0.5 * remove.height;

                departure.x = 0.27 * width - 0.5 * departure.width;
                departure.y = 0.25 * height - 0.5 * departure.height;

                destination.x = 0.5 * width - 0.5 * destination.width;
                destination.y = 0.83 * height - 0.5 * destination.height;
                return;
            }
            else if (!showAsDestination) {
                remove.x = 0.73 * width - 0.5 * remove.width;
                remove.y = 0.25 * height - 0.5 * remove.height;

                departure.x = 0.27 * width - 0.5 * departure.width;
                departure.y = 0.25 * height - 0.5 * departure.height;

                searchResult.x = 0.5 * width - 0.5 * searchResult.width;
                searchResult.y = 0.83 * height - 0.5 * searchResult.height;
                return;
            }
            else if (!showAsDeparture) {
                remove.x = 0.73 * width - 0.5 * remove.width;
                remove.y = 0.25 * height - 0.5 * remove.height;

                searchResult.x = 0.27 * width - 0.5 * searchResult.width;
                searchResult.y = 0.25 * height - 0.5 * searchResult.height;

                destination.x = 0.5 * width - 0.5 * destination.width;
                destination.y = 0.83 * height - 0.5 * destination.height;
                return;
            }
        }
        else if (!showAsDestination) {
            departure.x = 0.17 * width - 0.5 * departure.width
            departure.y = 0.5 * height - 0.5 * departure.height

            searchResult.x = 0.5 * width - 0.5 * searchResult.width;
            searchResult.y = 0.83 * height - 0.5 * searchResult.height;

            waypoint.x = 0.5 * width - 0.5 * waypoint.width;
            waypoint.y = 0.17 * height - 0.5 * waypoint.height;
        }
        else if (!showAsDeparture) {
            searchResult.x = 0.17 * width - 0.5 * searchResult.width
            searchResult.y = 0.5 * height - 0.5 * searchResult.height

            destination.x = 0.5 * width - 0.5 * destination.width;
            destination.y = 0.83 * height - 0.5 * destination.height;

            waypoint.x = 0.5 * width - 0.5 * waypoint.width;
            waypoint.y = 0.17 * height - 0.5 * waypoint.height;
        }
        else if (!showAsWaypoint) {
            departure.x = 0.17 * width - 0.5 * departure.width
            departure.y = 0.5 * height - 0.5 * departure.height

            destination.x = 0.5 * width - 0.5 * destination.width;
            destination.y = 0.83 * height - 0.5 * destination.height;

            searchResult.x = 0.5 * width - 0.5 * searchResult.width;
            searchResult.y = 0.17 * height - 0.5 * searchResult.height;
        }
        else if (!showAsPlacemark) {
            departure.x = 0.17 * width - 0.5 * departure.width
            departure.y = 0.5 * height - 0.5 * departure.height

            destination.x = 0.5 * width - 0.5 * destination.width;
            destination.y = 0.83 * height - 0.5 * destination.height;

            waypoint.x = 0.5 * width - 0.5 * waypoint.width;
            waypoint.y = 0.17 * height - 0.5 * waypoint.height;
        }
        remove.x = 0.83 * width - 0.5 * remove.width;
        remove.y = 0.5 * height - 0.5 * remove.height;

    }
}
