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

    color: "#C2D1B2"

    signal nextQuestionRequested()
    signal gameQuitRequested()
    signal answerDisplayRequested()
    signal questionsTimeout()

    property int panelWidth: 200
    property int panelHeight: 600

    width: panelWidth
    height: panelHeight

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

        color: "#C2D1B2"

        Rectangle {
            id: gameName
            width: parent.width/2
            height: parent.height
            anchors.left: parent.left
            anchors.leftMargin: 5
            border.width: 1
            border.color: "#000000"

            radius: 6
            smooth: true
            color: "#696969"

            Text {
                width: parent.width
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter
                color: "white"

                text: qsTr("Click On That Country")
            }
        }

        CustomButton {
            id: quitGameButton
            buttonWidth: parent.width/2
            buttonHeight: parent.height

            normalColor: "#696969"
            borderColor: "#000000"

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
        anchors.topMargin: parent.height/17
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        height: panelHeight/8
        color: "#A9A9A9"
        radius: width*0.5
        smooth: true
        border.width: 1
        border.color: "#696969"

        visible: true

        Text {
            id: questionText
            color: "white"
            width: parent.width
            height: parent.height/2
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: parent.height/5
            text: qsTr("Click on")
        }

        Text {
            id: country
            anchors.top: questionText.bottom
            anchors.topMargin: 5
            width: parent.width
            height: parent.height/2
            horizontalAlignment: Text.AlignHCenter

            font.pixelSize: parent.height/5
            font.bold: true
            color: "white"
            text: qsTr(countryName)
        }
    }

    CustomButton {
        id: nextButton
        anchors.top: question.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        buttonWidth: panelWidth*4/5
        buttonHeight: panelHeight/14
        normalColor: "#696969"
        borderColor: "#000000"

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
        anchors.topMargin: 20

        color: "#80FFFF"
        border.width: 1
        border.color: "#262626"
        radius: 40
        smooth: true

        height: panelHeight/6

        visible: showResult

        Text {
            id: text
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
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10

        buttonHeight: panelHeight*5/(6*14)
        borderColor: "#696969"
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
        anchors.leftMargin:10
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: viewCorrectAnswer.bottom
        anchors.topMargin: 20
        border.width: 1
        border.color: "#696969"

        height: panelHeight/8

        color: "#808080"

        Text {
            id: scoreContent
            color: "white"
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 18
            text: {
                qsTr("Your Progress \n\n" + score + "/" + totalQuestionsAsked)
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
