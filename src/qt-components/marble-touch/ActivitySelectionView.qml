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
Item {
    id: activityPage

    property alias model: activityView.model
    property bool shown: false

    signal itemSelected

    Loader {
        id: lazyLoader
    }

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
    ListView {
        id: activityView
        currentIndex: -1
        //anchors.top: parent.top
        //anchors.topMargin: 4
        //anchors.left: parent.left
        //anchors.right: parent.right
        //anchors.bottom: changelog.visible ? changelog.top : parent.bottom
        //anchors.margins: 9
        //anchors.leftMargin: 2
        model: activityModel
        focus: true
        clip: true
        spacing: 1
        width: parent.width
        height: parent.height

        delegate:
            Item {

            id: delegateItem
            property bool mouseOver: mouseTracker.containsMouse

            width: activityView.width
            height: 60

            Rectangle {
                color: "white"
                radius: 15
                anchors.fill: parent

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 5
                    width: 140
                    height: parent.height
                    smooth: true

                    Item {
                        width: 2
                        height: parent.height
                    }

                    Image {
                        id: activityImage
                        anchors.verticalCenter: parent.verticalCenter
                        height: 48
                        width: height
                        source: imagePath
                        smooth: true
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        color: delegateItem.mouseOver ? "#111111" : "black"
                        text: name
                        width: 180
                        font.bold: true

                        MouseArea {
                            id: mouseTracker
                            anchors.fill: parent
                            hoverEnabled: true
                        }
                    }

                    Loader {
                        id: previewLoader
                        anchors.verticalCenter: parent.verticalCenter
                        height: 48
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
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    activityPage.itemSelected()
                    activityPage.openActivity( name, path )
                }
            }
        }
    }

    ScrollDecorator {
        flickableItem: activityView
    }

    Label {
        id: changelog
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        font.pixelSize: 16
        visible: settings.changelogShown !== project.version
        color: "white"
        text: "New in version " + project.changelog.get(0).version + ": " + project.changelog.get(0).summary
        MarbleTouch { id: project }
        MouseArea {
            anchors.fill: parent
            onClicked: pageStack.push( "qrc:/AboutMarblePage.qml" )
        }
    }

    // Model that stores information about activities.
    ListModel {
        id: activityModel
        property string configureIcon: main.icon( "actions/configure", 48 );

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
            previewPath: "qrc:/activities/TrackingPreview.qml"
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
            path: "qrc:/activities/Explore.qml"
            previewPath: ""
        }
        ListElement {
            name: "Info"
            imagePath: "qrc:/icons/activity-empty.png"
            path: "qrc:/AboutMarblePage.qml"
            previewPath: ""
        }
        ListElement {
            name: "Preferences"
            imagePath: "qrc:/icons/activity-empty.png"
            path: "qrc:/PreferencesPage.qml"
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
        pageStack.replace( path, undefined, true )
        activityPage.shown = false
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
