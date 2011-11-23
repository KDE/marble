// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import Qt.labs.folderlistmodel 1.0
import com.nokia.meego 1.0

Item {
    id: root

    // Public API
    property alias filename: filenameField.text
    property string folder: "/" // Why can't I alias this as well?
    property alias nameFilters: directoryModel.nameFilters

    signal accepted()
    signal cancelled()

    TextField {
        id: filenameField
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        focus: true
        text: "New file"
    }

    Row {
        id: folderRow
        height: multiButton.height
        anchors.top: filenameField.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        ToolIcon {
            id: multiButton;
            anchors.verticalCenter: parent.verticalCenter
            iconId: "icon-m-toolbar-up"
            onClicked: directoryModel.folder = directoryModel.parentFolder
        }
        Label {
            id: folderLabel;
            anchors.verticalCenter: parent.verticalCenter
            text: directoryModel.folder
        }
    }

    ListView {
        id: folderView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: folderRow.bottom
        anchors.bottom: buttonRow.top

        clip: true

        FolderListModel {
            id: directoryModel
            folder: root.folder
            showOnlyReadable: true
        }

        Component {
            id: fileDelegate

            Item {
                id: fileItem
                property bool isFolder: directoryModel.isFolder(index)
                width: contentRow.width
                height: contentRow.height
                Row {
                    id: contentRow
                    spacing: 5
                    Image { anchors.verticalCenter: parent.verticalCenter; source: fileItem.isFolder ? "image://theme/icon-m-common-directory" : "image://theme/icon-m-content-document" }
                    Label { anchors.verticalCenter: parent.verticalCenter; text: fileName }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (fileItem.isFolder) {
                            directoryModel.folder = directoryModel.folder + "/" + fileName
                        } else {
                            filenameField.text = fileName
                        }
                    }
                }
            }
        }

        model: directoryModel
        delegate: fileDelegate
    }

    Row {
        id: buttonRow
        spacing: 5
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        Button {
            id: saveButton
            width: parent.width / 2 - 5

            text: "Save"
            onClicked: {
                root.folder = directoryModel.folder
                root.accepted()
            }
        }

        Button {
            id: cancelButton
            width: parent.width / 2 - 5

            text: "Cancel"
            onClicked: root.cancelled()
        }
    }
}
