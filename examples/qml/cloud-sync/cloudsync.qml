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

    // The map widget
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
        width: 300

        model: cloudSync.routeModel
        delegate: routeViewDelegate
    }

    Component {
        id: routeViewDelegate
        
        Rectangle {
            width: routeView.width
            height: Math.max( previewImage.height, nameText.height )
            
            Image {
                id: previewImage
                source: previewUrl
                width: 64; height: 64
                anchors.left: parent.left
            }
            
            Text {
                id: nameText
                text: name
                width: routeView.width
                anchors.left: previewImage.right
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }
            
            Button {
                id: downloadArea
                visible: !isCached && isOnCloud
                width: downloadText.width
                height: downloadText.height
                anchors.top: nameText.bottom
                anchors.left: nameText.left
                
                Rectangle {
                    width: downloadText.width
                    height: downloadText.height
                    color: "green"
                    
                    Text {
                        id: downloadText
                        text: "Download"
                        color: "white"
                    }
                }
                
                onClicked: {
                    cloudSync.downloadRoute( identifier )
                }
            }
            
            Button {
                id: deleteFromCloudArea
                visible: !isCached
                width: deleteFromCloudText.width
                height: deleteFromCloudText.height
                anchors.top: downloadArea.top
                anchors.left: downloadArea.right
                
                Rectangle {
                    width: deleteFromCloudText.width
                    height: deleteFromCloudText.height
                    color: "red"
                    
                    Text {
                        id: deleteFromCloudText
                        text: "Delete from cloud"
                        color: "white"
                    }
                }
                
                onClicked: {
                    cloudSync.deleteRouteFromCloud( identifier )
                }
            }
            
            Button {
                id: openArea
                visible: isCached
                width: openText.width
                height: openText.height
                anchors.top: downloadArea.top
                anchors.left: downloadArea.left
                
                Rectangle {
                    width: openText.width
                    height: openText.height
                    color: "blue"
                    
                    Text {
                        id: openText
                        text: "Open"
                        color: "white"
                    }
                }
                
                onClicked: {
                    cloudSync.openRoute( identifier )
                }
            }
            
            Button {
                id: removeFromCacheArea
                visible: isCached
                width: removeFromCacheText.width
                height: removeFromCacheText.height
                anchors.top: openArea.top
                anchors.left: openArea.right
                
                Rectangle {
                    width: removeFromCacheText.width
                    height: removeFromCacheText.height
                    color: "yellow"
                    
                    Text {
                        id: removeFromCacheText
                        text: "Remove from device"
                        color: "black"
                    }
                }
                
                onClicked: {
                    cloudSync.removeRouteFromDevice( identifier )
                }
            }
            
            Button {
                id: uploadArea
                visible: isCached && !isOnCloud
                width: uploadText.width
                height: uploadText.height
                anchors.top: removeFromCacheArea.top
                anchors.left: removeFromCacheArea.right
                
                Rectangle {
                    width: uploadText.width
                    height: uploadText.height
                    color: "grey"
                    
                    Text {
                        id: uploadText
                        text: "Upload"
                        color: "black"
                    }
                }
                
                onClicked: {
                    cloudSync.uploadRoute( identifier )
                }
            }
        }
    }
}
