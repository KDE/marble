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
import QtQuick.Layouts 1.2

import org.kde.edu.marble 0.20

Item {
    id: root

    property var routingManager: null

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: palette.base

        ListView {
            id: waypointList
            anchors.fill: parent

            delegate: Rectangle {
                width: parent.width
                height: text.height * 2
                color: touchArea.pressed || waypointList.currentIndex === index ? palette.highlight : palette.base

                WaypointImage {
                    id: image
                    width: parent.height
                    height: width
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                    }

                    type: index === 0 ? "departure" : (index === waypointList.count-1 ? "destination" : "waypoint")
                }

                Text {
                    id: text
                    anchors {
                        left: image.right
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                        leftMargin: parent.width * 0.05
                    }
                    wrapMode: Text.Wrap
                    text: name
                    font.pointSize: 18
                    color: palette.text
                }

                MouseArea {
                    id: touchArea
                    anchors.fill: parent
                    onClicked: {
                        waypointList.currentIndex = index;
                    }
                }

                NavigationSetupButton {
                    anchors.right: upButton.left
                    anchors.verticalCenter: parent.verticalCenter
                    visible: index === waypointList.currentIndex
                    imageSource: "qrc:///up.png"
                    enabled: index > 0
                    onClicked: {
                        routingManager.swapVias(index, index-1);
                        waypointList.currentIndex--;
                    }
                }

                NavigationSetupButton {
                    id: upButton
                    anchors.right: deleteButton.left
                    anchors.rightMargin: Screen.pixelDensity * 6
                    anchors.verticalCenter: parent.verticalCenter
                    visible: index === waypointList.currentIndex
                    imageSource: "qrc:///down.png"
                    width: 32
                    enabled: index+1 < routingManager.waypointCount()
                    onClicked: {
                        routingManager.swapVias(index, index+1);
                        waypointList.currentIndex++;
                    }
                }

                NavigationSetupButton {
                    id: deleteButton
                    anchors.right: parent.right
                    anchors.rightMargin: Screen.pixelDensity * 2
                    anchors.verticalCenter: parent.verticalCenter
                    visible: index === waypointList.currentIndex
                    imageSource: "qrc:///delete.png"
                    width: 32
                    onClicked: {
                        routingManager.removeVia(index);
                        waypointList.currentIndex--;
                    }
                }
            }
        }
    }

    onRoutingManagerChanged: {waypointList.model = routing.routeRequestModel;}
}
