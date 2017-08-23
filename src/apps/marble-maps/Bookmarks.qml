//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.8
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import QtQml.Models 2.1
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.0 as Kirigami

import org.kde.marble 0.20
Kirigami.ScrollablePage {
    id: bookmarkPage
    title: "Bookmarks"

    property int currentIndex: 0
    property var marbleQuickItem: null

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
        anchors {
            fill: parent
            margins: Screen.pixelDensity * 2
        }

        Text {
            text: "<h3>Bookmarks</h3>"
        }

        ListView {
            id: bookmarksView
            width: parent.width
            interactive: false
            spacing: 5
            height: contentHeight

            model: DelegateModel {
                id: visualModel
                model: bookmarks.model
                delegate: MouseArea {
                    id: delegateRoot
                    property int visualIndex: DelegateModel.itemsIndex
                    width: bookmarksView.width
                    height:  100
                    drag.target: bookmarkRectangle
                    drag.axis: Drag.YAxis

                    SwipeDelegate {
                        id: delegate
                        text: model.display
                        width: parent.width

                        contentItem: Rectangle {
                            id: bookmarkRectangle
                            width: parent.width
                            height: Screen.pixelDensity * 2 + Math.max(text.height, imageButton.height)
                            color: palette.basefconte
                            anchors {
                                horizontalCenter: parent.horizontalCenter;
                            }

                            Drag.active: delegateRoot.drag.active
                            Drag.source: delegateRoot
                            Drag.hotSpot.x: 36
                            Drag.hotSpot.y: 36

                            Text {
                                id: text
                                anchors {
                                    left: imageButton.right
                                    right: dragButton.left
                                    leftMargin: parent.width * 0.05
                                    verticalCenter: parent.verticalCenter
                                }
                                elide: Text.ElideMiddle
                                text: model.display
                                verticalAlignment: Qt.AlignVCenter

                                font.pointSize: 12
                                color: palette.text
                            }

                            Image {
                                id: imageButton
                                anchors {
                                    left: bookmarksView.left
                                    verticalCenter: parent.verticalCenter
                                }
                                source: "qrc:///material/place.svg"
                                width: Screen.pixelDensity * 6
                                verticalAlignment: Qt.AlignVCenter

                                height: width
                                smooth: true
                            }

                            ColorOverlay{
                                anchors.fill: imageButton
                                source: imageButton
                                color: palette.highlight
                            }

                            Image {
                                id: dragButton
                                anchors{
                                    verticalCenter: parent.verticalCenter
                                    right: parent.right
                                }
                                width: Screen.pixelDensity * 6
                                height: width
                                source: "qrc:///material/drag.png"
                                verticalAlignment: Qt.AlignVCenter

                                smooth: true
                            }
                        }

                        background: Rectangle {
                            width: parent.width
                            height: parent.height
                            opacity: enabled ? 1 : 0.3
                            color: delegate.down ? "#dddedf" : "transparent"
                        }


                        swipe.behind: Rectangle {
                            id: rect
                            width: parent.width
                            height: Screen.pixelDensity * 4
                            clip: true
                            color: SwipeDelegate.pressed ? "#555" : "transparent"

                            Label {
                                font.family: "Fontello"
                                text: delegate.swipe.complete ? "\ue805" // icon-cw-circled
                                                              : "\ue801" // icon-cancel-circled-1
                                anchors.fill: parent
                                horizontalAlignment: Qt.AlignRight

                                opacity: 2 * -delegate.swipe.position
                                color: Material.Red
                                Behavior on color { ColorAnimation { } }
                            }

                            Rectangle{
                                width: parent.width
                                color: "#333333"
                                height: parent.height
                                z: 200
                                Text {
                                    font.pointSize: 10
                                    text: qsTr("Removed")
                                    color: "white"
                                    padding: 20
                                    anchors.fill: parent
                                    horizontalAlignment: Qt.AlignRight
                                    opacity: delegate.swipe.complete ? 1 : 0
                                    Behavior on opacity {
                                        NumberAnimation {}
                                    }
                                }

                            }

                            SwipeDelegate.onClicked: delegate.swipe.close()
                            SwipeDelegate.onPressedChanged: undoTimer.stop()
                        }

                        Timer {
                            id: undoTimer
                            interval: 3600
                            onTriggered: {
                                var currentBookmark = bookmarks.placemark(index)
                                bookmarks.removeBookmark(currentBookmark.longitude, currentBookmark.latitude)
                            }
                        }

                        swipe.onCompleted: undoTimer.start()

                        states: [
                            State {
                                when: bookmarkRectangle.Drag.active
                                ParentChange {
                                    target: bookmarkRectangle
                                    parent: column
                                }

                                AnchorChanges {
                                    target: bookmarkRectangle;
                                    anchors.horizontalCenter: undefined;
                                    anchors.verticalCenter: undefined
                                }
                            }
                        ]
                    }

                    DropArea {
                        anchors.fill: parent
                        onEntered: visualModel.items.move(drag.source.visualIndex, delegateRoot.visualIndex)
                    }
                }
            }

            remove: Transition {
                SequentialAnimation {
                    PauseAnimation { duration: 125 }
                    NumberAnimation { property: "height"; to: 0; easing.type: Easing.InOutQuad }
                }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; easing.type: Easing.OutQuad }
            }
        }

        Column {
            visible: bookmarksView.model.count === 0
            width: parent.width

            Text {
                width: 0.8 * parent.width
                font.pointSize: 18
                color: paletteDisabled.text
                text: qsTr("Your bookmarks will appear here.")
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                elide: Text.ElideRight
            }

            Image {
                anchors.right: parent.right
                anchors.bottom: column.bottom
                width: 0.3 * parent.width
                fillMode: Image.PreserveAspectFit
                source: "qrc:/konqi/books.png"
            }
        }
    }

    Bookmarks {
        id: bookmarks
        map: marbleQuickItem
    }
}


