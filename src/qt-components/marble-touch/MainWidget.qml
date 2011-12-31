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

    property alias mapThemeModel: map.mapThemeModel
    property alias streetMapThemeModel: map.streetMapThemeModel
    property alias zoom: map.zoom

    // The widget representing the map.
    MarbleWidget {
        id: map
        anchors.fill: parent

        /** @todo: This property looks cumbersome... get rid of it */
        property bool initialized: false

        workOffline: settings.workOffline
        mapThemeId: settings.mapTheme
        zoom: settings.quitZoom
        projection: settings.projection
        activeFloatItems: [ "compass", "scalebar", "progress" ]
        activeRenderPlugins: settings.activeRenderPlugins

        // The grouped property "tracking" provides access to tracking related
        // properties.
        tracking {
            // Connect the position source from below with the map.
            positionSource: positionProvider
            showPosition: settings.showPosition
            showTrack: settings.showTrack
            positionMarker: marker

            onLastKnownPositionChanged: {
                settings.lastKnownLongitude = map.tracking.lastKnownPosition.longitude
                settings.lastKnownLatitude = map.tracking.lastKnownPosition.latitude
                map.updatePositionIndicator()
            }
        }
        
        Component.onCompleted: {
            mouseClickGeoPosition.connect( screen.mouseClickGeoPosition )

            // Load last center of the map.
            center.longitude = settings.quitLongitude
            center.latitude = settings.quitLatitude
            tracking.lastKnownPosition.longitude = settings.lastKnownLongitude
            tracking.lastKnownPosition.latitude = settings.lastKnownLatitude
            routing.clearRoute()
            initialized = true
        }
        
        Component.onDestruction: {
            settings.quitZoom = zoom
            settings.quitLongitude = center.longitude
            settings.quitLatitude = center.latitude
        }

        onVisibleLatLonAltBoxChanged: updatePositionIndicator()

        search {
            // Delegate of a search result.
            /** @todo: Simplify this beast */
            placemarkDelegate:
                Image {
                id: searchDelegate
                source: "qrc:/placemark.svg"
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
                        platformStyle: LabelStyle { fontPixelSize: 18 }

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
                                platformStyle: LabelStyle { fontPixelSize: 18 }
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
            }
        }

        function updatePositionIndicator() {
            if (positionFinderDirection.visible) {
                var pos = map.pixel( tracking.lastKnownPosition.longitude, tracking.lastKnownPosition.latitude )
                positionFinderDirection.rotation = 270 + 180.0 / Math.PI * Math.atan2 ( positionFinderDirection.y - pos.y, positionFinderDirection.x - pos.x )
                var indicatorPosition = map.coordinate( positionFinderDirection.x, positionFinderDirection.y )
                positionDistance.text = Math.round( tracking.lastKnownPosition.distance( indicatorPosition.longitude, indicatorPosition.latitude ) / 100 ) / 10 + " km"
            }
        }
    }
    
    // Delivers the current (gps) position.
    PositionSource {
        id: positionProvider

        // Can optionally be used to select a specific position provider
        // plugin of marble. Per default the first one is used.
        // The value is the nameId() of an installed Marble PositionProviderPlugin,
        // e.g. Gpsd
        //source: "QtMobilityPositionProviderPlugin"

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
        source: positionProvider.hasPosition ? "qrc:/marker.svg" : "qrc:/marker-yellow.svg"
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

    Image {
        id: positionFinderDirection
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        z: 10
        visible: settings.showPosition
        smooth: true

        source: "qrc:/marker-direction.svg"
        //rotation: 180 + map.tracking.lastKnownPosition.bearing( map.center.longitude, map.center.latitude )
    }

    Image {
        id: positionFinder
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        z: 10
        visible: settings.showPosition

        source: map.tracking.positionSource.hasPosition ? "qrc:/marker.svg" : "qrc:/marker-yellow.svg"

        MouseArea {
            anchors.fill: parent
            onClicked: centerOn( map.tracking.lastKnownPosition.longitude, map.tracking.lastKnownPosition.latitude )
        }
    }

    Text {
        id: positionDistance
        anchors.bottom: positionFinder.top
        anchors.horizontalCenter: positionFinder.horizontalCenter
        anchors.margins: 4
        visible: settings.showPosition
    }
    
    // Starts a search for the passed term.
    function find( term ) {
        map.search.find( term )
    }
    
    // Returns the model that contains routing instructions.
    /** @todo: Needed? Make a property? */
    function routeRequestModel() {
        return map.routing.routeRequestModel()
    }
    
    // Returns the model that contains points on the route.
    /** @todo: Needed? Make a property? */
    function waypointModel() {
        return map.routing.waypointModel()
    }
    
    // Returns the grouped property to access routing functions.
    /** @todo: Needed? Make a property? */
    function getRouting() {
        return map.routing
    }
    
    // Returns the grouped property to access search functions.
    /** @todo: Needed? Make a property? */
    function getSearch() {
        return map.search
    }

    // Returns the grouped property to access tracking functions.
    /** @todo: Needed? Make a property? */
    function getTracking() {
        return map.tracking
    }

    // Centers map on passed coordinates.
    function centerOn( lon, lat ) {
        map.center.longitude = lon
        map.center.latitude = lat
    }

    function setGeoSceneProperty( key, value ) {
        map.setGeoSceneProperty( key, value )
    }

    function getCenter() {
        return map.center
    }
}
