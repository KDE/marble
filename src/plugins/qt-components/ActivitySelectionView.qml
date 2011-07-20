// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.1
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

Rectangle {
    id: background
    color: "white"
    property int activity: -1
    GridView {
        id: activityView
        currentIndex: -1
        anchors.fill: parent
        cellWidth: 160
        cellHeight: 150
        model: activityModel
        delegate: delegate
        focus: true
        clip: true
        MouseArea {
            anchors.fill: parent
            onClicked: {
                var x = mouseX + activityView.contentX
                var y = mouseY + activityView.contentY
                background.activity = activityView.currentIndex = activityView.indexAt( x, y )
            }
        }

        ListModel {
            id: activityModel
            ListElement {
                name: "Virtual Globe"
                imagePath: "image://theme/icon-m-common-checkbox-checked"
            }
            ListElement {
                name: "Drive"
                imagePath: "image://theme/icon-m-common-checkbox-checked"
            }
            ListElement {
                name: "Cycle"
                imagePath: "image://theme/icon-m-common-checkbox-checked"
            }
            ListElement {
                name: "Walk"
                imagePath: "image://theme/icon-m-common-checkbox-checked"
            }
            ListElement {
                name: "Guidance"
                imagePath: "image://theme/icon-m-common-checkbox-checked"
            }
            ListElement {
                name: "Search"
                imagePath: "/usr/share/icons/oxygen/128x128/actions/edit-find.png"
            }
            ListElement {
                name: "Bookmarks"
                imagePath: "/usr/share/icons/oxygen/128x128/actions/bookmarks-organize.png"
            }
            ListElement {
                name: "Around Me"
                imagePath: "image://theme/icon-m-common-checkbox-checked"
            }
            ListElement {
                name: "Weather"
                imagePath: "/usr/share/icons/oxygen/128x128/status/weather-showers-day.png"
            }
            ListElement {
                name: "Tracking"
                imagePath: "image://theme/icon-m-common-checkbox-checked"
            }
            ListElement {
                name: "Geocaching"
                imagePath: "image://theme/icon-m-common-checkbox-checked"
            }
            ListElement {
                name: "Friends"
                imagePath: "image://theme/icon-m-common-checkbox-checked"
            }
            ListElement {
                name: "Download"
                imagePath: "/usr/share/icons/oxygen/128x128/places/folder-downloads.png"
            }
            ListElement {
                name: "Configuration"
                imagePath: "/usr/share/icons/oxygen/128x128/actions/configure.png"
            }
        }

        Component {
            id: delegate
            Item {
                width: 128 + 10
                height: 128 + 25
                Column {
                    x: 5
                    y: 10
                    Image {
                        id: activityImage
                        width: 128
                        height: 128
                        source: imagePath
                    }
                    Text {
                        width: parent.width
                        color: "black"
                        text: name
                        font.pointSize: 12
                        font.bold: true
                        horizontalAlignment: "AlignHCenter"
                    }
                }
            }
        }
    }
}