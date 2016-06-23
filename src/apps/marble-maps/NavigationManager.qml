//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//


import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtMultimedia 5.4

import org.kde.edu.marble 0.20

Item {
    id: root
    property var marbleItem: null
    property var tts: null
    property alias snappedPositionMarkerScreenPosition: navigation.screenPosition
    property bool guidanceMode: false
    property alias screenAccuracy: navigation.screenAccuracy
    property alias deviated: navigation.deviated
    property alias speedDistancePanelheight: speedDistancePanel.height

    onGuidanceModeChanged: {
        if (guidanceMode) {
            marbleItem.setZoomToMaximumLevel();
            marbleItem.centerOnCurrentPosition();
        }

        navigation.guidanceModeEnabled = guidanceMode;
    }

    Audio {
        id: audioPlayer
        autoPlay: false
        autoLoad: false
        source: "assets://data/audio/KDE-Sys-List-End.ogg";
    }

    NavigationInfoBar {
        id: infoBar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        instructionIcon: navigation.nextInstructionImage.replace("qrc:/", "qrc:///");
        distance: navigation.nextInstructionDistance;
    }

    Rectangle{
        id: speedDistancePanel
        color: palette.window
        height: totalDistance.height * 2
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        BoxedText {
            id: totalDistance
            color: palette.window
            textColor: palette.text
            text: "%1 km".arg((0.001 * navigation.destinationDistance).toFixed(1))

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 10
            }
        }

        BoxedText {
            id: speed
            color: palette.window
            textColor: palette.text
            text: qsTr("%1 km/h".arg((navigation.marbleQuickItem.speed * 3.6).toFixed(0)))

            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 10
            }
        }
    }

    Navigation {
        id: navigation
        marbleQuickItem: marbleItem
        soundEnabled: false

        onVoiceNavigationAnnouncementChanged: {
            if (!parent.visible) {
                return "";
            }

            var text = getCommand(voiceNavigationAnnouncement);
            switch (text) {
            case "KDE-Sys-List-End":
                audioPlayer.source = "assets:/data/audio/KDE-Sys-List-End.ogg";
                audioPlayer.stop();
                audioPlayer.play();
                break;
            case "KDE-Sys-App-Positive":
                audioPlayer.source = "assets:/data/audio/KDE-Sys-App-Positive.ogg";
                audioPlayer.stop();
                audioPlayer.play();
                break;
            default:
                textToSpeechClient.readText(text);
            }
        }

        function getCommand(command)
        {
            switch (command) {
            case "Straight":
                return qsTr("Go straight on!");
            case "AhKeepRight":
                return qsTr("Ahead, keep to the right!");
            case "AhKeepLeft":
                return qsTr("Ahead, keep to the left!");
            case "AhRightTurn":
                return qsTr("Ahead, turn right!");
            case  "AhLeftTurn":
                return qsTr("Ahead, turn left!");
            case "AhUTurn":
                return qsTr("Ahead, perform a U-turn!");
            case "RbExit1":
                return qsTr("Take the first exit!");
            case "RbExit2":
                return qsTr("Take the second exit!");
            case "RbExit3":
                return qsTr("Take the third exit!");
            case "AhExitLeft":
                return qsTr("Ahead, exit left!");
            case "AhExitRight":
                return qsTr("Ahead, exit right!");
            case "GpsLost":
                return qsTr("Lost GPS connection");
            case "GpsFound":
                return qsTr("GPS position found.");
            case "Arrive":
                return qsTr("You have arrived to your destination.");
            case "RouteDeviated":
                return qsTr("Deviated from the route.");
            case "Marble":
                return qsTr("The Marble team wishes you a pleasant and safe journey!")
            default:
                return command;
            }
        }
    }
}
