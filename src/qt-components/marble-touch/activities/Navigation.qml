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
        MarbleToolIcon {
            iconSource: main.icon( "actions/go-home", 48 );
            onClicked: main.showNavigation()
        }
        ToolButton {
            iconSource: main.icon( "actions/text-speak", 48 );
            checkable: true
            checked: !settings.voiceNavigationMuted
            onCheckedChanged: settings.voiceNavigationMuted = !checked
            width: 60
            flat: true
        }
        ToolButton {
            iconSource: main.icon( "devices/network-wireless", 48 );
            checkable: true
            checked: !settings.workOffline
            onCheckedChanged: settings.workOffline = !checked
            width: 60
            flat: true
        }
        MarbleToolIcon {
            id: menuIcon
            iconSource: main.icon( "actions/show-menu", 48 );
            onClicked: {
                if (main.components === "plasma") {
                    pageMenu.visualParent = menuIcon
                }
                pageMenu.open()
            }
        }
    }

    Menu {
        id: pageMenu
        content: MarbleMenuLayout {
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

    Rectangle {
        id: instructionItem
        color: marbleWidget.navigation.deviated ? Qt.rgba(156/255, 15/255, 15/255, 1) : Qt.rgba(0/255, 67/255, 138/255, 1)

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

            color: Qt.rgba(238/255, 238/255, 236/255, 1)
            text: "<font size=\"+2\">" + distance + "</font>" + "<font size=\"-1\"> " + distanceUnit + "</font>" + "<font size=\"+2\"> " + marbleWidget.navigation.nextRoad + "</font>"
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }

        PropertyAnimation {
            id: fadeAnimation;
            target: instructionItem;
            property: "opacity";
            from: 0.2;
            to: 1.0;
            duration: 200
        }
    }

    Rectangle {
        id: searchResultView
        anchors.left: navigationActivityPage.left
        anchors.bottom: navigationActivityPage.bottom
        anchors.right: navigationActivityPage.right

        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(46/255, 52/255, 54/255, 1) }
            GradientStop { position: 0.8; color: Qt.rgba(36/255, 42/255, 44/255, 1) }
            GradientStop { position: 1.0; color: Qt.rgba(26/255, 32/255, 34/255, 1) }
        }

        Grid {
            anchors.fill: parent
            anchors.margins: 5
            spacing: 20
            property bool portrait: main.inPortrait

            columns: portrait ? 4 : 1
            rows: portrait ? 1 : 4

            Label {
                id: currentSpeed
                width: parent.portrait ? parent.width / 2 : parent.width - 10
                color: Qt.rgba(238/255, 238/255, 236/255, 1)
                text: "<font size=\"+2\">" + Math.round( marbleWidget.tracking.positionSource.speed ) + "</font><font size=\"-1\"> km/h</font>"
                horizontalAlignment: parent.portrait ? Text.AlignHCenter : Text.AlignRight

                MouseArea {
                    anchors.fill: parent
                    onClicked: marbleWidget.centerOn( marbleWidget.tracking.lastKnownPosition.longitude, marbleWidget.tracking.lastKnownPosition.latitude )
                }
            }

            Rectangle {
                id: distanceSeparator
                visible: !parent.portrait
                width: parent.portrait ? 1 : parent.width
                height: parent.portrait ? parent.height - 20 : 1
                anchors.verticalCenter: parent.portrait ? parent.verticalCenter : undefined
                color: currentSpeed.color
            }

            Label {
                id: destinationDistance
                width: parent.portrait ? parent.width / 2 : parent.width - 10
                color: currentSpeed.color
                property bool isMeter: marbleWidget.navigation.destinationDistance < 1000
                property real distance: isMeter ? Math.round( marbleWidget.navigation.destinationDistance / 10 ) * 10 : Math.round( marbleWidget.navigation.destinationDistance / 100 ) / 10
                property string distanceUnit: isMeter ? "m" : "km"
                text: "<img src=\"qrc:/marble/flag-green.svg\"> <font size=\"+2\">" + distance + "</font><font size=\"-1\"> " + distanceUnit + "</font>"
                horizontalAlignment: parent.portrait ? Text.AlignHCenter : Text.AlignRight
            }
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
            settings.activeRenderPlugins = plugins
            settings.mapTheme = settings.streetMapTheme
            settings.gpsTracking = true
            settings.showPositionIndicator = true
            settings.showTrack = true
            marbleWidget.tracking.positionMarkerType = Tracking.Arrow
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
                PropertyChanges { target: searchResultView; height: destinationDistance.height+7; width: navigationActivityPage.width }
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
            if (!settings.navigationStartupWarningEverShown || settings.navigationStartupWarning) {
                safetyWarningDialog.open()
            }
        } else if ( status === PageStatus.Deactivating ) {
            saver.screenSaverDelayed = false
        }
    }

    Audio {
        id: playback
    }

    Dialog {
        id: safetyWarningDialog

        content: Column {
            width: parent.width
            spacing: 10

            Label {
            id: textLabel
            width: parent.width
            color: main.components === "harmattan" ? "white" : "black"
            text: "<p>Caution: Driving instructions may be incomplete or wrong. Road construction, weather and other unforeseen variables can result in the suggested route not to be the most expedient or safest route to your destination. Please use common sense while navigating.</p><p>The Marble development team wishes you a pleasant and safe journey.</p>"
            wrapMode: Text.Wrap
            }

            Row {
                CheckBox {
                    id: startupCheckbox
                    checked: settings.navigationStartupWarning
                    onCheckedChanged: settings.navigationStartupWarning = checked
                }
                Label {
                    text: "Show again";
                    color: textLabel.color
                    anchors.verticalCenter: startupCheckbox.verticalCenter
                }
            }

            Item {
                height: 5; width: 1
            }
        }

        buttons: ButtonRow {
            anchors.horizontalCenter: parent.horizontalCenter
            Button { text: "OK"; onClicked: safetyWarningDialog.accept() }
        }

        onAccepted: settings.navigationStartupWarningEverShown = true
    }

    Connections { target: marbleWidget.navigation; onVoiceNavigationAnnouncementChanged: voiceAnnouncement() }

    function voiceAnnouncement() {
        playback.source = "file://" + marbleWidget.navigation.voiceNavigationAnnouncement
        playback.play()
    }
}
