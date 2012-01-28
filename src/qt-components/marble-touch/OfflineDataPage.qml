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
    id: offlineDataPage

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
    }

    OfflineDataModel {
        id: offlineDataModel
    }

    ListView {
        id: dataView
        anchors.fill: parent
        anchors.margins: 5
        model: offlineDataModel
        delegate: dataDelegate
        highlight: Rectangle { radius: 5; color: "lightsteelblue" }
        highlightMoveDuration: 200
        spacing: 10
        section.property: "continent"
        section.delegate: sectionDelegate
        currentIndex: -1
    }

    ScrollDecorator {
        flickableItem: dataView
    }

    Component {
        id: sectionDelegate

        Rectangle {
            color: Qt.rgba(0/255, 102/255, 255/255, 1)
            width: dataView.width
            height: childrenRect.height
            radius: 5

            Label {
                x: 5
                width: parent.width
                text: section
                color: Qt.rgba(238/255, 238/255, 236/255, 1)
            }
        }
    }

    Component {
        id: dataDelegate

        Item {
            id: delegateRoot
            width: parent.width
            height: row.height
            property bool installing: transitioning
            property bool selected: ListView.view.currentIndex === idx
            property bool showDetails: selected || installing
            property int idx: index

            Row {
                id: row
                spacing: 10

                Image {
                    id: previewIcon
                    anchors.verticalCenter: parent.verticalCenter
                    width: 45
                    smooth: true
                    fillMode: Image.PreserveAspectFit
                    source: name.search(/Bicycle/) !== -1 ? "qrc:/icons/routing-bike.svg" : (name.search(/Pedestrian/) !== -1 ? "qrc:/icons/routing-pedestrian.svg" : "qrc:/icons/routing-motorcar.svg")
                }

                Item {
                    id: textItem
                    width: delegateRoot.width - row.spacing - previewIcon.width - 10
                    height: nameLabel.height + (delegateRoot.showDetails ? versionLabel.height + installButton.height + 15 : 0)

                    Label {
                        id: nameLabel
                        width: parent.width
                        text: display
                        font.bold: upgradable
                    }

                    Label {
                        id: versionLabel
                        width: parent.width
                        anchors.top: nameLabel.bottom
                        anchors.topMargin: 5
                        visible: delegateRoot.showDetails
                        property string localVersion: "<font size=\"-1\">Version installed: " + installedreleasedate + "</font>"
                        text: "<font size=\"-1\">Version available: " + releasedate + "</font>" + (installed ? "<br />" + localVersion : "")
                    }

                    ProgressBar {
                        id: progressBar
                        visible: delegateRoot.installing
                        anchors.top: versionLabel.bottom
                        width: parent.width - 50
                        anchors.margins: 20
                        minimumValue: 0.0
                        maximumValue: 1.0
                        indeterminate: true

                        Connections {
                            target: offlineDataModel
                            onInstallationProgressed: {
                                if (newstuffindex === delegateRoot.idx) {
                                    progressBar.indeterminate = false
                                    progressBar.value = progress
                                }
                            }
                            onInstallationFinished: {
                                if (newstuffindex === delegateRoot.idx) {
                                    delegateRoot.installing = false
                                }
                            }
                            onInstallationFailed: {
                                if (newstuffindex === delegateRoot.idx) {
                                    delegateRoot.installing = false
                                }
                            }
                            onUninstallationFinished: {
                                if (newstuffindex === delegateRoot.idx) {
                                    delegateRoot.installing = false
                                }
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: delegateRoot.ListView.view.currentIndex = delegateRoot.idx
                    }

                    ToolButton {
                        anchors.left: progressBar.right
                        anchors.leftMargin: 5
                        anchors.verticalCenter: progressBar.verticalCenter
                        visible: delegateRoot.installing
                        width: 40
                        flat: true
                        iconSource: "image://theme/icon-s-cancel"
                        onClicked: {
                            progressBar.indeterminate = true
                            offlineDataModel.cancel(delegateRoot.idx)
                        }
                    }

                    Button {
                        id: installButton
                        text: installed && upgradable ? "Upgrade" : "Install"
                        enabled: !installed
                        anchors.top: versionLabel.bottom
                        anchors.left: parent.left
                        anchors.margins: 5
                        width: parent.width / 2 - 5
                        visible: delegateRoot.selected && !delegateRoot.installing
                        onClicked: {
                            progressBar.indeterminate = true
                            delegateRoot.installing = true
                            offlineDataModel.install(delegateRoot.idx)
                        }
                    }

                    Button {
                        text: "Remove"
                        anchors.top: versionLabel.bottom
                        anchors.right: parent.right
                        anchors.left: installButton.right
                        anchors.margins: 5
                        width: parent.width / 2 - 5
                        visible: delegateRoot.selected && !delegateRoot.installing
                        enabled: installed
                        onClicked: {
                            progressBar.indeterminate = true
                            delegateRoot.installing = true
                            offlineDataModel.uninstall(delegateRoot.idx)
                        }
                    }
                }
            }
        }
    }
}
