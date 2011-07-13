// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import Qt 4.7
import org.kde.edu.marble 0.11
import com.nokia.meego 1.0

Rectangle {
    id: screen

    // The map widget
    MarbleWidget {
        id: map
        anchors.fill: parent

        property bool autoCenter: settings.autoCenter
        property bool initialized: false

        mapThemeId: settings.mapTheme
        zoom: settings.quitZoom
        projection: settings.projection
        activeFloatItems: [ "compass", "scalebar", "progress" ]

        // The grouped property tracking provides access to tracking related
        // properties
        tracking {
            // We connect the position source from above with the map
            positionSource: positionProvider
            showPosition: settings.showPosition
            showTrack: settings.showTrack
            // We have our own position marker, the image of a ghost.
            // Marble will take care of positioning it correctly. It will
            // be hidden when there is no current position or it is not
            // visible on the screen
            positionMarker: marker
        }
        
        Component.onCompleted: {
            center.longitude = settings.quitLongitude
            center.latitude = settings.quitLatitude
            routing.clearRoute()
            initialized = true
        }
        
        onZoomChanged: {
            settings.quitZoom = zoom
        }
        
        onVisibleLatLonAltBoxChanged: {
            if( initialized ) {
                settings.quitLongitude = center.longitude
                settings.quitLatitude = center.latitude
            }
        }
        
        search {
            placemarkDelegate: 
                Rectangle {
                    id: searchDelegate
                    color: "red"
                    width: 20
                    height: 20
                    border.width: 3
                    border.color: "black"
                    smooth: true
                    Rectangle {
                        id: routingOptions
                        visible: false
                        color: "white"
                        width: 350
                        height: nameLabel.height + routingButtons.height + 30
                        border.width: 1
                        border.color: "blue"
                        radius: 10
                        Label {
                            id: nameLabel
                            text: name
                            width: 340
                            anchors.top: parent.top
                            anchors.left: parent.left
                            anchors.topMargin: 10
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            platformStyle: LabelStyle { fontPixelSize: 20 }
                        }
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
                                border.color: "blue"
                                radius: 10
                                Label {
                                    text: "Directions from here"
                                    anchors.centerIn: parent
                                    platformStyle: LabelStyle { fontPixelSize: 20 }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        routingOptions.visible = false
                                        map.routing.setVia( 0, longitude, latitude )
                                    }
                                }
                            }
                            Rectangle {
                                color: "white"
                                width: 230
                                height: 35
                                border.width: 1
                                border.color: "blue"
                                radius: 10
                                Label {
                                    text: "Directions to here"
                                    anchors.centerIn: parent
                                    platformStyle: LabelStyle { fontPixelSize: 20 }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        routingOptions.visible = false
                                        map.routing.setVia( 1, longitude, latitude )
                                    }
                                }
                            }
                        }
                        Image {
                            id: closeImage
                            width: 30
                            fillMode: Image.PreserveAspectFit
                            smooth: true
                            source: "image://theme/icon-m-toolbar-close"
                            anchors.bottom: parent.top
                            anchors.left: parent.right
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    routingOptions.visible = false
                                }
                            }
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            routingOptions.visible = !routingOptions.visible
                        }
                    }
                }
        }
        
        function centerOn( lon, lat ) {
            map.center.longitude = lon
            map.center.latitude = lat
        }
        
        function getRouting() {
            return map.routing
        }

    }
    
    // Delivers the current (gps) position
    PositionSource {
        id: positionProvider

        // Can optionally be used to select a specific position provider
        // plugin of marble. Per default the first one is used.
        // The value is the nameId() of an installed Marble PositionProviderPlugin,
        // e.g. Gpsd
        //source: "QtMobilityPositionProviderPlugin"

        // This starts/stops gps tracking
        active: settings.gpsTracking

        // A small grow/shrink animation of the ghost to indicate position updates
        onPositionChanged: {
            growAnimation.running = true
            if ( map.autoCenter ) {
                map.center = positionProvider.position
            }
        }
    }
    
    // A marker indicates the current position
    Image {
        id: marker
        width: 60
        fillMode: Image.PreserveAspectFit
        smooth: true
        source: "qrc:/marker.svg"
        visible: false

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
    
    function find( term ) {
        map.search.find( term )
    }
    
    function routeRequestModel() {
        return map.routing.routeRequestModel()
    }
    
    function waypointModel() {
        return map.routing.waypointModel()
    }
    
    function getRouting() {
        return map.getRouting()
    }

}
