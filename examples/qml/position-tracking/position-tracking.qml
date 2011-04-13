// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>

// A (gps) tracking example. Shows the current (gps) position on the map
// using a small ghost image. The visibility of the track can be toggled.

import Qt 4.7
import org.kde.edu.marble 0.11

Rectangle {
    id: screen
    width: 640; height: 480

    // Delivers the current (gps) position
    PositionSource {
        id: gpsd

        // Can optionally be used to select a specific position provider
        // plugin of marble. Per default the first one is used.
        // The value is the nameId() of an installed Marble PositionProviderPlugin,
        // e.g. Gpsd
        //source: "Gpsd"

        // This starts gps tracking
        active: true

        // A small grow/shrink animation of the ghost to indicate position updates
        onPositionChanged: { growAnimation.running = true }
    }

    // The map widget
    MarbleWidget {
        id: map
        anchors.fill: parent

        mapThemeId: "earth/openstreetmap/openstreetmap.dgml"
        activeFloatItems: [ "compass", "scalebar", "progress" ]

        // The grouped property tracking provides access to tracking related
        // properties
        tracking {
            // We connect the position source from above with the map
            positionSource: gpsd

            // Don't show the default Marble position indicator (arrow)
            showPosition: false

            // Initially we don't show the track
            showTrack: false

            // We have our own position marker, the image of a ghost.
            // Marble will take care of positioning it correctly. It will
            // be hidden when there is no current position or it is not
            // visible on the screen
            positionMarker: marker
        }

        // A control to toggle track visibility
        Rectangle {
            id: toggleTrack
            width: 100; height: 20
            radius: 5
            color: map.tracking.showTrack ? "red" : "gray"
            x: 30; y: 30

            Text {
                anchors.horizontalCenter: toggleTrack.horizontalCenter
                anchors.verticalCenter: toggleTrack.verticalCenter
                anchors.margins: 10
                color: "white"
                text: map.tracking.showTrack ? "Track visible" : "Track hidden"
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    map.tracking.showTrack = !map.tracking.showTrack
                }
            }
        }
    }

    // A small ghost indicates the current position
    Image {
        id: marker
        width: 60
        fillMode: Image.PreserveAspectFit
        smooth: true
        source: "ghost.svg"
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
}
