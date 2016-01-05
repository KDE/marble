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
import QtQuick.Controls 1.4

Rectangle {
    property bool showInitialMenu: true
    property bool showGameOptions: false

    property int leftPanelWidth: 200
    property int leftPanelHeight: 600
    signal browseMapButtonClicked()

    id: leftPanel
    objectName: "leftPanel"
    width: leftPanelWidth
    height: leftPanelHeight

    StackView {
        id: stackContainer
        anchors.fill: parent
        initialItem: buttonArea
    }

    InitialMenu {
        id: buttonArea
        objectName: "buttonArea"

        onGameMenuButtonClicked: {
            stackContainer.push(gameOptions);
        }
        onBrowseButtonClicked: {
            browseMapButtonClicked();
            stackContainer.pop(gameOptions);
        }
    }

    // This element contains the game menu
    GameOptions {
        id: gameOptions
        objectName: "gameOptions"

        onBackButtonClick: {
            stackContainer.pop(buttonArea);
        }
    }

    Component.onCompleted: {
        stackContainer.push(buttonArea);
    }

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
