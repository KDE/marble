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

Item {
    property MarbleWidget map
    property Tracking tracking

    Image {
        id: positionFinderDirection
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        z: 10
        visible: tracking !== null
        smooth: true

        source: "qrc:/icons/marker-direction.svg"
        //rotation: 180 + tracking.lastKnownPosition.bearing( map.center.longitude, map.center.latitude )
    }

    Image {
        id: positionFinder
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        z: 10
        visible: tracking !== null

        source: ( tracking !== null && tracking.positionSource.hasPosition ) ? "qrc:/icons/marker.svg" : "qrc:/icons/marker-yellow.svg"

        MouseArea {
            anchors.fill: parent
            onClicked: centerOn( tracking.lastKnownPosition.longitude, tracking.lastKnownPosition.latitude )
        }
    }

    Rectangle {
        anchors.bottom: positionFinder.top
        width: positionDistanceText.width + 6
        height: positionDistanceText.height + 4
        anchors.right: parent.right
        anchors.margins: 4
        visible: tracking !== null
        radius: 5
        color: Qt.rgba(192/255, 192/255, 192/255, 192/255)

        Text {
            id: positionDistanceText
            anchors.centerIn: parent
        }
    }

    function updatePositionIndicator() {
        if (visible && map !== null && tracking !== null) {
            var pos = map.pixel( tracking.lastKnownPosition.longitude, tracking.lastKnownPosition.latitude )
            positionFinderDirection.rotation = 270 + 180.0 / Math.PI * Math.atan2 ( positionFinderDirection.y - pos.y, positionFinderDirection.x - pos.x )
            var indicatorPosition = map.coordinate( positionFinderDirection.x, positionFinderDirection.y )
            positionDistanceText.text = (tracking.lastKnownPosition.distance( indicatorPosition.longitude, indicatorPosition.latitude ) / 1000).toFixed(1) + " km"
        }
    }

    onMapChanged: {
        map.visibleLatLonAltBoxChanged.connect( updatePositionIndicator )
        updatePositionIndicator()
    }

    onTrackingChanged: {
        tracking.lastKnownPositionChanged.connect( updatePositionIndicator )
        updatePositionIndicator()
    }

    onVisibleChanged: updatePositionIndicator()
}
