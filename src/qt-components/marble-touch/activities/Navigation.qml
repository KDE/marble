// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import com.nokia.meego 1.0
import QtMobility.systeminfo 1.1
import QtMultimediaKit 1.1
import org.kde.edu.marble 0.11
import org.kde.edu.marble.qtcomponents 0.12

/*
 * Page for navigation activity.
 */
Page {
    id: navigationActivityPage
    anchors.fill: parent

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
        ToolButton {
            iconSource: "image://theme/icon-m-toolbar-volume-off";
            checkable: true
            checked: settings.voiceNavigationMuted
            onCheckedChanged: settings.voiceNavigationMuted = checked
            width: 60
            flat: true
        }
        ToolButton {
            iconSource: "image://theme/icon-m-common-no-internet-connection";
            checkable: true
            checked: settings.workOffline
            onCheckedChanged: settings.workOffline = checked
            width: 60
            flat: true
        }
        ToolIcon {
            iconId: "toolbar-view-menu"
            onClicked: pageMenu.open()
        }
    }

    Menu {
        id: pageMenu
        content: MenuLayout {
            MenuItemSwitch {
                text: "Elevation Profile"
                checked: false
                onCheckedChanged: {
                    var plugins = settings.activeRenderPlugins
                    if ( checked ) {
                        plugins.push("elevationprofile")
                    } else {
                        settings.removeElementsFromArray(plugins, ["elevationprofile"])
                    }
                    settings.activeRenderPlugins = plugins
                    marbleWidget.setGeoSceneProperty( "hillshading", checked )
                }
            }
        }
    }

    Item {
        id: instructionItem

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: Math.max(instructionImage.height, instructionDistance.height)

        Image {
            id: instructionImage
            anchors.verticalCenter: parent.verticalCenter
            width: 64
            height: 64
            smooth: true
            source: marbleWidget.navigation.nextInstructionImage
            onSourceChanged: {
                fadeAnimation.running = true
            }
        }

        Label {
            id: instructionDistance
            anchors.left: instructionImage.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right

            property bool isMeter: marbleWidget.navigation.nextInstructionDistance < 1000
            property real distance: isMeter ? Math.round( marbleWidget.navigation.nextInstructionDistance / 10 ) * 10 : Math.round( marbleWidget.navigation.nextInstructionDistance / 100 ) / 10
            property string distanceUnit: isMeter ? "m" : "km"

            color: Qt.rgba(0, 87/255, 174/255, 1)
            text: "<font size=\"+2\">" + distance + "</font>" + "<font size=\"-1\"> " + distanceUnit + "</font>" + "<font size=\"+2\"> " + marbleWidget.navigation.nextRoad + "</font>"
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        ParallelAnimation {
            id: fadeAnimation
            PropertyAnimation { target: instructionItem; property: "scale"; from: 0.75; to: 1.0; duration: 200 }
            PropertyAnimation { target: instructionItem; property: "opacity"; from: 0.2; to: 1.0; duration: 200 }
        }
    }

    Grid {
        id: searchResultView
        anchors.left: navigationActivityPage.left
        anchors.bottom: navigationActivityPage.bottom
        anchors.right: navigationActivityPage.right
        anchors.margins: 10
        height: 100
        spacing: 20
        property bool portrait: main.inPortrait

        columns: portrait ? 4 : 1
        rows: portrait ? 1 : 4

        Rectangle {
            id: speedSeparator
            visible: !parent.portrait
            width: parent.portrait ? 100 : parent.width - 20
            height: 1
            color: "black"
        }

        Label {
            id: currentSpeed
            width: parent.portrait ? 150 : parent.width - 20
            text: "<font size=\"+2\">" + Math.round( marbleWidget.tracking.positionSource.speed ) + "</font><font size=\"-1\"> km/h</font>"
        }

        Rectangle {
            id: distanceSeparator
            width: parent.portrait ? 1 : parent.width - 20
            height: parent.portrait ? parent.height - 20 : 1
            anchors.verticalCenter: parent.portrait ? parent.verticalCenter : undefined
            color: "black"
        }

        Label {
            id: destinationDistance
            width: parent.portrait ? 150 : parent.width - 20
            property bool isMeter: marbleWidget.navigation.destinationDistance < 1000
            property real distance: isMeter ? Math.round( marbleWidget.navigation.destinationDistance / 10 ) * 10 : Math.round( marbleWidget.navigation.destinationDistance / 100 ) / 10
            property string distanceUnit: isMeter ? "m" : "km"
            text: "<img src=\"qrc:/marble/flag-green.svg\"> <font size=\"+2\">" + distance + "</font><font size=\"-1\"> " + distanceUnit + "</font>"
        }
    }

    Item {
        id: mapContainer
        clip: true
        anchors.top: instructionItem.bottom
        anchors.right: navigationActivityPage.right
        anchors.left: navigationActivityPage.left
        anchors.bottom: searchResultView.top

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Mercator"
            var plugins = settings.defaultRenderPlugins
            settings.removeElementsFromArray(plugins, ["coordinate-grid", "sun", "stars", "compass", "crosshairs"])
            plugins.push("positionMarker")
            settings.activeRenderPlugins = plugins
            settings.mapTheme = settings.streetMapTheme
            settings.gpsTracking = true
            settings.showPosition = true
            settings.showTrack = true
            marbleWidget.navigation.guidanceModeEnabled = true
            marbleWidget.visible = true
        }

        Component.onDestruction: {
            if ( marbleWidget.parent === mapContainer ) {
                marbleWidget.parent = null
                marbleWidget.visible = false
            }
        }
    }

    StateGroup {
        // Here be dragons. Order is crucial in the changes below
        states: [
            State { // Horizontal
                when: (navigationActivityPage.width / navigationActivityPage.height) > 1.20
                AnchorChanges { target: searchResultView; anchors.top: instructionItem.bottom; anchors.right: undefined }
                AnchorChanges { target: mapContainer; anchors.left: searchResultView.right; anchors.bottom: navigationActivityPage.bottom; }
                PropertyChanges { target: searchResultView; height: navigationActivityPage.height-instructionItem.height; width: navigationActivityPage.width * 0.2; }
            },
            State { // Vertical
                when: (true)
                AnchorChanges { target: searchResultView; anchors.top: undefined; anchors.right: navigationActivityPage.right }
                AnchorChanges { target: mapContainer; anchors.left: navigationActivityPage.left; anchors.bottom: searchResultView.top; }
                PropertyChanges { target: searchResultView; height: destinationDistance.height; width: navigationActivityPage.width }
            }
        ]
    }

    ScreenSaver {
        id: saver
    }

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
            saver.screenSaverDelayed = settings.inhibitScreensaver
        } else if ( status === PageStatus.Deactivating ) {
            saver.screenSaverDelayed = false
        }
    }

    Audio {
        id: playback
    }

    Connections { target: marbleWidget.navigation; onVoiceNavigationAnnouncementChanged: voiceAnnouncement() }

    function voiceAnnouncement() {
        playback.source = "file://" + marbleWidget.navigation.voiceNavigationAnnouncement
        playback.play()
    }
}
