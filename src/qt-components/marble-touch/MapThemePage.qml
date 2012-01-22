// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import QtMultimediaKit 1.1
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

Page {
    id: preferencesPage

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
    }

    NewstuffModel {
        id: themeInstallModel
        provider: "http://edu.kde.org/marble/newstuff/maps-4.5.xml"
    }

    ListView {
        id: themeView
        anchors.fill: parent
        model: themeInstallModel
        delegate: themeDelegate
        highlight: Rectangle { radius: 5; color: "lightsteelblue" }
        highlightMoveDuration: 200
        spacing: 10
    }

    ScrollDecorator {
        flickableItem: themeView
    }

    Component {
        id: themeDelegate

        Item {
            id: delegateRoot
            width: parent.width
            height: row.height
            property bool installing: false
            property bool selected: ListView.view.currentIndex === idx
            property int idx: index

            Row {
                id: row
                spacing: 10

                Item {
                    width: previewIcon.width
                    height: Math.max(previewIcon.height + (delegateRoot.selected ? versionLabel.height + 5 : 0), textItem.height)

                    Image {
                        id: previewIcon
                        anchors.top: parent.top
                        source: preview
                        width: 136
                        height: 136
                    }

                    Label {
                        id: versionLabel
                        width: parent.width
                        anchors.top: previewIcon.bottom
                        anchors.topMargin: 5
                        visible: delegateRoot.selected
                        text: "<p><font size=\"-1\">Version: " + version + "<br />" + releasedate + "</font></p>"
                    }
                }

                Item {
                    id: textItem
                    width: delegateRoot.width - row.spacing - previewIcon.width - 10
                    height: nameLabel.height + summaryLabel.height + (delegateRoot.selected ? installButton.height + 10 : 0)

                    Label {
                        id: nameLabel
                        width: parent.width
                        text: display
                    }

                    Label {
                        id: summaryLabel
                        width: parent.width
                        anchors.top: nameLabel.bottom
                        property string details: "<p><font size=\"-1\">Author: " + author + "</font></p><p><font size=\"-1\">License: " + licence + "</font></p>"
                        text: "<p><font size=\"-1\">" + summary + (delegateRoot.selected ? details : "") + "</font></p>"
                    }

                    ProgressBar {
                        id: progressBar
                        visible: delegateRoot.installing
                        anchors.top: summaryLabel.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 5
                        minimumValue: 0.0
                        maximumValue: 1.0
                        indeterminate: true

                        Connections {
                            target: themeInstallModel
                            onInstallationProgressed: {
                                if (index === delegateRoot.idx) {
                                    progressBar.indeterminate = false
                                    progressBar.value = progress
                                }
                            }
                            onInstallationFinished: {
                                delegateRoot.installing = false
                            }
                        }

                        Connections {
                            target: themeInstallModel
                            onInstallationFinished: {
                                delegateRoot.installing = false
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: delegateRoot.ListView.view.currentIndex = delegateRoot.idx
                    }

                    Button {
                        id: installButton
                        text: "Install"
                        anchors.top: summaryLabel.bottom
                        anchors.left: parent.left
                        anchors.margins: 5
                        width: parent.width / 2 - 5
                        visible: delegateRoot.selected && !delegateRoot.installing
                        onClicked: {
                            delegateRoot.installing = true
                            themeInstallModel.install(delegateRoot.idx)
                        }
                    }

                    Button {
                        text: "Remove"
                        anchors.top: summaryLabel.bottom
                        anchors.right: parent.right
                        anchors.left: installButton.right
                        anchors.margins: 5
                        width: parent.width / 2 - 5
                        visible: delegateRoot.selected && !delegateRoot.installing
                        enabled: false
                        onClicked: {
                            /** @todo: Implement */
                            delegateRoot.installing = true
                            themeInstallModel.uninstall(delegateRoot.idx)
                        }
                    }
                }
            }
        }
    }
}
