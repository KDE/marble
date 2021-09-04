//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.8
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import QtQml.Models 2.1
import QtQuick.Controls.Material 2.1
import QtGraphicalEffects 1.0

import org.kde.kirigami 2.0 as Kirigami

import org.kde.marble 0.20

Kirigami.Page {
    id: bookmarkPage
    title: "Bookmarks"

    property variant marbleQuickItem
    signal backTriggered()

    Flickable {
        id: root
        anchors.fill: parent
        property int currentIndex: 0

        SystemPalette {
            id: palette
            colorGroup: SystemPalette.Active
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
                interactive: false
                width: parent.width
                spacing: 5
                height: contentHeight

                model: DelegateModel {
                    id: visualModel
                    model: bookmarks.model

                    delegate:
                        MouseArea {
                        id: delegateRoot
                        propagateComposedEvents: true
                        property int visualIndex: DelegateModel.itemsIndex
                        width: bookmarksView.width
                        height:  100
                        drag.target: icon
                        drag.axis: Drag.YAxis

                        SwipeDelegate {
                            id: delegate
                            text: model.display
                            width: parent.width
                            height: Screen.pixelDensity * 2 + Math.max(bookmarkText.height, imageButton.height)

                            contentItem: Rectangle {
                                id: icon
                                width: parent.width
                                height: Screen.pixelDensity + Math.max(bookmarkText.height, imageButton.height)

                                anchors {
                                    horizontalCenter: parent.horizontalCenter;
                                    verticalCenter: parent.verticalCenter
                                }

                                Drag.active: delegateRoot.drag.active
                                Drag.source: delegateRoot
                                Drag.hotSpot.x: 50
                                Drag.hotSpot.y: 50
                                color: "transparent"

                                Text{
                                    id: bookmarkText
                                    anchors.left: imageButton.right
                                    leftPadding: 5
                                    text: delegate.text
                                    elide: Text.ElideRight
                                    horizontalAlignment: Text.AlignLeft
                                    anchors.verticalCenter: parent.verticalCenter
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
                            }

                            swipe.behind: Item {
                                width: parent.width
                                height: parent.height

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
                                        verticalAlignment: Qt.AlignVCenter

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
                                interval: 1600
                                onTriggered: {
                                    var currentBookmark = bookmarks.placemark(index)
                                    bookmarks.removeBookmark(currentBookmark.longitude, currentBookmark.latitude)
                                }
                            }

                            swipe.onCompleted: undoTimer.start()
                            swipe.onClosed: delegate.swipe.close()
                            swipe.transition: Transition {
                                SmoothedAnimation { velocity: 3; easing.type: Easing.InOutCubic }
                            }

                            states: [
                                State {
                                    when: icon.Drag.active
                                    ParentChange {
                                        target: icon
                                        parent: root
                                    }

                                    AnchorChanges {
                                        target: icon;
                                        anchors.horizontalCenter: undefined;
                                        anchors.verticalCenter: undefined
                                    }
                                }
                            ]

                            background: Rectangle {
                                color: "transparent"
                            }
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
                    SequentialAnimation {
                        PauseAnimation { duration: 125 }
                        NumberAnimation { property: "y"; easing.type: Easing.InOutQuad }
                    }
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

    }

    Bookmarks {
        id: bookmarks
        map: marbleQuickItem
    }
}
