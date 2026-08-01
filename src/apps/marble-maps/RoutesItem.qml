// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Dennis Nienhüser <nienhueser@kde.org>
//

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

import org.kde.marble

Item {
    id: root
    property alias model: listView.model
    property alias count: listView.count
    property Placemark placemark

    implicitHeight: Math.min(0.75 * Screen.height, listView.contentHeight)

    signal highlightChanged(int oid, bool enabled)

    ListView {
        id: listView
        anchors.fill: parent
        contentWidth: width

        model: root.placemark ? root.placemark.routeRelationModel : undefined
        clip: true
        spacing: Screen.pixelDensity * 2

        delegate: Item {
            id: routeCard

            required property int index
            required property string iconSource
            required property string description;
            required property string network
            required property string routeColor
            required property string textColor
            required property string routeFrom
            required property string routeTo
            required property string routeRef
            required property list<string> routeVia
            required property int oid
            required property bool routeVisible

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
                            source: routeCard.iconSource
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
                                color: routeCard.routeColor
                                Text {
                                    anchors.left: parent.left
                                    anchors.margins: Screen.pixelDensity * 0.5
                                    clip: true
                                    font.pointSize: 16
                                    text: routeCard.routeRef
                                    color: routeCard.textColor
                                    width: parent.width
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                }
                            }

                            Text {
                                visible: text.length > 2
                                font.pointSize: 16
                                text: "● " + routeCard.routeFrom
                                width: parent.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Repeater {
                                model: routeCard.expanded ? routeCard.routeVia : undefined
                                Text {
                                    required property string modelData

                                    font.pointSize: 16
                                    text: "○ " + modelData
                                    width: parent.width
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                }
                            }

                            Text {
                                visible: text.length > 2
                                font.pointSize: 16
                                text: "● " + routeCard.routeTo
                                width: parent.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Text {
                                visible: routeCard.expanded
                                font.pointSize: 14
                                text: "Network: " + routeCard.network
                                width: parent.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }

                            Text {
                                visible: routeCard.expanded
                                font.pointSize: 14
                                text: routeCard.description
                                width: parent.width
                                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                routeCard.expanded = !routeCard.expanded
                                listView.currentIndex = routeCard.index
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

                            checked: routeCard.routeVisible
                            onClicked: root.highlightChanged(routeCard.oid, checked)
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
