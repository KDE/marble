// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
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

        columns: 2
        rows: 3
        spacing: 10

        property int leftRowWidth: 250
        property int rightRowWidth: width - leftRowWidth - spacing

        Label {
            id: offlineLabel
            width: pageGrid.leftRowWidth
            text: "Connection"
        }

        Item {
            id: onlineSettings
            width: pageGrid.rightRowWidth
            anchors.right: parent.right
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
            text: "Navigation"
        }

        Item {
            id: screensaverSettings
            width: pageGrid.rightRowWidth
            height: screensaverHelp.height + screensaverSwitch.height

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
                text: "Disable screensaver while navigating"
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
            text: "Street Map Theme"
        }

        Item {
            width: pageGrid.rightRowWidth
            //anchors.bottom: parent.bottom
            height: pageGrid.height - screensaverSettings.height - pageGrid.spacing - onlineSettings.height - pageGrid.spacing

            ListView {
                id: mapListView
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: mapThemeLabel.top
                model: marbleWidget.streetMapThemeModel
                clip: true

                delegate:
                    Rectangle {
                    width: mapListView.width
                    height: mapImage.height

                    /** @todo FIXME Find a way to make this the current index on startup, and use a highlight */
                    color: mapThemeId === settings.streetMapTheme ? "lightsteelblue" : "#00000000"

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
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            mapListView.currentIndex = index
                            settings.streetMapTheme = mapThemeId
                        }
                    }
                }

                ScrollDecorator {
                    flickableItem: mapListView
                }
            }

            Label {
                id: mapThemeLabel
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                color: "gray"
                text: "Select the map to use in the Search, Routing, Tracking and Friends activities."
                font.pixelSize: 16
            }
        }
    }
}
