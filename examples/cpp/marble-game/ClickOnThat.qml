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
    id: clickOnThat
    objectName: "clickOnThat"

    signal nextQuestionRequested()
    signal gameQuitRequested()
    signal answerDisplayRequested()
    signal questionsTimeout()

    property int panelWidth: 200
    property int panelHeight: 600

    width: panelWidth
    height: panelHeight

    property color normalColor: "lightblue"
    property color onHoverColor: "crimson"
    property color borderColor: "transparent"

    // Display the button to show correct Answer
    property bool showCorrectAnswer: false

    // Result Display
    property bool showResult: false
    property string result: qsTr("Undetermined")
    property int score: 0
    property bool scoreDetermined: false
    property int totalQuestionsAsked: 0  // Total no. of questions that user is asked
    property int maximumQuestions: 0

    property bool showQuestion: false
    property string countryName: qsTr("Undetermined")

    Rectangle {
        id: gameDescription
        width: panelWidth
        height: panelHeight/10
        anchors.top: parent.top
        anchors.topMargin: 10

        Rectangle {
            id: gameName
            width: parent.width/2
            height: parent.height
            anchors.left: parent.left
            anchors.leftMargin: 5

            radius: 6
            color: "#e74c3c"

            Text {
                text: qsTr("Click On That Country")
                wrapMode: Text.WordWrap
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
            }
        }

        CustomButton {
            id: quitGameButton
            buttonWidth: parent.width/2
            buttonHeight: parent.height

            labelText: qsTr("Quit Game")
            labelSize: parent.width/15
            labelColor: "white"

            anchors.left: gameName.right
            anchors.leftMargin: 5

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.rightMargin: 5

            onButtonClick: {
                gameQuitRequested();
                resetOptions();
            }
        }
    }

    Rectangle {
        id: question

        anchors.top: gameDescription.bottom
        anchors.topMargin: 30
        width: panelWidth
        height: panelHeight/8
        color: "#2ecc71"

        visible: true

        Text {
            id: questionText
            color: "white"
            width: parent.width
            height: parent.height/2

            text: qsTr("Click on")
        }

        Text {
            id: country
            anchors.top: questionText.bottom
            anchors.topMargin: 5
            width: parent.width
            height: parent.height/2

            font.bold: true
            color: "#d35400"
            text: qsTr(countryName)
        }
    }

    CustomButton {
        id: nextButton
        anchors.top: question.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        buttonWidth: panelWidth*4/5
        buttonHeight: panelHeight/10
        normalColor: "#3498db"

        labelText: qsTr("Next")
        labelColor: "#ffffff";
        onButtonClick: {
            nextQuestionRequested()
        }
    }

    Rectangle {
        id: resultDisplay

        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.top: nextButton.bottom
        anchors.topMargin: 10

        color: "yellow"
        radius: 40

        height: panelHeight*1/6

        visible: showResult

        Text {
            id: text
            color: "yellow"
            anchors.left: parent.left
            anchors.leftMargin: 25
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.verticalCenter: parent.verticalCenter

            wrapMode: Text.WordWrap
            text: qsTr(result)
        }
    }

    CustomButton {
        id: viewCorrectAnswer

        anchors.top: resultDisplay.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20

        buttonHeight: panelHeight*5/(6*14)
        labelColor: "green"
        labelText: qsTr("View Answer")

        visible: showCorrectAnswer

        onButtonClick: {
            answerDisplayRequested();
        }
    }

    Rectangle {
        id: scoreDisplay

        anchors.left: parent.left
        anchors.leftMargin:20
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.top: viewCorrectAnswer.bottom
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        height: panelHeight/10

        color: "darkgrey"

        Text {
            id: scoreContent
            color: "white"
            anchors.centerIn: parent
            text: {
                qsTr("Your Progress \n" + score + "/" + totalQuestionsAsked)
            }
        }
    }

    Timer {
        id: timer
        interval: 700
        repeat: false
        onTriggered: {
            questionsTimeout();
        }
    }

    onQuestionsTimeout: {
        timer.stop();
        showCorrectAnswer = false;
        nextQuestionRequested();
    }

    function initGame() {
        score = 0;
        totalQuestionsAsked = 0;
    }

    function resetOptions() {
        result = "Undetermined";
        showResult = false;
        showQuestion = false;
    }

    function setQuestion( name ) {
        if ( maximumQuestions > 0 ) {
            resetOptions();
            ++totalQuestionsAsked;
            countryName = name;
            scoreDetermined = false;
            showQuestion = true;
        }
        else {
            gameQuitRequested()
        }
        maximumQuestions = maximumQuestions - 1;
    }

    function displayResult( correct ) {
        showResult = true
        if ( correct && !scoreDetermined ) {
            ++score
            scoreDetermined = true;
            result = "<font face=\"verdana\" size=\"4\" color=\"#00ff00\"><b>Hooray !! Right Answer</b></font>"
            timer.start();
        }
        else {
            /**
             * Stop timer ( so that the game
             * doesn't switch to next question
             * automatically ) and let the user
             * choose whether he/she wants
             * to see the correct answer.
             **/
            if ( timer.running == true ) {
                timer.stop();
            }

            result = "<p align=\"center\"> <font face=\"verdana\" size=\"4\" color=\"#ff0000\"><b>Oops, Wrong Answer</b></font> </p>"
            showCorrectAnswer = true;
        }
    }
    
    function setMaximumQuestions( questionsCount ) {
        maximumQuestions = questionsCount;
    }
}
