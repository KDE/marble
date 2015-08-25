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

    property int selectedItem: -1
    property var routingManager: null

    signal edited()

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
            anchors {
                top: parent.top
                bottom: buttons.top
                left: parent.left
                right: parent.right
            }

            delegate: Rectangle {
                width: parent.width
                height: text.height * 2
                color: touchArea.pressed || root.selectedItem == index ? palette.highlight : palette.base

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
                        root.selectedItem = index;
                    }
                }
            }
        }

        RowLayout {
            id: buttons
            height: Screen.pixelDensity * 10
            anchors{
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            spacing: 0
            visible: true

            NavigationSetupButton {
                imageSource: "qrc:///up.png"
                text: qsTr("Sooner")
                onClicked: {
                    if (routingManager && selectedItem > 0) {
                        routingManager.swapVias(selectedItem, selectedItem-1);
                        selectedItem--;
                        edited();
                    }
                }
            }

            NavigationSetupButton {
                imageSource: "qrc:///down.png"
                text: qsTr("Later")
                onClicked: {
                    if (routingManager && selectedItem < routingManager.waypointCount()-1) {
                        routingManager.swapVias(selectedItem, selectedItem+1);
                        selectedItem++;
                        edited();
                    }
                }
            }

            NavigationSetupButton {
                imageSource: "qrc:///delete.png"
                text: qsTr("Delete")
                onClicked: {
                    if (routingManager && selectedItem >= 0)
                    {
                        routingManager.removeVia(selectedItem);
                        selectedItem = -1;
                        edited();
                    }
                }
            }
        }
    }

    onRoutingManagerChanged: {waypointList.model = routing.routeRequestModel;}
}
