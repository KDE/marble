//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015 Abhinav Gangwar <abhgang@gmail.com>
//


import QtQuick 2.0
import QtQuick.Controls 1.4


/*
* Provides two initial options
* "Browse Map" and "Play Game"
*/
Rectangle {
    property real partition: 1/4
    property real spacingFraction: 1/10

    signal gameMenuButtonClicked()
    signal browseButtonClicked()

    id: buttonArea
    objectName: "buttonArea"

    color: "#d3d7cf"

    Column {
        id: initialMenuLayout
        anchors.centerIn: buttonArea
        spacing: buttonArea.height*spacingFraction

        CustomButton {
            id: browseMapButton
            buttonWidth: buttonArea.width*4/5
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
            buttonWidth: buttonArea.width*4/5
            normalColor: "#114730"
            borderColor: "#000000"
            labelText: qsTr("Play Game")
            labelColor: "white"

            onButtonClick: {
                gameMenuButtonClicked();
            }
        }
    }
}
