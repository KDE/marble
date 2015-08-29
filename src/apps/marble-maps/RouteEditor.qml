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
    property alias currentProfileIcon: profileSelector.profileIcon
    property alias currentIndex: waypointList.currentIndex

    height: visible ? Screen.pixelDensity * 4 + column.height : 0

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        anchors.fill: parent
        color: palette.base
    }

    Column {
        id: column
        spacing: Screen.pixelDensity * 2
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: Screen.pixelDensity * 2
        }

        Item {
            anchors {
                left: parent.left
                right: parent.right
            }

            height: profileSelector.height

            ProfileSelectorMenu {
                id: profileSelector
                anchors.left: parent.left
            }
        }

        ListView {
            id: waypointList
            anchors {
                left: parent.left
                right: parent.right
            }

            height: Math.min(0.4 * Screen.height, contentHeight)
            clip: true

            delegate: Rectangle {
                width: parent.width
                height: Screen.pixelDensity * 2 + Math.max(text.height, image.height)
                color: touchArea.pressed || waypointList.currentIndex === index ? palette.highlight : palette.base

                WaypointImage {
                    id: image
                    anchors {
                        left: parent.left
                        verticalCenter: parent.verticalCenter
                    }

                    type: index === 0 ? "departure" : (index === waypointList.count-1 ? "destination" : "waypoint")
                }

                Text {
                    id: text
                    anchors {
                        left: image.right
                        right: buttonsRow.left
                        leftMargin: parent.width * 0.05
                        verticalCenter: parent.verticalCenter
                    }
                    elide: Text.ElideMiddle
                    text: name
                    font.pointSize: 18
                    color: palette.text
                }

                MouseArea {
                    id: touchArea
                    anchors.fill: parent
                    onClicked: {
                        waypointList.currentIndex = index === waypointList.currentIndex ? -1 : index;
                    }
                }

                Row {
                    id: buttonsRow
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    ImageButton {
                        id: upButton
                        anchors.verticalCenter: parent.verticalCenter
                        visible: index === waypointList.currentIndex
                        imageSource: "qrc:///up.png"
                        enabled: index > 0
                        onClicked: {
                            routingManager.swapVias(index, index-1);
                            waypointList.currentIndex--;
                        }
                    }

                    ImageButton {
                        id: downButton
                        anchors.verticalCenter: parent.verticalCenter
                        visible: index === waypointList.currentIndex
                        imageSource: "qrc:///down.png"
                        enabled: index+1 < routingManager.waypointCount()
                        onClicked: {
                            routingManager.swapVias(index, index+1);
                            waypointList.currentIndex++;
                        }
                    }

                    ImageButton {
                        id: deleteButton
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
        }
    }

    onRoutingManagerChanged: {waypointList.model = routing.routeRequestModel;}
}
