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
    property int previousActivity: -1
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
                if( background.activity != -1 ) {
                    background.previousActivity = background.activity
                }
                background.activity = activityView.currentIndex = activityView.indexAt( x, y )
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
    ListModel {
        id: activityModel
        ListElement {
            name: "Virtual Globe"
            imagePath: "image://theme/icon-m-common-checkbox-checked"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Drive"
            imagePath: "image://theme/icon-m-common-checkbox-checked"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Cycle"
            imagePath: "image://theme/icon-m-common-checkbox-checked"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Walk"
            imagePath: "image://theme/icon-m-common-checkbox-checked"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Guidance"
            imagePath: "image://theme/icon-m-common-checkbox-checked"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Search"
            imagePath: "/usr/share/icons/oxygen/128x128/actions/edit-find.png"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Bookmarks"
            imagePath: "/usr/share/icons/oxygen/128x128/actions/bookmarks-organize.png"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Around Me"
            imagePath: "image://theme/icon-m-common-checkbox-checked"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Weather"
            imagePath: "/usr/share/icons/oxygen/128x128/status/weather-showers-day.png"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Tracking"
            imagePath: "image://theme/icon-m-common-checkbox-checked"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Geocaching"
            imagePath: "image://theme/icon-m-common-checkbox-checked"
            property variant enablePlugins: [ "opencaching" ]
            property variant disablePlugins: [ "weather" ]
            property variant relatedActivities: {
                "Guidance": [ "geocaching" ]
            }
        }
        ListElement {
            name: "Friends"
            imagePath: "image://theme/icon-m-common-checkbox-checked"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Download"
            imagePath: "/usr/share/icons/oxygen/128x128/places/folder-downloads.png"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
        ListElement {
            name: "Configuration"
            imagePath: "/usr/share/icons/oxygen/128x128/actions/configure.png"
            property variant enablePlugins: [  ]
            property variant disablePlugins: [  ]
            property variant relatedActivities: {  }
        }
    }

    function getModel() {
        return activityModel
    }
}