// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
import QtMultimediaKit 1.1
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

/*
 * General preferences
 */
Page {
    id: preferencesPage

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
    }

    Grid {
        id: pageGrid
        anchors.fill: parent
        anchors.margins: 5

        columns: inPortrait ? 1 : 2
        rows: 10 / columns
        spacing: inPortrait ? 20 : 5

        property int leftRowWidth: inPortrait ? parent.width : 250
        property int rightRowWidth: inPortrait ? parent.width : width - leftRowWidth - spacing

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
                text: "Online mode"
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
            height: speakerHelp.height + speakersSwitch.height

            ButtonRow {
                id: speakersSwitch
                width: parent.width
                checkedButton: settings.voiceNavigationMuted ? b1 : ( settings.voiceNavigationSoundEnabled ? b2 : b3 )

                Button {
                    id: b1
                    text: "Disabled"
                    onCheckedChanged: {
                        if (checked) {
                            settings.voiceNavigationMuted = true
                            speakerHelp.text = "Turn instructions are not announced by sound/voice."
                        }
                    }
                }

                Button {
                    id: b2
                    text: "Sound"
                    onCheckedChanged: {
                        if (checked) {
                            settings.voiceNavigationMuted = false
                            settings.voiceNavigationSoundEnabled = true
                            speakerHelp.text = "A sound is played when approaching turn points during Navigation."
                        }
                    }
                }

                Button {
                    id: b3
                    text: "Speaker"
                    onCheckedChanged: {
                        if (checked) {
                            settings.voiceNavigationMuted = false
                            settings.voiceNavigationSoundEnabled = false
                            speakerHelp.text = "Turn instructions are spoken when approaching them."
                        }
                    }
                    onClicked: speakerDialog.open()

                    SpeakersModel{ id: speakers }

                    SelectionDialog {
                        id: speakerDialog
                        titleText: "Voice Navigation Speaker"
                        selectedIndex: speakers.indexOf(settings.voiceNavigationSpeaker)
                        model: speakers
                        onAccepted: {
                            if ( speakers.isLocal(selectedIndex) ) {
                                settings.voiceNavigationSpeaker = speakers.path(selectedIndex)
                            } else {
                                voiceNavigationPreviewButton.enabled = false
                                speakers.install(selectedIndex)
                            }
                        }

                        Connections {
                            target: speakers
                            onInstallationFinished: {
                                settings.voiceNavigationSpeaker = speakers.path(speakerDialog.selectedIndex)
                                voiceNavigationPreviewButton.enabled = true
                            }
                        }
                    }
                }
            }

            Item {
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.top: speakersSwitch.bottom
                anchors.topMargin: 5

                Label {
                    anchors.left: parent.left
                    anchors.right: voiceNavigationPreviewButton.left
                    id: speakerHelp
                    color: "gray"
                    font.pixelSize: 16
                }

                ToolButton {
                    id: voiceNavigationPreviewButton
                    anchors.right: parent.right
                    anchors.margins: 5
                    visible: !settings.voiceNavigationMuted
                    iconSource: "image://theme/icon-m-toolbar-mediacontrol-play";
                    checkable: true
                    checked: false
                    width: 60

                    VoiceNavigation {
                        id: voiceNavigationPreview
                        speaker: settings.voiceNavigationSpeaker
                        isSpeakerEnabled: !settings.voiceNavigationSoundEnabled
                    }

                    Audio {
                        id: voiceNavigationPreviewPlayer
                        source: "file://" + voiceNavigationPreview.preview
                        playing: voiceNavigationPreviewButton.checked
                        onStatusChanged: {
                            if ( status == Audio.EndOfMedia ) {
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
                text: marbleWidget.streetMapThemeModel.name(settings.streetMapTheme)
                onClicked: themeDialog.open()

                SelectionDialog {
                    id: themeDialog
                    titleText: "Street Map Theme"
                    selectedIndex: marbleWidget.streetMapThemeModel.indexOf(settings.streetMapTheme)
                    model: marbleWidget.streetMapThemeModel
                    delegate:
                        Rectangle {
                        id: delegate
                        width: root.width
                        height: mapImage.height

                        color: index === themeDialog.selectedIndex ? root.platformStyle.itemSelectedBackgroundColor : root.platformStyle.itemBackgroundColor

                        Row {
                            anchors.verticalCenter: parent.verticalCenter
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
                                color: delegate.index === themeDialog.selectedIndex ? root.platformStyle.itemSelectedTextColor : root.platformStyle.itemTextColor
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                themeDialog.selectedIndex = index
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
            text: "Additional Map Themes"
        }

        Button {
            id: manageThemeButton
            text: "Manage"
            onClicked: pageStack.push(themePage)
            MapThemePage { id: themePage }
        }
    }
}
