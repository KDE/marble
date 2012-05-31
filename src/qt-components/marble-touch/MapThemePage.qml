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
import org.kde.edu.marble.qtcomponents 0.12

Page {
    id: preferencesPage

    tools: ToolBarLayout {
        MarbleToolIcon {
            iconSource: main.icon( "actions/go-previous-view", 48 );
            onClicked: pageStack.pop()
        }
    }

    NewstuffModel {
        id: themeInstallModel
        provider: "http://edu.kde.org/marble/newstuff/maps-4.5.xml"
        registryFile: "~/.kde/share/apps/knewstuff3/marble.knsregistry"
    }

    ListView {
        id: themeView
        anchors.fill: parent
        anchors.margins: 5
        model: themeInstallModel
        delegate: themeDelegate
        highlight: Rectangle { radius: 5; color: "lightsteelblue" }
        highlightMoveDuration: 200
        spacing: 10
        currentIndex: -1
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
            property bool installing: transitioning
            property bool selected: ListView.view.currentIndex === idx
            property bool showDetails: selected || installing
            property int idx: index

            Row {
                id: row
                spacing: 10

                Item {
                    width: previewIcon.width
                    height: Math.max(previewIcon.height + (delegateRoot.showDetails ? versionLabel.height + 5 : 0), textItem.height)

                    Image {
                        id: previewIcon
                        anchors.top: parent.top
                        source: settings.workOffline ? "" : preview
                        width: 136
                        height: 136

                        Image {
                            id: fallbackIcon
                            anchors.centerIn: parent
                            source: "qrc:/icons/activity-virtualglobe.png"
                            width: 128
                            height: 128
                            opacity: (previewIcon.status == Image.Loading || settings.workOffline) ? 1.0 : 0.0
                            Behavior on opacity { NumberAnimation {} }
                        }
                    }

                    Label {
                        id: versionLabel
                        width: parent.width
                        anchors.top: previewIcon.bottom
                        anchors.topMargin: 5
                        visible: delegateRoot.showDetails
                        text: delegateRoot.showDetails ? "<p><font size=\"-1\">Version: " + version + "<br />" + releasedate + "<br />" + formatSize() + "</font></p>" : ""

                        function formatSize() {
                            if (settings.workOffline) {
                                return ""
                            }

                            var length = size

                            if (length<0) {
                                return ""
                            }

                            var unit = 0
                            for (var i=0; i<9 && length >= 1000; ++i) {
                                length /= 1000.0
                                ++unit
                            }

                            switch (unit) {
                            case 0: return length.toFixed(1) + " byte"
                            case 1: return length.toFixed(1) + " kB"
                            case 2: return length.toFixed(1) + " MB"
                            case 3: return length.toFixed(1) + " GB"
                            case 4: return length.toFixed(1) + " TB"
                            case 5: return length.toFixed(1) + " PB"
                            case 6: return length.toFixed(1) + " EB"
                            case 7: return length.toFixed(1) + " ZB"
                            case 8: return length.toFixed(1) + " YB"
                            }
                        }
                    }
                }

                Item {
                    id: textItem
                    width: delegateRoot.width - row.spacing - previewIcon.width - 10
                    height: nameLabel.height + summaryLabel.height + (delegateRoot.showDetails ? installButton.height + 10 : 0)

                    Label {
                        id: nameLabel
                        width: parent.width
                        text: display
                        font.bold: upgradable
                    }

                    Label {
                        id: summaryLabel
                        width: parent.width
                        anchors.top: nameLabel.bottom
                        property string details: "<p><font size=\"-1\">Author: " + author + "</font></p><p><font size=\"-1\">License: " + license + "</font></p>"
                        text: "<p><font size=\"-1\">" + summary + (delegateRoot.showDetails ? details : "") + "</font></p>"
                    }

                    ProgressBar {
                        id: progressBar
                        visible: delegateRoot.installing
                        anchors.top: summaryLabel.bottom
                        width: parent.width - 50
                        anchors.margins: 20
                        minimumValue: 0.0
                        maximumValue: 1.0
                        indeterminate: true

                        Connections {
                            target: themeInstallModel
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
                        iconSource: main.icon( "actions/dialog-cancel", 32 );
                        onClicked: {
                            progressBar.indeterminate = true
                            themeInstallModel.cancel(delegateRoot.idx)
                        }
                    }

                    Button {
                        id: installButton
                        text: installed && upgradable ? "Upgrade" : "Install"
                        enabled: !installed || upgradable
                        anchors.top: summaryLabel.bottom
                        anchors.left: parent.left
                        anchors.margins: 5
                        width: parent.width / 2 - 5
                        visible: delegateRoot.selected && !delegateRoot.installing
                        onClicked: {
                            progressBar.indeterminate = true
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
                        enabled: installed
                        onClicked: {
                            progressBar.indeterminate = true
                            delegateRoot.installing = true
                            themeInstallModel.uninstall(delegateRoot.idx)
                        }
                    }
                }
            }
        }
    }
}
