// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 2.3
import org.kde.edu.marble 0.20
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtMultimedia 5.4

/*
 * General preferences
 */
Item {
    id: preferencesPage

    RowLayout {
        id: toolBar
        anchors.fill: parent
        ToolButton {
            text: "Home"
            onClicked: activitySelection.showActivities()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "white"
    }

    Flickable {
        id: pageFlickable
        anchors.fill: parent
        anchors.margins: 5

        contentHeight: pageGrid.height

        Grid {
            id: pageGrid
            width: parent.width - 20

            columns: main.inPortrait ? 1 : 2
            rows: 10 / columns
            spacing: main.inPortrait ? 10 : 5

            property int leftRowWidth: main.inPortrait ? width : 250
            property int rightRowWidth: main.inPortrait ? width : width - leftRowWidth - spacing

            Label {
                id: offlineLabel
                width: pageGrid.leftRowWidth
                text: "Connection"
            }

            Item {
                id: onlineSettings
                width: pageGrid.rightRowWidth
                height: onlineHelp.height + onlineSwitch.height

                Switch {
                    id: onlineSwitch
                    height: 40
                    checked: !settings.workOffline
                    onCheckedChanged: settings.workOffline = !checked
                }

                Label {
                    id: onlineLabel
                    anchors.left: onlineSwitch.right
                    anchors.right: parent.right
                    anchors.leftMargin: 5
                    anchors.verticalCenter: onlineSwitch.verticalCenter
                    text: "Online"
                }

                Label {
                    id: onlineHelp
                    anchors.top: onlineSwitch.bottom
                    anchors.topMargin: 5
                    anchors.left: onlineSwitch.left
                    anchors.right: parent.right

                    property string activatedText: "Map data is downloaded as needed. Search and route calculation use online services."
                    property string deactivatedText: "Only data available offline is used to display maps, search and calculate routes."

                    color: "gray"
                    text: onlineSwitch.checked ? activatedText : deactivatedText

                    font.pixelSize: 16
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Label {
                width: pageGrid.leftRowWidth
                text: "Screensaver"
            }

            Item {
                id: screensaverSettings
                width: pageGrid.rightRowWidth
                height: screensaverHelp.height + Math.max(screensaverSwitch.height, screensaverLabel.height)

                Switch {
                    id: screensaverSwitch
                    height: 40
                    checked: settings.inhibitScreensaver
                    onCheckedChanged: settings.inhibitScreensaver = checked
                }

                Label {
                    id: screensaverLabel
                    anchors.left: screensaverSwitch.right
                    anchors.leftMargin: 5
                    anchors.right: parent.right
                    anchors.verticalCenter: screensaverSwitch.verticalCenter
                    text: "Navigation disables screensaver"
                }

                Label {
                    id: screensaverHelp
                    anchors.top: screensaverSwitch.bottom
                    anchors.left: screensaverSwitch.left
                    anchors.right: parent.right
                    anchors.topMargin: 5

                    property string activatedText: "The automatic start of the screensaver is inhibited in the Tracking activity. You can still enable it manually."
                    property string deactivatedText: "The screensaver is starting according to your device settings."

                    color: "gray"
                    text: screensaverSwitch.checked ? activatedText : deactivatedText
                    font.pixelSize: 16
                }
            }

            Label {
                width: pageGrid.leftRowWidth
                text: "Voice Navigation"
            }

            Item {
                id: speakerSettings
                width: pageGrid.rightRowWidth
                height: speakersSwitch.height + speakersItem.height

                RowLayout {
                    id: speakersSwitch
                    ExclusiveGroup { id: voiceGroup }
                    width: parent.width

                    RadioButton {
                        id: b1
                        text: "Disabled"
                        checked: true
                        exclusiveGroup: voiceGroup
                        onCheckedChanged: {
                            if (checked) {
                                settings.voiceNavigationMuted = true
                                speakerHelp.text = "Turn instructions are not announced by sound/voice."
                            }
                        }
                    }

                    RadioButton {
                        id: b2
                        text: "Sound"
                        exclusiveGroup: voiceGroup
                        onCheckedChanged: {
                            if (checked) {
                                settings.voiceNavigationMuted = false
                                settings.voiceNavigationSoundEnabled = true
                                speakerHelp.text = "A sound is played when approaching turn points during Navigation."
                            }
                        }
                    }

                    RadioButton {
                        id: b3
                        text: "Speaker"
                        exclusiveGroup: voiceGroup
                        onCheckedChanged: {
                            if (checked) {
                                settings.voiceNavigationMuted = false
                                settings.voiceNavigationSoundEnabled = false
                                speakerHelp.text = "Turn instructions are spoken when approaching them."
                            }
                        }
                        property SelectionDialog dialog
                        onClicked: {
                            if (dialog === null) {
                                dialog = speakerComponent.createObject(b3)
                            }
                            dialog.open()
                        }

                        Component {
                            id: speakerComponent
                            SelectionDialog {
                                id: speakerDialog
                                titleText: "Voice Navigation Speaker"
                                currentIndex: speakers.indexOf(settings.voiceNavigationSpeaker)

                                model: SpeakersModel {
                                    id: speakers
                                    onCountChanged: speakerDialog.currentIndex = speakers.indexOf(settings.voiceNavigationSpeaker)
                                    onInstallationProgressed: {
                                        progressBar.indeterminate = false
                                        progressBar.value = progress
                                    }
                                    onInstallationFinished: {
                                        progressBar.visible = false
                                        settings.voiceNavigationSpeaker = speakers.path(speakersDialog.currentIndex)
                                        voiceNavigationPreviewButton.enabled = true
                                    }
                                }

                                delegate: Item {
                                    height: 20
                                    width: speakerDialog.width
                                    Text {
                                        text: name
                                    }
                                }

                                onAccepted: {
                                    if ( speakers.isLocal(currentIndex) ) {
                                        settings.voiceNavigationSpeaker = speakers.path(currentIndex)
                                    } else {
                                        voiceNavigationPreviewButton.enabled = false
                                        progressBar.visible = true
                                        speakers.install(currentIndex)
                                    }
                                }
                            }
                        }
                    }

                    Component.onCompleted: {
                        if (settings.voiceNavigationMuted) {
                            b1.checked = true
                        } else {
                            if (settings.voiceNavigationSoundEnabled) {
                                b2.checked = true
                            } else {
                                b3.checked = true
                            }
                        }
                    }
                }

                Item {
                    id: speakersItem
                    anchors.right: parent.right
                    anchors.left: parent.left
                    anchors.top: speakersSwitch.bottom
                    anchors.topMargin: 5
                    height: Math.max(speakerHelp.height, voiceNavigationPreviewButton.height)

                    Label {
                        id: speakerHelp
                        visible: !progressBar.visible
                        anchors.left: parent.left
                        anchors.right: voiceNavigationPreviewButton.left
                        color: "gray"
                        font.pixelSize: 16
                    }

                    ProgressBar {
                        id: progressBar
                        visible: false
                        anchors.left: parent.left
                        anchors.right: voiceNavigationPreviewButton.left
                        minimumValue: 0.0
                        maximumValue: 1.0
                        indeterminate: true
                    }

                    ToolButton {
                        id: voiceNavigationPreviewButton
                        anchors.right: parent.right
                        anchors.margins: 5
                        visible: !settings.voiceNavigationMuted
                        iconSource: main.icon( "actions/media-playback-start", 48 );
                        checkable: true
                        checked: false
                        onCheckedChanged: {
                            if (checked) {
                                voiceNavigationPreviewPlayer.play()
                            } else {
                                voiceNavigationPreviewPlayer.pause()
                            }
                        }
                        width: 60

                        VoiceNavigation {
                            id: voiceNavigationPreview
                            speaker: settings.voiceNavigationSpeaker
                            isSpeakerEnabled: !settings.voiceNavigationSoundEnabled
                        }

                        Audio {
                            id: voiceNavigationPreviewPlayer
                            source: "file://" + voiceNavigationPreview.preview
                            onPlaybackStateChanged: {
                                if ( playbackState == Audio.StoppedState ) {
                                    voiceNavigationPreviewButton.checked = false
                                }
                            }
                        }
                    }
                }
            }

            Label {
                width: pageGrid.leftRowWidth
                text: "Street Map Theme"
            }

            Item {
                width: pageGrid.rightRowWidth
                height: themeSelectionButton.height + mapThemeLabel.height

                Button {
                    id: themeSelectionButton
                    text: mapThemeModel.name(settings.streetMapTheme)
                    onClicked: themeDialog.open()

                    MapThemeModel {
                        id: mapThemeModel
                        mapThemeFilter: MapThemeModel.Extraterrestrial | MapThemeModel.LowZoom
                    }

                    SelectionDialog {
                        id: themeDialog
                        titleText: "Street Map Theme"
                        currentIndex: mapThemeModel.indexOf(settings.streetMapTheme)

                        model: mapThemeModel

                        delegate:
                            Rectangle {
                            id: delegate
                            width: row.width
                            height: row.height

                            color: index === themeDialog.currentIndex ? "lightsteelblue" : "white"

                            Row {
                                id: row
                                Image {
                                    id: mapImage
                                    source: "image://maptheme/" + mapThemeId
                                    smooth: true
                                    width: 68
                                    height: 68
                                }
                                Label {
                                    id: themeLabel
                                    text: display
                                    color: index === themeDialog.currentIndex ? "black" : "gray"
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    themeDialog.currentIndex = index
                                    themeDialog.accept()
                                    settings.streetMapTheme = mapThemeId
                                }
                            }
                        }
                    }
                }

                Label {
                    id: mapThemeLabel
                    anchors.top: themeSelectionButton.bottom
                    width: pageGrid.rightRowWidth
                    color: "gray"
                    text: "Select the map to use in the Search, Routing, Navigation, Tracking and Friends activities."
                    font.pixelSize: 16
                }
            }

            Label {
                width: pageGrid.leftRowWidth
                text: "Data Management"
            }

            Item {
                width: pageGrid.rightRowWidth
                height: manageThemeButton.height

                Button {
                    id: manageThemeButton
                    width: pageGrid.rightRowWidth / 2 - 5
                    text: "Map Themes"
                    onClicked: pageStack.push(themePage)
                    Component {
                        id: themePage
                        MapThemePage {  }
                    }
                }

                Button {
                    id: manageOfflineDataButton
                    anchors.left: manageThemeButton.right
                    anchors.leftMargin: 5
                    anchors.right: parent.right
                    text: "Offline Data"
                    onClicked: pageStack.push(offlineDataPage)
                    Component {
                        id: offlineDataPage
                        OfflineDataPage {  }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        mainWindow.toolBar.replaceWith(toolBar)
    }
}
