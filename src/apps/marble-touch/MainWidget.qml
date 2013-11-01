// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
// Copyright 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

import QtQuick 1.0
import org.kde.edu.marble 0.11
import com.nokia.meego 1.0

/*
 * Main widget containing the map, models for routing, search, etc.
 *
 * @todo: Rename MarbleWidget or similar
 */
Item {
    id: screen
    anchors.fill: parent
    visible: false

    signal mouseClickGeoPosition(real longitude, real latitude)
    signal placemarkSelected(variant placemark)

    property alias mapThemeModel: map.mapThemeModel
    property alias radius: map.radius

    property alias bookmarks: map.bookmarks
    property alias routing: map.routing
    property alias search: map.search
    property alias tracking: map.tracking
    property alias navigation: map.navigation
    property alias cloudSync: map.cloudSync

    // The widget representing the map.
    MarbleWidget {
        id: map
        anchors.fill: parent

        /** @todo: This property looks cumbersome... get rid of it */
        property bool initialized: false

        workOffline: settings.workOffline
        mapThemeId: settings.mapTheme
        radius: settings.quitRadius
        projection: settings.projection
        activeFloatItems: [ "compass", "scalebar", "progress" ]
        activeRenderPlugins: settings.activeRenderPlugins

        property Bookmarks bookmarks: Bookmarks {
            map: map
        }

        property Routing routing: Routing {
            map: map
        }

        // The grouped property "tracking" provides access to tracking related
        // properties.
        property Tracking tracking: Tracking {
            // Connect the position source from below with the map.
            map: map
            positionSource: positionProvider
            showTrack: settings.showTrack
            positionMarker: marker

            onLastKnownPositionChanged: {
                settings.lastKnownLongitude = map.tracking.lastKnownPosition.longitude
                settings.lastKnownLatitude = map.tracking.lastKnownPosition.latitude
                map.updatePositionIndicator()
            }
        }

        property Navigation navigation: Navigation {
            map: map
            muted: settings.voiceNavigationMuted
            speaker: settings.voiceNavigationSpeaker
            soundEnabled: settings.voiceNavigationSoundEnabled
        }

        property CloudSync cloudSync: CloudSync {
            id: cloudSync
            map: map
            owncloudServer: settings.owncloudServer
            owncloudUsername: settings.owncloudUsername
            owncloudPassword: settings.owncloudPassword
        }
        
        Component.onCompleted: {
            mouseClickGeoPosition.connect( screen.mouseClickGeoPosition )

            // Load last center of the map.
            center.longitude = settings.quitLongitude
            center.latitude = settings.quitLatitude
            tracking.lastKnownPosition.longitude = settings.lastKnownLongitude
            tracking.lastKnownPosition.latitude = settings.lastKnownLatitude
            initialized = true
        }
        
        Component.onDestruction: {
            settings.quitRadius = radius
            settings.quitLongitude = center.longitude
            settings.quitLatitude = center.latitude
        }

        property Search search: Search {
            map: map
            // Delegate of a search result.
            /** @todo: Simplify this beast */
            placemarkDelegate:
                Image {
                id: searchDelegate
                source: "qrc:/icons/placemark.svg"
                transformOrigin: Item.Bottom
                width: 32
                fillMode: Image.PreserveAspectFit
                smooth: true

                Text {
                    text: (index+1)
                    width: 32
                    height: 32
                    anchors.top: parent.top
                    anchors.left: parent.left
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "white"
                    font.bold: true
                    font.pixelSize: 20
                }

                Rectangle {
                    id: routingOptions
                    visible: false
                    color: "white"
                    width: 250
                    height: nameLabel.height + routingButtons.height + 30
                    border.width: 1
                    border.color: "gray"
                    radius: 10
                    // Name of the search result.
                    Label {
                        id: nameLabel
                        text: display
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 10
                        font.pixelSize: 18

                        MouseArea {
                            anchors.fill: parent
                            onClicked: routingOptions.visible = false
                        }
                    }
                    // Route button
                    Column {
                        id: routingButtons
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottomMargin: 10
                        spacing: 5
                        Rectangle {
                            color: "white"
                            width: 230
                            height: 35
                            border.width: 1
                            border.color: "gray"
                            radius: 10
                            Label {
                                text: "Route"
                                anchors.centerIn: parent
                                font.pixelSize: 18
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    routingOptions.visible = false
                                    settings.gpsTracking = true
                                    map.routing.clearRoute()
                                    map.routing.setVia( 0, map.tracking.lastKnownPosition.longitude, map.tracking.lastKnownPosition.latitude )
                                    map.routing.setVia( 1, longitude, latitude )
                                    openActivity( "Routing" )
                                }
                            }
                        }
                    }
                }
                // Show search result info if the placemark is clicked.
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        routingOptions.visible = !routingOptions.visible
                    }
                }

                Connections {
                    target: map
                    onMouseClickGeoPosition: routingOptions.visible = false
                }
            }
        }

        onPlacemarkSelected: {
            pageStack.find(function(page) {
                // Open the Placemark activity unless it's already the top most page
                // id is not a QML Item property, hence the uid workaround
                if (page.uid !== "edu.kde.org.marble.placemarkActivityPage") {
                    openPage("qrc:/activities/Placemark.qml");
                }
                return true;
            })
            screen.placemarkSelected(placemark)
        }
    }

    // Delivers the current (gps) position.
    PositionSource {
        id: positionProvider

        map: map

        // Can optionally be used to select a specific position provider
        // plugin of marble. Per default the first one is used.
        // The value is the nameId() of an installed Marble PositionProviderPlugin,
        // e.g. Gpsd
        source: "QtMobilityPositionProviderPlugin"

        // This starts/stops gps tracking.
        active: settings.gpsTracking

        // Start a small grow/shrink animation of the marker to indicate position updates.
        onPositionChanged: {
            growAnimation.running = true
        }
    }
    
    // A marker that indicates the current position.
    Image {
        id: marker
        width: 60
        fillMode: Image.PreserveAspectFit
        smooth: true
        source: positionProvider.hasPosition ? "qrc:/icons/marker.svg" : "qrc:/icons/marker-yellow.svg"
        visible: false

        // Animation that grows/shrinks the marker.
        PropertyAnimation on x { duration: 300; easing.type: Easing.OutBounce }
        PropertyAnimation on y { duration: 300; easing.type: Easing.OutBounce }
        SequentialAnimation {
            id: growAnimation
            PropertyAnimation {
                target: marker
                properties: "scale"
                to: 1.2
                duration: 150
            }
            PropertyAnimation {
                target: marker
                properties: "scale"
                to: 1.0
                duration: 150
            }
        }
    }

    PositionIndicator {
        anchors.fill: parent
        visible: settings.showPositionIndicator

        map: map
        tracking: tracking
    }

    // Starts a search for the passed term.
    function find( term ) {
        map.search.find( term )
    }
    
    // Centers map on passed coordinates.
    function centerOn( lon, lat ) {
        map.center.longitude = lon
        map.center.latitude = lat
    }

    function setGeoSceneProperty( key, value ) {
        map.setGeoSceneProperty( key, value )
    }

    function downloadRoute( offset, from, to ) {
        map.downloadRoute( offset, from, to )
    }

    function downloadArea( from, to ) {
        map.downloadArea( from, to )
    }

    function getCenter() {
        return map.center
    }

    function setDataPluginDelegate( id, delegate ) {
        map.setDataPluginDelegate( id, delegate )
    }

    function renderPlugin( name ) {
        return map.renderPlugin( name )
    }
}
