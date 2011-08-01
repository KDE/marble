// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

Rectangle {
    id: background
    color: "white"
    property int activity: -1
    property int previousActivity: -1
    property alias model: activityView.model

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

    ActivityModel {
        id: activityModel
        Component.onCompleted: {
            console.log( "adding activities" )
            activityModel.addActivity(
                "Virtual Globe",
                "qrc:/icons/activity-virtualglobe.png",
                [],
                [],
                {}
            )
            activityModel.addActivity(
                "Drive",
                "qrc:/icons/activity-default.png",
                [],
                [],
                {}
            )
            activityModel.addActivity(
                "Cycle",
                "qrc:/icons/activity-default.png",
                [],
                [],
                {}
            )
            activityModel.addActivity(
                "Walk",
                "qrc:/icons/activity-default.png",
                [],
                [],
                {}
            )
            activityModel.addActivity(
                "Guidance",
                "qrc:/icons/activity-default.png",
                [],
                [ "opencaching" ],
                {}
            )
            activityModel.addActivity(
                "Search",
                "qrc:/icons/activity-search.png",
                [],
                [],
                {}
            )
            activityModel.addActivity(
                "Bookmarks",
                "qrc:/icons/activity-bookmarks.png",
                [],
                [],
                {}
            )
            activityModel.addActivity(
                "Around Me",
                "qrc:/icons/activity-default.png",
                [],
                [],
                {}
            )
            activityModel.addActivity(
                "Weather",
                "qrc:/icons/activity-weather.png",
                [],
                [],
                {}
            )
            activityModel.addActivity(
                "Tracking",
                "qrc:/icons/activity-default.png",
                [],
                [],
                {}
            )
            activityModel.addActivity(
                "Geocaching",
                "qrc:/icons/activity-default.png",
                [ "opencaching" ],
                [ "weather" ],
                { "Guidance": [ "opencaching" ] }
            )
            activityModel.addActivity(
                "Friends",
                "qrc:/icons/activity-friends.png",
                [],
                [],
                {}
            )
            activityModel.addActivity(
                "Download",
                "qrc:/icons/activity-download.png",
                [],
                [],
                {}
            )
            activityModel.addActivity(
                "Configuration",
                "qrc:/icons/activity-configure.png",
                [],
                [],
                {}
            )
            console.log( "finished adding activities" )
        }
    }
    
    function setCurrentActivity( name ) {
        for( var i = 0; i < activityModel.rowCount(); i++ ) {
            if( name == activityModel.get( i, "name" ) ) {
                previousActivity = activity
                activity = i
                return
            }
        }
    }
}