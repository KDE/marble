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

/*
 * Page to select activity. This component also contains the model for
 * the activities, which stores all relevant information.
 */
Page {
    id: activityPage

    property alias model: activityView.model

    Loader {
        id: lazyLoader
    }

    tools: ToolBarLayout {
        Item{}
        ToolButton {
            text: "Info";
            onClicked: pageStack.push( "qrc:/AboutMarblePage.qml" )
        }
    }

    // Grid view to display images and names of activities.
    GridView {
        id: activityView
        currentIndex: -1
        anchors.fill: parent
        cellWidth: 160
        cellHeight: 150
        model: activityModel
        focus: true
        clip: true

        delegate: Item {
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

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if ( marbleWidget === null ) {
                        lazyLoader.source = "qrc:/MainWidget.qml";
                        marbleWidget = lazyLoader.item
                    }

                    pageStack.push( path )
                }
            }
        }
    }

    // Model that stores information about activities.
    ActivityModel {
        id: activityModel
        // Inserts activities into model, add more activities here.
        Component.onCompleted: {
            activityModel.addActivity(
                        "Virtual Globe",
                        "qrc:/icons/activity-virtualglobe.png",
                        "qrc:/activities/VirtualGlobe.qml"
                        )
            activityModel.addActivity(
                        "Search",
                        "qrc:/icons/activity-search.png",
                        "qrc:/activities/Search.qml"
                        )
            activityModel.addActivity(
                        "Routing",
                        "qrc:/icons/activity-default.png",
                        "qrc:/activities/Routing.qml"
                        )
            activityModel.addActivity(
                        "Tracking",
                        "qrc:/icons/activity-default.png",
                        "qrc:/activities/Tracking.qml"
                        )
            activityModel.addActivity(
                        "Weather",
                        "qrc:/icons/activity-weather.png",
                        "qrc:/activities/Weather.qml"
                        )
            // @todo: Terms of usage still not clear
            //            activityModel.addActivity(
            //                        "Geocaching",
            //                        "qrc:/icons/activity-default.png",
            //                        marbleWidget,
            //                        "qrc:/activities/Geocaching.qml"
            //                        )
            activityModel.addActivity(
                        "Friends",
                        "qrc:/icons/activity-friends.png",
                        "qrc:/activities/Friends.qml"
                        )
            activityModel.addActivity(
                        "Space View",
                        "qrc:/icons/activity-spaceview.png",
                        "qrc:/activities/SpaceView.qml"
                        )

            /** @todo: Implement missing stuff and re-enable the activities below */
            /*
            activityModel.addActivity(
                "Drive",
                "qrc:/icons/activity-default.png",
                "qrc:/activities/Drive.qml",
                [],
                [],
                {},
                { "mapTheme": "earth/openstreetmap/openstreetmap.dgml" }
            )
            activityModel.addActivity(
                "Cycle",
                "qrc:/icons/activity-default.png",
                "qrc:/activities/Cycle.qml",
                [],
                [],
                {},
                { "mapTheme": "earth/openstreetmap/openstreetmap.dgml" }
            )
            activityModel.addActivity(
                "Walk",
                "qrc:/icons/activity-default.png",
                "qrc:/activities/Walk.qml",
                [],
                [],
                {},
                { "mapTheme": "earth/openstreetmap/openstreetmap.dgml" }
            )
            activityModel.addActivity(
                "Guidance",
                "qrc:/icons/activity-default.png",
                "qrc:/activities/Guidance.qml",
                [],
                [ "opencaching" ],
                {},
                { "projection": "Mercator",
                  "mapTheme": "earth/openstreetmap/openstreetmap.dgml" }
            )
            activityModel.addActivity(
                "Bookmarks",
                "qrc:/icons/activity-bookmarks.png",
                "qrc:/activities/Bookmarks.qml",
                [],
                [],
                {},
                {}
            )
            activityModel.addActivity(
                "Around Me",
                "qrc:/icons/activity-default.png",
                "qrc:/activities/AroundMe.qml",
                [],
                [],
                {},
                { "projection": "Mercator",
                  "mapTheme": "earth/openstreetmap/openstreetmap.dgml" }
            )
            activityModel.addActivity(
                "Download",
                "qrc:/icons/activity-download.png",
                "qrc:/activities/Download.qml",
                [],
                [],
                {},
                {}
            )
            activityModel.addActivity(
                "Configuration",
                "qrc:/icons/activity-configure.png",
                "qrc:/activities/Configuration.qml",
                [],
                [],
                {},
                {}
            )
*/
        }
    }

}
