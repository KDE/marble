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
import QtGraphicalEffects 1.0
import QtQuick.Window 2.2

import org.kde.edu.marble 0.20

Item {
    id: root

    property alias type: image.type
    property int xPos: 0
    property int yPos: 0
    property alias menuObjectsCount: menu.menuObjectsCount
    property alias showAsDestination: menu.showAsDestination
    property alias showAsDeparture: menu.showAsDeparture
    property alias showAsWaypoint: menu.showAsWaypoint
    property alias showAsPlacemark: menu.showAsPlacemark
    property int index: -1

    signal clicked(var index, var type)
    signal menuItemSelected(var index, var type, var selection)

    width: Screen.pixelDensity * 15
    height: width
    x: xPos - 0.5 * width
    y: yPos - 0.5 * height

    WaypointImage {
        id: image
        onClicked: {
            if (menu.visible) {
                menu.visible = false;
            }
            else {
                openMenu();
            }
        }
        anchors {
            bottom: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }

        x: 0.5 * parent.width - 0.5 * width
        y: 0.5 * parent.height - height
    }

    CircularMenu {
        id: menu
        visible: false
        anchors {
            centerIn: parent
        }
        onSelected: {
            menuItemSelected(index, image.type, newType);
            visible = false;
        }
    }

    function openMenu()
    {
        menuObjectsCount = 4;
        if (type == "searchResult") {
            showAsPlacemark = false;
            if (waypointCount() < 2) {
                showAsWaypoint = false;
                menuObjectsCount--;
            }
            else {
                showAsWaypoint = true;
            }
            showAsDestination = true;
            showAsDeparture = true;
        }
        else {
            if (type == "destination") {
                showAsDestination = false;
                showAsDeparture = true;
                if (waypointCount() < 3) {
                    showAsWaypoint = false;
                    menuObjectsCount--;
                }
                else {
                    showAsWaypoint = true;
                }
                showAsPlacemark = true;
            }
            else if (type == "departure") {
                showAsDestination = true;
                showAsDeparture = false;
                if (waypointCount() < 3) {
                    showAsWaypoint = false;
                    menuObjectsCount--;
                }
                else {
                    showAsWaypoint = true;
                }
                showAsPlacemark = true;
            }
            else if (type == "waypoint") {
                showAsDestination = true;
                showAsDeparture = true;
                showAsWaypoint = false;
                showAsPlacemark = true;
            }
        }
        menu.organizeMenu();
        menu.visible = true;
    }

    function closeMenu()
    {
        menu.visible = false;
    }
}
