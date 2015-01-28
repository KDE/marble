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
    id: gamesDisplayPanel
    objectName: "gamesDisplayPanel"

    signal quitGame()
    signal requestNextQuestion()
    signal requestAnswerDisplay()

    property int gamesDisplayWidth: 200
    property int gamesDisplayHeight: 600
    property alias currentIndex: gameView.currentIndex

    width: gamesDisplayWidth
    height: gamesDisplayHeight

    /*
     * The Model contains data for
     * UI for different types of available
     * games. Whenever user opts for a
     * particular game we switch the view
     * to that game. View is controlled by
     * ListView element defined below
     */
    VisualItemModel {
        id: gamesModel

        CountryByShape {
            id: countryByShapeGame
            panelWidth: gamesDisplayPanel.width
            panelHeight: gamesDisplayPanel.height
            onGameQuitRequested: {
                quitGame();
            }
            onNextQuestionRequested: {
                requestNextQuestion();
            }
        }

        CountryByFlag {
            id: countryByFlagGame
            panelWidth: gamesDisplayPanel.width
            panelHeight: gamesDisplayPanel.height
            onGameQuitRequested: {
                quitGame();
            }
            onNextQuestionRequested: {
                requestNextQuestion();
            }
        }

        ClickOnThat {
            id: clickOnThat
            panelWidth: gamesDisplayPanel.width
            panelHeight: gamesDisplayPanel.height
            onGameQuitRequested: {
                quitGame();
            }
            onNextQuestionRequested: {
                requestNextQuestion();
            }
            onAnswerDisplayRequested: {
                requestAnswerDisplay();
            }
        }
    }

    /*
     * View to display game model
     */
    ListView {
        id: gameView
        objectName: "gameView"
        width: gamesDisplayPanel.width
        height: gamesDisplayPanel.height
        anchors.centerIn: parent

        model: gamesModel
        highlightMoveDuration: 250
        highlightRangeMode: ListView.StrictlyEnforceRange

        orientation: ListView.Vertical
    }

    /*
     * These are functions which tell a
     * particular type of game to display
     * the question data
     */
    function postCountryShapeQuestion( answerOptions, correctAnswer ) {
        gameView.currentItem.setQuestion( answerOptions, correctAnswer );
    }

    function postCountryFlagQuestion( answerOptions, imageSource, correctAnswer ) {
        gameView.currentItem.setQuestion( answerOptions, imageSource, correctAnswer );
    }

    function postClickOnThatQuestion( countryName ) {
        gameView.currentItem.setQuestion( countryName );
    }

    function initGame() {
        gameView.currentItem.initGame()
    }

    function displayResult( result ) {
        gameView.currentItem.displayResult( result )
    }

    function setMaximumQuestionsCounts( questionsCount ) {
        gameView.currentItem.setMaximumQuestions( questionsCount );
    }
}
