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

        Text {
            id: helpText
            visible: waypointList.count < 2
            color: "gray"
            text: "Search for places to integrate them into a route."
        }

        ListView {
            id: waypointList
            anchors {
                left: parent.left
                right: parent.right
            }

            height: Math.min(0.4 * Screen.height, contentHeight)
            clip: true
            model: routing.routeRequestModel

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
                        if (index === waypointList.currentIndex) {
                            waypointList.currentIndex =  -1
                        } else {
                            waypointList.currentIndex =  index
                            marbleMaps.centerOn(longitude, latitude)
                        }
                    }
                }

                Row {
                    id: buttonsRow
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    ImageButton {
                        id: upButton
                        anchors.verticalCenter: parent.verticalCenter
                        visible: index > 0 && index === waypointList.currentIndex
                        imageSource: "qrc:///up.png"
                        onClicked: {
                            routing.swapVias(index, index-1);
                            waypointList.currentIndex--;
                        }
                    }

                    ImageButton {
                        id: downButton
                        anchors.verticalCenter: parent.verticalCenter
                        visible: index+1 < routing.routeRequestModel.count && index === waypointList.currentIndex
                        imageSource: "qrc:///down.png"
                        onClicked: {
                            routing.swapVias(index, index+1);
                            waypointList.currentIndex++;
                        }
                    }

                    ImageButton {
                        id: deleteButton
                        anchors.verticalCenter: parent.verticalCenter
                        visible: index === waypointList.currentIndex
                        imageSource: "qrc:///delete.png"
                        onClicked: {
                            routing.removeVia(index);
                            waypointList.currentIndex = Math.max(0, waypointList.currentIndex-1);
                        }
                    }
                }
            }
        }
    }
}
