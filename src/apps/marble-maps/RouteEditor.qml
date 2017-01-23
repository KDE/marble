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
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

import org.kde.marble 0.20

Item {
    id: root

    property alias routingProfile: profileSelector.selectedProfile
    property alias currentProfileIcon: profileSelector.profileIcon
    property int currentIndex: 0
    property var routingManager

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
            text: qsTr("Search for places to integrate them into a route.")
        }

        ListView {
            id: waypointList
            anchors {
                left: parent.left
                right: parent.right
            }

            height: Math.min(0.4 * Screen.height, contentHeight)
            clip: true
            model: routingManager.routeRequestModel

            currentIndex: root.currentIndex

            delegate: Rectangle {
                width: parent.width
                height: Screen.pixelDensity * 2 + Math.max(text.height, image.height)
                color: touchArea.pressed || root.currentIndex === index ? palette.highlight : palette.base

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
                        if (index === root.currentIndex) {
                            root.currentIndex =  -1
                        } else {
                            root.currentIndex =  index
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
                        visible: index > 0 && index === root.currentIndex
                        imageSource: "qrc:///up.png"
                        onClicked: {
                            routingManager.swapVias(index, index-1);
                            root.currentIndex--;
                        }
                    }

                    ImageButton {
                        id: downButton
                        anchors.verticalCenter: parent.verticalCenter
                        visible: index+1 < routingManager.routeRequestModel.count && index === root.currentIndex
                        imageSource: "qrc:///down.png"
                        onClicked: {
                            routingManager.swapVias(index, index+1);
                            root.currentIndex++;
                        }
                    }

                    ImageButton {
                        id: deleteButton
                        anchors.verticalCenter: parent.verticalCenter
                        visible: index === root.currentIndex
                        imageSource: "qrc:///delete.png"
                        onClicked: {
                            routingManager.removeVia(index);
                            root.currentIndex = Math.max(0, root.currentIndex-1);
                        }
                    }
                }
            }

            MarbleScrollBar {
                id: scrollBar
                flickableItem: waypointList
            }
        }
    }
}
