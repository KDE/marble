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
        id: searchResultView
        width: parent.width * 0.33
        height: parent.height

        Item {
            anchors.margins: 10
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            id: instructionItem
            height: instructionImage.height + instructionText.height

            Image {
                id: instructionImage
                width: 64
                height: 64
                smooth: true
                source: marbleWidget.getNavigation().nextInstructionImage
                onSourceChanged: {
                    fadeAnimation.running = true
                }
            }

            Label {
                id: instructionDistance
                anchors.left: instructionImage.right
                anchors.verticalCenter: instructionImage.verticalCenter
                width: parent.width
                color: Qt.rgba(0, 87/255, 174/255, 1)
                text: formatDistance( marbleWidget.getNavigation().nextInstructionDistance )

                function formatDistance(distance)
                {
                    if ( distance < 1000 ) {
                        return Math.round( distance ) + " m";
                    } else {
                        return Math.round( distance / 100 ) / 10 + " km"
                    }
                }
            }

            Label {
                id: instructionText
                anchors.top: instructionImage.bottom
                width: parent.width
                color: Qt.rgba(0, 87/255, 174/255, 1)
                text: marbleWidget.getNavigation().nextRoad
            }

            ParallelAnimation {
                id: fadeAnimation
                PropertyAnimation { target: instructionItem; property: "scale"; from: 0.75; to: 1.0; duration: 200 }
                PropertyAnimation { target: instructionItem; property: "opacity"; from: 0.2; to: 1.0; duration: 200 }
            }
        }

        Rectangle {
            id: separator
            anchors.top: instructionItem.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            height: 1
            color: "black"
        }

        Label {
            id: currentSpeed
            anchors.top: separator.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            text: Math.round( marbleWidget.getTracking().positionSource.speed ) + " km/h"
        }
    }

    Item {
        id: mapContainer
        clip: true
        width: parent.width * 0.67
        height: parent.height

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
            marbleWidget.getNavigation().guidanceModeEnabled = true
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
        states: [
            State { // Horizontal
                when: (navigationActivityPage.width / navigationActivityPage.height) > 1.20
                AnchorChanges { target: searchResultView; anchors.top: navigationActivityPage.top }
                PropertyChanges { target: searchResultView; width: navigationActivityPage.width * 0.33; height: navigationActivityPage.height }
                AnchorChanges { target: mapContainer; anchors.left: searchResultView.right; anchors.bottom: navigationActivityPage.bottom; anchors.top: navigationActivityPage.top; }
                PropertyChanges { target: mapContainer; width: navigationActivityPage.width * 0.67; height: navigationActivityPage.height }
            },
            State { // Vertical
                when: (true)
                AnchorChanges { target: mapContainer; anchors.left: navigationActivityPage.left; anchors.top: navigationActivityPage.top }
                PropertyChanges { target: mapContainer; width: navigationActivityPage.width; height: navigationActivityPage.height * 0.67 }
                AnchorChanges { target: searchResultView; anchors.right: navigationActivityPage.right; anchors.top: mapContainer.bottom }
                PropertyChanges { target: searchResultView; width: navigationActivityPage.width; height: navigationActivityPage.height * 0.33 }
            }
        ]
    }


    Component {
        id: turnTypeDelegate

        Column {
            width: parent.width
            spacing: 4

            Row {
                id: row
                width: parent.width
                Image {
                    id: turnIcon
                    width: 64; height: 64
                    source: "qrc" + turnTypeIcon
                }

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    platformStyle: LabelStyle { fontPixelSize: 18 }
                    text: display;
                    width: parent.width - turnIcon.width - 20
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            waypointListView.currentIndex = index
                            marbleWidget.centerOn(longitude, latitude)
                        }
                    }
                }
            }

            Rectangle {
                x: 5
                width: parent.width - 20
                height: 1
                color: "white"
            }
        }
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

    Connections { target: marbleWidget.getNavigation(); onVoiceNavigationAnnouncementChanged: voiceAnnouncement() }

    function voiceAnnouncement() {
        console.log("announce" + marbleWidget.getNavigation().voiceNavigationAnnouncement )
    }
}
