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
                        source: previewLoader.visible ? "qrc:/icons/activity-empty.png" : imagePath
                        smooth: true

                        Loader {
                            id: previewLoader
                            anchors.centerIn: parent
                            visible: status == Loader.Ready && item.isActive
                            source: previewPath

                            Component.onCompleted: {
                                if ( source !== "" && marbleWidget === null) {
                                    lazyLoader.source = "qrc:/MainWidget.qml";
                                    marbleWidget = lazyLoader.item
                                }
                            }
                        }
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
    ListModel {
        id: activityModel

        ListElement {
            name: "Virtual Globe"
            imagePath: "qrc:/icons/activity-virtualglobe.png"
            path: "qrc:/activities/VirtualGlobe.qml"
            previewPath: ""
        }

        ListElement {
            name: "Search"
            imagePath: "qrc:/icons/activity-search.png"
            path: "qrc:/activities/Search.qml"
            previewPath: ""
        }
        ListElement {
            name: "Routing"
            imagePath: "qrc:/icons/activity-routing.png"
            path: "qrc:/activities/Routing.qml"
            previewPath: ""
        }
        ListElement {
            name: "Tracking"
            imagePath: "qrc:/icons/activity-tracking.png"
            path: "qrc:/activities/Tracking.qml"
            previewPath: ""
        }
        ListElement {
            name: "Navigation"
            imagePath: "qrc:/icons/activity-navigation.png"
            path: "qrc:/activities/Navigation.qml"
            previewPath: "qrc:/activities/NavigationPreview.qml"
        }
        ListElement {
            name: "Weather"
            imagePath: "qrc:/icons/activity-weather.png"
            path: "qrc:/activities/Weather.qml"
            previewPath: "qrc:/activities/WeatherPreview.qml"
        }
        ListElement {
            name: "Community"
            imagePath: "qrc:/icons/activity-friends.png"
            path: "qrc:/activities/Friends.qml"
            previewPath: ""
        }
        ListElement {
            name: "Space View"
            imagePath: "qrc:/icons/activity-spaceview.png"
            path: "qrc:/activities/SpaceView.qml"
            previewPath: ""
        }
        ListElement {
            name: "Explore"
            imagePath: "qrc:/icons/activity-explore.png"
            path: "qrc:/activities/SpaceView.qml"
            previewPath: ""
        }
    }

    function openActivity( name ) {
        for ( var i=0; i<activityModel.count; i++ ) {
            if ( activityModel.get(i).name === name ) {
                switchTo( name, activityModel.get(i).path )
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
}
