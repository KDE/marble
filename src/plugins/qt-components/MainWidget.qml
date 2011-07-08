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

        // This starts/stops gps tracking
        active: false

        // A small grow/shrink animation of the ghost to indicate position updates
        onPositionChanged: {
            growAnimation.running = true
            if ( map.autoCenter ) {
                map.center = gpsd.position
            }
        }
    }

    // The map widget
    MarbleWidget {
        id: map
        anchors.fill: parent

        property bool autoCenter: false

        mapThemeId: "earth/openstreetmap/openstreetmap.dgml"
        activeFloatItems: [ "compass", "scalebar", "progress" ]

        // The grouped property tracking provides access to tracking related
        // properties
        tracking {
            // We connect the position source from above with the map
            positionSource: QtMobilityPositionProviderPlugin

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
    }
}
