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
        columns: 2
        rows: 2

        Label {
            id: screenSaverLabel
            height: 50
            width: 250
            text: "Navigation"
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            width: preferencesPage.width - screenSaverLabel.width
            height: 40

            Switch {
                id: screensaverSwitch
                height: 40
                checked: settings.inhibitScreensaver
                onCheckedChanged: settings.inhibitScreensaver = checked
            }

            Label {
                height: 40
                anchors.left: screensaverSwitch.right
                text: "Disable screensaver while navigating"
                verticalAlignment: Text.AlignVCenter
            }
        }

        Label {
            width: 250
            height: 50
            text: "Street Map Theme"
        }

        ListView {
            id: mapListView
            width: preferencesPage.width - screenSaverLabel.width
            height: preferencesPage.height - screenSaverLabel.height
            model: marbleWidget.mapThemeModel
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
        }

        ScrollDecorator {
            flickableItem: mapListView
        }
    }
}
