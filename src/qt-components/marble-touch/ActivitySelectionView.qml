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

    Rectangle {
        anchors.fill: parent
        color: "black"


        Image {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 20
            source: "qrc:/marble/globe.svg"
            smooth: true
            width: 360
            height: 360
            opacity: 0.1
        }

        // Grid view to display images and names of activities.
        GridView {
            id: activityView
            currentIndex: -1
            anchors.fill: parent
            anchors.margins: 9
            cellWidth: 154
            cellHeight: 174
            model: activityModel
            focus: true
            clip: true

            delegate:
                Item {
                width: 154
                height: 184

                Column {
                    anchors.centerIn: parent
                    spacing: 5
                    width: 140
                    height: 180
                    smooth: true

                    Image {
                        id: activityImage
                        width: 140
                        height: 140
                        source: imagePath
                        smooth: true
                    }
                    Text {
                        width: parent.width
                        color: "white"
                        text: name
                        font.pointSize: 12
                        font.bold: true
                        horizontalAlignment: "AlignHCenter"
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: activityPage.openActivity( name, path )
                }
            }
        }

        Item {
            id: buttonRow
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            height: infoButton.height

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10

                Button {
                    id: infoButton
                    text: "Info";
                    width: buttonRow.width / 2 - 20
                    onClicked: pageStack.push( "qrc:/AboutMarblePage.qml" )
                }
                Button {
                    text: "Preferences";
                    width: buttonRow.width / 2 - 20
                    onClicked: pageStack.push( "qrc:/PreferencesPage.qml" )
                }
            }
        }

        Label {
            anchors.bottom: buttonRow.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 30
            font.pixelSize: 16
            width: parent.width
            visible: settings.changelogShown !== project.version
            color: "white"
            text: "New in version " + project.changelog.get(0).version + ": " + project.changelog.get(0).summary
            MarbleTouch { id: project }
            MouseArea {
                anchors.fill: parent
                onClicked: pageStack.push( "qrc:/AboutMarblePage.qml" )
            }
        }

    }

    // Model that stores information about activities.
    ActivityModel { id: activityModel }

    function openActivity( name ) {
        for ( var i=0; i<activityModel.rowCount(); i++ ) {
            if ( activityModel.get( i, "name" ) === name ) {
                switchTo( name, activityModel.get( i, "path" ) )
                return
            }
        }
    }

    function switchTo( name, path ) {
        if ( marbleWidget === null ) {
            lazyLoader.source = "qrc:/MainWidget.qml";
            marbleWidget = lazyLoader.item
        }

        settings.lastActivity = name
        pageStack.push( path )
    }

    Timer {
        id: loadTimer
        interval: 50; running: false; repeat: false
        onTriggered: activityPage.initialize()
    }

    function initializeDelayed() {
        if ( marbleWidget === null ) {
            loadTimer.running = true
        }
    }

    function initialize() {
        if ( marbleWidget === null ) {
            lazyLoader.source = "qrc:/MainWidget.qml";
            marbleWidget = lazyLoader.item
        }
    }

    Component.onCompleted: {
        // Inserts activities into model, add more activities here.
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
                    "qrc:/icons/activity-routing.png",
                    "qrc:/activities/Routing.qml"
                    )
        activityModel.addActivity(
                    "Tracking",
                    "qrc:/icons/activity-tracking.png",
                    "qrc:/activities/Tracking.qml"
                    )
        activityModel.addActivity(
                    "Navigation",
                    "qrc:/icons/activity-navigation.png",
                    "qrc:/activities/Navigation.qml"
                    )
        activityModel.addActivity(
                    "Weather",
                    "qrc:/icons/activity-weather.png",
                    "qrc:/activities/Weather.qml"
                    )
        activityModel.addActivity(
                    "Community",
                    "qrc:/icons/activity-friends.png",
                    "qrc:/activities/Friends.qml"
                    )
        activityModel.addActivity(
                    "Space View",
                    "qrc:/icons/activity-spaceview.png",
                    "qrc:/activities/SpaceView.qml"
                    )
        activityModel.addActivity(
                    "Explore",
                    "qrc:/icons/activity-spaceview.png",
                    "qrc:/activities/SpaceView.qml"
                    )

        // @todo: Terms of usage still not clear
        //            activityModel.addActivity(
        //                        "Geocaching",
        //                        "qrc:/icons/activity-default.png",
        //                        marbleWidget,
        //                        "qrc:/activities/Geocaching.qml"
        //                        )
        /** @todo: Implement missing stuff and re-enable the activities below */
        /*
            activityModel.addActivity(
                "Drive",
                "qrc:/icons/activity-default.png",
                "qrc:/activities/Drive.qml",
            )
            activityModel.addActivity(
                "Cycle",
                "qrc:/icons/activity-default.png",
                "qrc:/activities/Cycle.qml",
            )
            activityModel.addActivity(
                "Walk",
                "qrc:/icons/activity-default.png",
                "qrc:/activities/Walk.qml",
            )
            activityModel.addActivity(
                "Guidance",
                "qrc:/icons/activity-default.png",
                "qrc:/activities/Guidance.qml",
            )
            activityModel.addActivity(
                "Bookmarks",
                "qrc:/icons/activity-bookmarks.png",
                "qrc:/activities/Bookmarks.qml",
            )
            activityModel.addActivity(
                "Around Me",
                "qrc:/icons/activity-default.png",
                "qrc:/activities/AroundMe.qml",
            )
            activityModel.addActivity(
                "Download",
                "qrc:/icons/activity-download.png",
                "qrc:/activities/Download.qml",
            )
            activityModel.addActivity(
                "Configuration",
                "qrc:/icons/activity-configure.png",
                "qrc:/activities/Configuration.qml",
            )
*/
    }
}
