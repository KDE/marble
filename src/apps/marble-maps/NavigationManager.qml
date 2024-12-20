// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//


import QtQuick
import QtQuick.Controls
import QtQuick.Window

import org.kde.marble

Item {
    id: root
    property var marbleItem: null
    property var tts: null
    property alias snappedPositionMarkerScreenPosition: navigation.screenPosition
    property alias screenAccuracy: navigation.screenAccuracy
    property alias deviated: navigation.deviated
    property bool hasRoute: false
    property alias guidanceModeEnabled: navigation.guidanceModeEnabled

    onVisibleChanged: {
        if (visible) {
            marbleItem.setZoomToMaximumLevel();
            marbleItem.centerOnCurrentPosition();
        }

        navigation.guidanceModeEnabled = visible;
    }

    Settings {
        id: settings
        Component.onDestruction: {
            settings.setValue("Navigation", "muted", muteButton.muted)
        }
    }

    BorderImage {
        anchors.fill: infoBar
        anchors.margins: -14
        visible: infoBar.visible
        border { top: 14; left: 14; right: 14; bottom: 14 }
        source: "qrc:///border_shadow.png"
    }

    NavigationInfoBar {
        id: infoBar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        visible: root.hasRoute
        instructionIcon: navigation.nextInstructionImage.replace("qrc:/", "qrc:///");
        distance: navigation.nextInstructionDistance;
        destinationDistance: navigation.destinationDistance
    }

    RoundButton {
        id: muteButton
        visible: root.hasRoute

        property bool muted: settings.value("Navigation", "muted") === "true"

        anchors.right: infoBar.right
        anchors.rightMargin: Screen.pixelDensity * 3
        anchors.top: infoBar.bottom
        anchors.topMargin: Screen.pixelDensity * 5
        icon.source: muted ? "qrc:///material/volume-off.svg" : "qrc:///material/volume-on.svg"
        onClicked: muted = !muted
    }

    Navigation {
        id: navigation
        marbleQuickItem: marbleItem

        onVoiceNavigationAnnouncementChanged: {
            if (root.visible && root.hasRoute && !muteButton.muted) {
                textToSpeechClient.readText(voiceNavigationAnnouncement);
            }
        }
    }
}
