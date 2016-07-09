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

    CircularButton {
        id: muteButton

        property bool muted: false

        anchors.right: infoBar.right
        anchors.rightMargin: Screen.pixelDensity * 3
        anchors.top: infoBar.bottom
        anchors.topMargin: Screen.pixelDensity * 3
        iconSource: muted ? "qrc:///material/volume-off.svg" : "qrc:///material/volume-on.svg"
        onClicked: muted = !muted
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

        onVoiceNavigationAnnouncementChanged: {
            if (root.guidanceMode && !muteButton.muted) {
                textToSpeechClient.readText(voiceNavigationAnnouncement);
            }
        }
    }
}
