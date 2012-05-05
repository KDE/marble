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
    id: offlineDataPage

    tools: ToolBarLayout {
        MarbleToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
    }

    OfflineDataModel {
        id: offlineDataModel
    }

    Row {
        id: monavStatus
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 10

        visible: !Marble.canExecute("monav-daemon") && !Marble.canExecute("MoNavD")

        MarbleToolIcon {
            id: statusIcon
            anchors.verticalCenter: parent.verticalCenter
            iconId: "applet-error"
        }

        Label {
            anchors.verticalCenter: parent.verticalCenter
            /*readonly*/ property string monavStoreUrl: "http://store.ovi.mobi/content/250322"
            width: parent.width - statusIcon.width
            text: "Please install Monav Routing Daemon (free) to enable offline routing. <a href=\"" + monavStoreUrl + "\">Install now</a>."
            wrapMode: Text.Wrap

            onLinkActivated: Qt.openUrlExternally(link)
        }
    }

    SearchField {
        id: search
        anchors.top: monavStatus.visible ? monavStatus.bottom : parent.top
        anchors.topMargin: 5
        width: parent.width
        onSearch: {
            offlineDataModel.setFilterFixedString(term)
        }
    }

    ButtonRow {
        id: filterRow
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: search.bottom
        anchors.topMargin: 10
        anchors.margins: 5
        checkedButton: noFilter

        Button {
            id: noFilter
            text: "All"
        }

        Button {
            id: motorcarFilter
            iconSource: "qrc:/icons/routing-motorcar.svg";
            width: 120
        }

        Button {
            id: bikeFilter
            iconSource: "qrc:/icons/routing-bike.svg";
            width: 80
        }

        Button {
            id: pedestrianFilter
            iconSource: "qrc:/icons/routing-pedestrian.svg";
            width: 60
        }

        onCheckedButtonChanged: {
            offlineDataModel.setVehicleTypeFilter(
                        (filterRow.checkedButton === noFilter          ? OfflineDataModel.Any        : OfflineDataModel.None) |
                        (filterRow.checkedButton === motorcarFilter    ? OfflineDataModel.Motorcar   : OfflineDataModel.None) |
                        (filterRow.checkedButton === bikeFilter        ? OfflineDataModel.Bicycle    : OfflineDataModel.None) |
                        (filterRow.checkedButton === pedestrianFilter  ? OfflineDataModel.Pedestrian : OfflineDataModel.None) )
        }
    }


    ListView {
        id: dataView
        model: offlineDataModel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: filterRow.bottom
        anchors.bottom: parent.bottom
        anchors.topMargin: 10
        anchors.margins: 5
        delegate: dataDelegate
        highlight: Rectangle { radius: 5; color: "lightsteelblue" }
        highlightMoveDuration: 200
        spacing: 10
        section.property: "continent"
        section.delegate: sectionDelegate
        currentIndex: -1
        clip: true
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
                        property string displaySize: delegateRoot.showDetails && !settings.workOffline ? " (" + formatSize() + ")" : ""
                        text: "<font size=\"-1\">Version available: " + releasedate + displaySize + "</font>" + (installed ? "<br />" + localVersion : "")

                        function formatSize() {
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
                        enabled: !installed || upgradable
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
