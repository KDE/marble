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

/*
 * Settings of the application.
 */
Item {
    id: root

    // Instance of the C++ class that manages the settings.
    Settings {
        id: settings
    }

    // Load settings from file.
    property string mapTheme: settings.value( "MarbleWidget", "mapTheme", "earth/openstreetmap/openstreetmap.dgml" )
    property string streetMapTheme: settings.value( "MarbleTouch", "streetMapTheme", "earth/openstreetmap/openstreetmap.dgml" )
    property bool workOffline: settings.value( "MainWindow", "workOffline", false )
    property real quitLongitude: settings.value( "MarbleWidget", "quitLongitude", 0.0 )
    property real quitLatitude: settings.value( "MarbleWidget", "quitLatitude", 0.0 )
    property real lastKnownLongitude: settings.value( "MarbleWidget", "lastKnownLongitude", 0.0 )
    property real lastKnownLatitude: settings.value( "MarbleWidget", "lastKnownLatitude", 0.0 )
    property real quitRadius: settings.value( "MarbleWidget", "quitRadius", 148.0 )
    property bool gpsTracking: settings.value( "MarbleWidget", "gpsTracking", false )
    property bool showPosition: settings.value( "MarbleWidget", "showPosition", false )
    property bool showTrack: settings.value( "MarbleWidget", "showTrack", false )
    property string projection: settings.value( "MarbleWidget", "projection", "Spherical" )
    property variant defaultRenderPlugins: ["compass", "coordinate-grid", "progress",
        "crosshairs", "stars", "scalebar"]
    property variant activeRenderPlugins: settings.value( "MarbleWidget", "activeRenderPlugins", defaultRenderPlugins )
    property string lastActivity: settings.value( "MarbleTouch", "lastActivity", "" )
    property bool inhibitScreensaver: settings.value( "MarbleTouch", "inhibitScreensaver", true )
    property bool voiceNavigationMuted: settings.value( "MarbleTouch", "voiceNavigationMuted", false)
    property bool voiceNavigationSoundEnabled: settings.value( "MarbleTouch", "voiceNavigationSoundEnabled", false)
    property string voiceNavigationSpeaker: settings.value( "MarbleTouch", "voiceNavigationSpeaker", "English - Bugsbane")

    // Save settings to file.
    Component.onDestruction: {
        settings.setValue( "MarbleWidget", "mapTheme", root.mapTheme )
        settings.setValue( "MainWindow", "workOffline", root.workOffline )
        settings.setValue( "MarbleWidget", "quitLongitude", root.quitLongitude )
        settings.setValue( "MarbleWidget", "quitLatitude", root.quitLatitude )
        settings.setValue( "MarbleWidget", "lastKnownLongitude", root.lastKnownLongitude )
        settings.setValue( "MarbleWidget", "lastKnownLatitude", root.lastKnownLatitude )
        settings.setValue( "MarbleWidget", "quitRadius", root.quitRadius )
        settings.setValue( "MarbleWidget", "gpsTracking", root.gpsTracking )
        settings.setValue( "MarbleWidget", "showPosition", root.showPosition )
        settings.setValue( "MarbleWidget", "showTrack", root.showTrack )
        settings.setValue( "MarbleWidget", "projection", root.projection )
        settings.setValue( "MarbleWidget", "activeRenderPlugins", root.activeRenderPlugins )
        settings.setValue( "MarbleTouch", "lastActivity", root.lastActivity )
        settings.setValue( "MarbleTouch", "streetMapTheme", root.streetMapTheme )
        settings.setValue( "MarbleTouch", "inhibitScreensaver", root.inhibitScreensaver )
        settings.setValue( "MarbleTouch", "voiceNavigationMuted", root.voiceNavigationMuted )
        settings.setValue( "MarbleTouch", "voiceNavigationSoundEnabled", root.voiceNavigationSoundEnabled )
        settings.setValue( "MarbleTouch", "voiceNavigationSpeaker", root.voiceNavigationSpeaker )
    }

    function removeElementsFromArray(array, elements) {
        for( var j=0; j<elements.length; j++) {
            for( var i=0; i<array.length; i++) {
                if (array[i] === elements[j]) {
                    array.splice(i,1)
                }
            }
        }
    }
}
