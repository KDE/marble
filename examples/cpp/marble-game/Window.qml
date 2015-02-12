//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//


import QtQuick 2.0

Rectangle {
    property real partition: 1/4
    property real spacingFraction: 1/10

    property bool showInitialMenu: true
    property bool showGameOptions: false

    property int leftPanelWidth: 200
    property int leftPanelHeight: 600
    signal browseMapButtonClicked()

    id: leftPanel
    objectName: "leftPanel"
    width: leftPanelWidth
    height: leftPanelHeight

    state: "GAME_OPTIONS_HIDDEN"

    /*
     * Provides two initial options
     * "Browse Map" and "Play Game"
     */
    Rectangle {
        id: buttonArea
        objectName: "buttonArea"
        width: leftPanelWidth
        height: leftPanelHeight
        color: "#d3d7cf"

        anchors.centerIn:leftPanel

        visible: showInitialMenu

        Column {
            anchors.centerIn: buttonArea
            spacing: buttonArea.height*spacingFraction

            CustomButton {
                id: browseMapButton
                buttonWidth: leftPanel.width*4/5
                normalColor: "#114269"
                borderColor: "#000000"
                labelText: qsTr("Browse Map")
                labelColor: "white"

                onButtonClick: {
                    browseMapButtonClicked();
                }
            }

            CustomButton {
                id: gameButton
                buttonWidth: leftPanel.width*4/5
                normalColor: "#114730"
                borderColor: "#000000"
                labelText: qsTr("Play Game")
                labelColor: "white"

                onButtonClick: {
                    if ( leftPanel.state == "GAME_OPTIONS_HIDDEN" ) {
                        leftPanel.state = "GAME_OPTIONS_VISIBLE"
                    }
                }
            }
        }
    }

    // This element contains the game menu
    GameOptions {
        id: "gameOptions"
        objectName: "gameOptions"
        gameOptionsPanelWidth: leftPanelWidth
        gameOptionsPanelHeight: leftPanelHeight

        anchors.centerIn: leftPanel
        visible: showGameOptions

        onBackButtonClick: {
            if ( leftPanel.state == "GAME_OPTIONS_VISIBLE" ) {
                leftPanel.state = "GAME_OPTIONS_HIDDEN"
            }
        }
    }

    states: [
        /*
         * State when game menu is visible.
         * At this point we remove the two
         * initial options "Browse Map" and
         * "Play Game".
         */
        State {
            name: "GAME_OPTIONS_VISIBLE"
            PropertyChanges { target: gameOptions; width: leftPanelWidth; height: leftPanelHeight; visible: true }
            PropertyChanges { target: buttonArea; width:0; height: 0; visible: false }
        },

        /*
         * State hides the game menu and takes
         * the user to initial menu. It can be
         * achieved by clicking on "Main Menu"
         * button in game menu
         */
        State {
            name: "GAME_OPTIONS_HIDDEN"
            PropertyChanges { target: buttonArea; width: leftPanelWidth; height: leftPanelHeight; visible: true }
            PropertyChanges { target: gameOptions; width:0; height: 0; visible: false }
        }
    ]
    
    transitions: [
        Transition {
            to: "*"
            NumberAnimation { target: buttonArea; properties: "height, width"; duration: 150 }
            NumberAnimation { target: gameOptions; properties: "height, width"; duration: 150 }
        }
    ]

    function resizeWindow(windowHeight) {
        leftPanelHeight = windowHeight;
    }
}
