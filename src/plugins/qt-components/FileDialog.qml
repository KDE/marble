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

Sheet {
    id: root

    // Public API
    property alias filename: filenameField.text
    property string folder: "/" // Why can't I alias this as well?
    property alias nameFilters: directoryModel.nameFilters
    property alias filenameEditable: filenameField.visible

    acceptButtonText: "Save"
    rejectButtonText: "Cancel"

    content: Item {
        anchors.fill: parent

        TextField {
            id: filenameField
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            text: "New file"
        }

        Row {
            id: folderRow
            height: multiButton.height
            anchors.top: filenameField.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            MarbleToolIcon {
                id: multiButton;
                anchors.verticalCenter: parent.verticalCenter
                iconSource: main.icon( "actions/go-up", 48 );
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
            anchors.bottom: parent.bottom

            clip: true

            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            highlightMoveDuration: 200

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
                    width: parent.width
                    height: contentRow.height
                    Row {
                        id: contentRow
                        spacing: 5
                        Image { anchors.verticalCenter: parent.verticalCenter; source: main.icon( fileItem.isFolder ? "places/folder" : "mimetypes/unknown", 48 ) }
                        Label { anchors.verticalCenter: parent.verticalCenter; text: fileName }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            folderView.currentIndex = index
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
    }

    onAccepted: root.folder = directoryModel.folder
}
