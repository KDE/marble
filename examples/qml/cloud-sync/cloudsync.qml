// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import org.kde.edu.marble 0.11

Rectangle {
    id: screen
    width: 640; height: 480

    MarbleWidget {
        id: map
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: routeView.right

        property bool autoCenter: false

        mapThemeId: "earth/openstreetmap/openstreetmap.dgml"
        activeFloatItems: [ "compass", "scalebar", "progress" ]
    }

    CloudSync {
        id: cloudSync
        map: map

        owncloudServer: ""
        owncloudUsername: ""
        owncloudPassword: ""
    }

    ListView {
        id: routeView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: 350

        model: cloudSync.routeModel
        delegate: routeViewDelegate
    }

    Component {
        id: routeViewDelegate
        
        Rectangle {
            width: routeView.width
            height: Math.max( previewImage.height, nameText.height+buttonRow.height )
            
            Image {
                id: previewImage
                source: previewUrl
                width: 64; height: 64
                anchors.left: parent.left
            }
            
            Text {
                id: nameText
                text: name
                anchors.left: previewImage.right
                anchors.right: parent.right
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Row {
                id: buttonRow
                anchors.top: nameText.bottom
                anchors.left: nameText.left

                Button {
                    id: downloadArea
                    visible: !isCached && isOnCloud

                    label: "Download"
                    color: "green"

                    onClicked: {
                        cloudSync.downloadRoute( identifier )
                    }
                }

                Button {
                    id: deleteFromCloudArea
                    visible: !isCached

                    label: "Delete from cloud"
                    color: "red"

                    onClicked: {
                        cloudSync.deleteRouteFromCloud( identifier )
                    }
                }

                Button {
                    id: openArea
                    visible: isCached

                    label: "Open"
                    color: "blue"

                    onClicked: {
                        cloudSync.openRoute( identifier )
                    }
                }

                Button {
                    id: removeFromCacheArea
                    visible: isCached

                    label: "Remove from device"
                    color: "yellow"

                    onClicked: {
                        cloudSync.removeRouteFromDevice( identifier )
                    }
                }

                Button {
                    id: uploadArea
                    visible: isCached && !isOnCloud

                    label: "Upload"
                    color: "grey"

                    onClicked: {
                        cloudSync.uploadRoute( identifier )
                    }
                }
            }
        }
    }
}
