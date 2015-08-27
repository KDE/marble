//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2

import org.kde.edu.marble 0.20

Item {
    id: root

    property var routingManager: null
    property alias routingProfile: profileSelector.selectedProfile

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: palette.base
    }

    ProfileSelectorMenu {
        id: profileSelector
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: Screen.pixelDensity * 2
        }
    }

    ListView {
        id: waypointList
        anchors {
            top: profileSelector.bottom
            left: parent.left
            right: parent.right
            bottom: navigationControls.top
            margins: Screen.pixelDensity * 2
        }

        delegate: Rectangle {
            width: parent.width
            height: Screen.pixelDensity * 2 + Math.max(text.height, image.height)
            color: touchArea.pressed || waypointList.currentIndex === index ? palette.highlight : palette.base

            WaypointImage {
                id: image
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
                anchors.rightMargin: Screen.pixelDensity * 4
                anchors.verticalCenter: parent.verticalCenter
                visible: index === waypointList.currentIndex
                imageSource: "qrc:///down.png"
                enabled: index+1 < routingManager.waypointCount()
                onClicked: {
                    routingManager.swapVias(index, index+1);
                    waypointList.currentIndex++;
                }
            }

            NavigationSetupButton {
                id: deleteButton
                anchors.right: parent.right
                anchors.rightMargin: Screen.pixelDensity * 1
                anchors.verticalCenter: parent.verticalCenter
                visible: index === waypointList.currentIndex
                imageSource: "qrc:///delete.png"
                onClicked: {
                    routingManager.removeVia(index);
                    waypointList.currentIndex--;
                }
            }
        }
    }

    Item {
        id: navigationControls
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: Screen.pixelDensity * 2
        }
        height: navigationButton.height

        Button {
            id: navigationButton
            text: "Start Navigation"
            onClicked: {
                itemStack.pop();
                navigationManager.marbleItem = marbleMaps;
                itemStack.push(navigationManager);
            }
        }
    }

    onRoutingManagerChanged: {waypointList.model = routing.routeRequestModel;}

    Keys.onBackPressed: {
        event.accepted = itemStack.pop();
    }
}
