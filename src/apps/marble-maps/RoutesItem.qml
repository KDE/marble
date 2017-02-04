//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0


Item {
    id: root
    property alias model: listView.model
    property alias count: listView.count

    implicitHeight: Math.min(0.75 * Screen.height, listView.contentHeight)

    signal highlightChanged(int oid, bool enabled)

    ListView {
        id: listView
        anchors.fill: parent
        contentWidth: width

        model: placemark ? placemark.routeRelationModel : undefined
        clip: true
        spacing: Screen.pixelDensity * 2

        delegate: Item {
            id: routeCard
            property bool expanded: false

            width: parent.width
            height: rectangle.height + dropShadow.verticalOffset + dropShadow.radius
            Rectangle {
                id: rectangle
                width: parent.width - dropShadow.horizontalOffset - dropShadow.radius
                height: column.height
                radius: Screen.pixelDensity * 1

                Column {
                    id: column
                    width: parent.width
                    spacing: Screen.pixelDensity * 1

                    Item {
                        width: parent.width
                        height: Math.max(icon.height, textColumn.height)

                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }

                        Image {
                            id: icon
                            anchors.left: parent.left
                            source: iconSource
                            height: Screen.pixelDensity * 6
                            width: height
                            sourceSize.height: Screen.pixelDensity * 6
                            sourceSize.width: width
                            fillMode: Image.PreserveAspectFit
                        }

                        Column {
                            id: textColumn
                            anchors.left: icon.right
                            anchors.right: parent.right

                            Rectangle {
                                width: parent.width
                                height: childrenRect.height
                                color: routeColor
                                Text {
                                    anchors.left: parent.left
                                    anchors.margins: Screen.pixelDensity * 0.5
                                    clip: true
                                    font.pointSize: 16
                                    text: routeRef
                                    color: textColor
                                    width: parent.width
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                }
                            }

                            Text {
                                visible: text.length > 2
                                font.pointSize: 16
                                text: "● " + routeFrom
                                width: parent.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Repeater {
                                model: routeCard.expanded ? routeVia : undefined
                                Text {
                                    font.pointSize: 16
                                    text: "○ " + modelData
                                    width: parent.width
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                }
                            }

                            Text {
                                visible: text.length > 2
                                font.pointSize: 16
                                text: "● " + routeTo
                                width: parent.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Text {
                                visible: routeCard.expanded
                                font.pointSize: 14
                                text: "Network: " + network
                                width: parent.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Text {
                                visible: routeCard.expanded
                                font.pointSize: 14
                                text: description
                                width: parent.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                routeCard.expanded = !routeCard.expanded
                                listView.currentIndex = index
                            }
                        }

                        Behavior on height {
                            NumberAnimation {
                                duration: 200
                                easing.type: Easing.OutQuart
                            }
                        }
                    }

                    Item {
                        visible: routeCard.expanded

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.rightMargin: Screen.pixelDensity * 4
                        height: childrenRect.height

                        Switch {
                            id: highlightSwitch
                            anchors.right: switchText.left
                            anchors.verticalCenter: switchText.verticalCenter

                            checked: routeVisible
                            onClicked: root.highlightChanged(oid, checked)
                        }

                        Text {
                            id: switchText
                            anchors.right: parent.right
                            text: "Highlight in Map"

                            MouseArea {
                                anchors.fill: parent
                                onClicked: highlightSwitch.checked = !highlightSwitch.checked
                            }
                        }
                    }
                }
            }

            DropShadow {
                id: dropShadow
                anchors.fill: rectangle
                horizontalOffset: 4
                verticalOffset: 4
                radius: 4.0
                samples: 17
                color: "#40000000"
                source: rectangle
            }
        }

        ScrollBar.vertical: ScrollBar {}
    }
}
