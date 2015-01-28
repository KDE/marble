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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
    property alias gameOptionsPanelWidth: gameOptionsPanel.width
    property alias gameOptionsPanelHeight: gameOptionsPanel.height

    //property real nItemInColumn: gameOptionsPanelLayout.children.size
    property real nItemInColumn: 3

    signal nextButtonClicked()
    signal gameClosed()
    signal answerDisplayButtonClicked()

    // These signals tell which game has been selected by user
    signal countryByShapeGameRequested()
    signal countryByFlagGameRequested()
    signal clickOnThatGameRequested()

    signal backButtonClick()

    id: gameOptionsPanel
    objectName: "gameOptionsPanel"
    width: 300
    height: 600

    state: "GAMES_VIEW_HIDDEN"

    Column {
        id: gameOptionsPanelLayout
        anchors.centerIn: parent
        spacing: gameOptionsPanel.height/( nItemInColumn * 4 )

        CustomButton {
            id: countryByShape
            buttonWidth: gameOptionsPanel.width*4/5
            buttonHeight: gameOptionsPanel.height*2/( nItemInColumn * 4 )
            normalColor: "#2ecc71"
            labelText: qsTr("Identify The Country Shape")
            labelColor: "white"
            onButtonClick: {
                gamesView.currentIndex = 0;
                slider.value = 0;
                gameOptionsPanel.state = "QUESTION_COUNT_QUERY";
            }
        }

        CustomButton {
            id: countryByFlag
            buttonWidth: gameOptionsPanel.width*4/5
            buttonHeight: gameOptionsPanel.height*2/( nItemInColumn * 4 )
            normalColor: "#2980b9"
            labelText: qsTr("Identify The Flag")
            labelColor: "white"
            onButtonClick: {
                gamesView.currentIndex = 1;
                slider.value = 0;
                gameOptionsPanel.state = "QUESTION_COUNT_QUERY";
            }
        }
        

        CustomButton {
            id: clickOnThat
            buttonWidth: gameOptionsPanel.width*4/5
            buttonHeight: gameOptionsPanel.height*2/( nItemInColumn * 4 )
            normalColor: "#8e44ad"
            labelText: qsTr("Identify The Location of Country")
            labelColor: "white"
            onButtonClick: {
                gamesView.currentIndex = 2;
                slider.value = 0;
                gameOptionsPanel.state = "QUESTION_COUNT_QUERY";
            }
        }

        CustomButton {
            id: backButton
            buttonWidth: gameOptionsPanel.width*4/5
            buttonHeight: gameOptionsPanel.height*2/( nItemInColumn * 4 )
            normalColor: "#d35400" //"#3498db"
            labelText: qsTr("Main Menu")
            labelColor: "white"

            onButtonClick: {
                backButtonClick()
            }
        }
    }

    GamesView {
        id: gamesView
        objectName: "gamesView"
        gamesDisplayWidth: parent.width
        gamesDisplayHeight: parent.height
    }

    Component.onCompleted: {
        gamesView.quitGame.connect(gameOptionsPanel.gameQuitRequested);
        gamesView.requestNextQuestion.connect(gameOptionsPanel.nextButtonClicked);
        gamesView.requestAnswerDisplay.connect(gameOptionsPanel.answerDisplayButtonClicked);

        countryByShapeGameRequested.connect(gamesView.initGame);
        countryByFlagGameRequested.connect(gamesView.initGame);
        clickOnThatGameRequested.connect(gamesView.initGame);
    }

    Rectangle {
        id: questionsCountInput
        objectName: "questionsCountInput"
        width: parent.width
        height: parent.height/9
        anchors.centerIn: parent

        Text {
            id: description
            objectName: "description"
            width: parent.width
            height: parent.height
            anchors.top: questionsCountInput.bottom
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10

            color: "gray"

            text: {
                "Total Questions: " + slider.value
            }
        }

        Slider {
            id: slider
            stepSize: 1
            minimumValue: 0
            maximumValue: 40

            style: SliderStyle {
                groove: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 8
                    color: "gray"
                    radius: 8
                }
                handle: Rectangle {
                    anchors.centerIn: parent
                    color: control.pressed ? "white" : "lightgray"
                    border.color: "gray"
                    border.width: 2
                    implicitWidth: 34
                    implicitHeight: 34
                    radius: 12
                }
            }
        }

        Text {
            id: numberOfQuestions
            objectName: "numberOfQuestions"
            width: parent.width
            height: parent.height
            anchors.top: questionsCountInput.bottom
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10

            color: "gray"

            text: {
                "Total Questions: " + slider.value
            }
        }

        CustomButton {
            id: okButton
            objectName: "okButton"
            normalColor: "yellow"
            labelColor: "blue"
            buttonWidth: parent.width/2
            buttonHeight: parent.height
            labelText: qsTr("OK")
            labelSize: parent.width/8
            anchors.top: numberOfQuestions.bottom
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10

            onButtonClick: {
                gamesView.setMaximumQuestionsCounts( slider.value );
                gameOptionsPanel.state = "GAMES_VIEW_VISIBLE";

                /**
                 * Emit signals for game initiation
                 * after the user has entered the
                 * number of questions he/she wants
                 * to attempt.
                 */
                if ( gamesView.currentIndex == 0 && slider.value > 0 ) {
                    countryByShapeGameRequested();
                }
                if ( gamesView.currentIndex == 1 && slider.value > 0 ) {
                    countryByFlagGameRequested();
                }
                if ( gamesView.currentIndex == 2 && slider.value > 0 ) {
                    clickOnThatGameRequested();
                }
            }
        }
    }


    /*
     * This function quits a particular game
     * and shows the user available games.
     * After that user can also switch to
     * main menu where we have "browse map"
     * and "play game" option
     */
    function gameQuitRequested() {
        if ( gameOptionsPanel.state == "GAMES_VIEW_VISIBLE" ) {
            gameOptionsPanel.state = "GAMES_VIEW_HIDDEN"
        }
        gameClosed();
    }

    /*
     * The following functions sets the questions
     * for different type of games
     */
    function countryByShapeQuestion( answerOptions, correctAnswer ) {
        gamesView.postCountryShapeQuestion( answerOptions, correctAnswer );
    }

    function countryByFlagQuestion( answerOptions, imageSource, rightAnswer ) {
        gamesView.postCountryFlagQuestion( answerOptions, imageSource, rightAnswer );
    }

    function clickOnThatQuestion( countryName ) {
        gamesView.postClickOnThatQuestion( countryName );
    }

    function displayResult( result ) {
        gamesView.displayResult( result );
    }

    states: [
        /*
         * State when a particular game is
         * shown ( decided on basis of what user has opted
         * out of from available games in game menu )
         */
        State {
            name: "GAMES_VIEW_VISIBLE"
            PropertyChanges { target: gameOptionsPanelLayout; width: 0; height: 0; visible: false }
            PropertyChanges { target: questionsCountInput; width: 0; height: 0; visible: false }
            PropertyChanges { target: gamesView; anchors.centerIn:gameOptionsPanel; visible: true }
        },

        /*
         * State when available games are shown.
         * User needs to choose any one of the game
         * from this menu
         */
        State {
            name: "GAMES_VIEW_HIDDEN"
            PropertyChanges { target: gamesView; width: 0; height: 0; visible: false }
            PropertyChanges { target: questionsCountInput; width: 0; height: 0; visible: false }
            PropertyChanges { target: gameOptionsPanelLayout; anchors.centerIn:gameOptionsPanel; visible: true }
        },

        /**
         * The state displays UI to query
         * the user the maximum number of
         * questions he/she wants to attempts
         */
        State {
            name: "QUESTION_COUNT_QUERY"
            PropertyChanges { target: questionsCountInput; width: gameOptionsPanel.width; 
                              height: gameOptionsPanel.height/9; visible: true }
            PropertyChanges { target: gameOptionsPanelLayout; width: 0; height: 0; visible: false }
            PropertyChanges { target: gamesView; width: 0; height: 0; visible: false }
        }
    ]

    /*
     * Let's do some animation !!
     */
    transitions: [
        Transition {
            to: "*"
            NumberAnimation { target: gameOptionsPanelLayout; properties: "height, width"; duration: 150 }
            NumberAnimation { target: gamesView; properties: "height, width"; duration: 150 }
        }
    ]

}
