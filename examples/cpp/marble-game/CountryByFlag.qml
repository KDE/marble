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
    id: countryByFlag
    objectName: "countryByFlag"

    signal nextQuestionRequested()
    signal gameQuitRequested()
    signal questionsTimeout()

    property int panelWidth: 200
    property int panelHeight: 600

    width: panelWidth
    height: panelHeight

    property color normalColor: "lightblue"
    property color onHoverColor: "crimson"
    property color borderColor: "transparent"

    // No. of radio buttons in Column element
    property real nItemsInColumn: 4

    // Button Width and Height
    property real labelWidth: countryByFlag.panelWidth*4/5
    property real labelHeight: countryByFlag.panelHeight/16

    // Result Display
    property bool showResult: false

    // Display the button to show correct Answer
    property bool showCorrectAnswer: false
    property string answerToShow: qsTr("View Answer")

    property string result: qsTr("Undetermined")
    property int score: 0
    property bool scoreDetermined: false
    property int totalQuestionsAsked: 0  // Total no. of questions that user is asked
    property int maximumQuestions: 0

    //property string userAnswer: "Undetermined"
    property string correctAnswer: qsTr("Undetermined")

    property string flagPath: ""

    Rectangle {
        id: gameDescription
        width: panelWidth
        height: panelHeight/12
        anchors.top: parent.top
        anchors.topMargin: 5

        Rectangle {
            id: gameName
            width: parent.width/2
            height: parent.height
            anchors.left: parent.left
            anchors.leftMargin: 5

            radius: 6
            color: "#e74c3c"

            Text {
                text: qsTr("Identify the flag")
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

            labelColor: "white"
            labelSize: parent.width/15
            labelText: qsTr("Quit Game")

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

    Image {
        id: flag

        width: panelWidth
        height: panelHeight/7
        anchors.top: gameDescription.bottom
        anchors.topMargin: 10
        source: flagPath
        fillMode: Image.PreserveAspectFit
    }

    Column {
        id: buttonArea
        anchors.top: flag.bottom
        anchors.topMargin: 10
        spacing: 5

        ExclusiveGroup { id: group }
        CustomRadioButton {
            id: answerOption1
            radioButtonWidth: labelWidth
            radioButtonHeight: labelHeight

            normalColor: "#e67e22"
            labelColor: "white"

            labelText: qsTr("Option1")
            buttonGroup: group

            onRadioButtonClick: {
                if ( timer.running == false ) {
                    timer.start();
                }
                showResult = true;
                if ( labelText == correctAnswer ) {
                    result = "<font face=\"verdana\" size=\"4\" color=\"#00ff00\"><b>Hooray !! Right Answer</b></font>";
                    if ( !scoreDetermined &&
                        !showCorrectAnswer )
                    {
                        ++score;
                        scoreDetermined = true;
                    }
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

                    result = "<p align=\"center\"> <font face=\"verdana\" size=\"4\" color=\"#ff0000\"><b>Oops, Wrong Answer</b></font> </p>";
                    showCorrectAnswer = true;
                }
            }
        }

        CustomRadioButton {
            id: answerOption2
            radioButtonWidth: labelWidth
            radioButtonHeight: labelHeight

            normalColor: "#e67e22"
            labelColor: "white"

            labelText: qsTr("Option2")
            buttonGroup: group

            onRadioButtonClick: {
                if ( timer.running == false ) {
                    timer.start();
                }
                showResult = true;
                if ( labelText == correctAnswer ) {
                    result = "<font face=\"verdana\" size=\"4\" color=\"#00ff00\"><b>Hooray !! Right Answer</b></font>";
                    if ( !scoreDetermined &&
                        !showCorrectAnswer )
                    {
                        ++score;
                        scoreDetermined = true;
                    }
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

                    result = "<p align=\"center\"> <font face=\"verdana\" size=\"4\" color=\"#ff0000\"><b>Oops, Wrong Answer</b></font> </p>";
                    showCorrectAnswer = true;
                }
            }
        }

        CustomRadioButton {
            id: answerOption3
            radioButtonWidth: labelWidth
            radioButtonHeight: labelHeight

            normalColor: "#e67e22"
            labelColor: "white"

            labelText: qsTr("Option3")
            buttonGroup: group

            onRadioButtonClick: {
                if ( timer.running == false ) {
                    timer.start();
                }
                showResult = true;
                if ( labelText == correctAnswer ) {
                    result = "<font face=\"verdana\" size=\"4\" color=\"#00ff00\"><b>Hooray !! Right Answer</b></font>";
                    if ( !scoreDetermined &&
                        !showCorrectAnswer )
                    {
                        ++score;
                        scoreDetermined = true;
                    }
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

                    result = "<p align=\"center\"> <font face=\"verdana\" size=\"4\" color=\"#ff0000\"><b>Oops, Wrong Answer</b></font> </p>";
                    showCorrectAnswer = true;
                }
            }
        }

        CustomRadioButton {
            id: answerOption4
            radioButtonWidth: labelWidth
            radioButtonHeight: labelHeight

            normalColor: "#e67e22"
            labelColor: "white"

            labelText: qsTr("Option4")
            buttonGroup: group

            onRadioButtonClick: {
                if ( timer.running == false ) {
                    timer.start();
                }
                showResult = true;
                if ( labelText == correctAnswer ) {
                    result = "<font face=\"verdana\" size=\"4\" color=\"#00ff00\"><b>Hooray !! Right Answer</b></font>";
                    if ( !scoreDetermined &&
                        !showCorrectAnswer )
                    {
                        ++score;
                        scoreDetermined = true;
                    }
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

                    result = "<p align=\"center\"> <font face=\"verdana\" size=\"4\" color=\"#ff0000\"><b>Oops, Wrong Answer</b></font> </p>";
                    showCorrectAnswer = true;
                }
            }
        }
    }

    CustomButton {
        id: nextButton
        anchors.top: buttonArea.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter
        buttonWidth: labelWidth
        buttonHeight: labelHeight*3/4
        normalColor: "#3498db"

        labelText: qsTr("Next")
        labelColor: "white"

        onButtonClick: {
            showCorrectAnswer = false;
            answerToShow = qsTr("View Answer");
            nextQuestionRequested();
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

        height: panelHeight/6

        color: "yellow"
        radius: 40

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
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20

        buttonHeight: labelHeight*5/6
        labelColor: "green"
        labelText: answerToShow

        visible: showCorrectAnswer

        onButtonClick: {
            answerToShow = correctAnswer;
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
        anchors.bottomMargin: 10

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
        answerToShow = qsTr("View Answer");
        nextQuestionRequested();
    }

    function initGame() {
        score = 0;
        totalQuestionsAsked = 0;
    }

    function resetOptions() {
        for ( var i = 0; i < buttonArea.children.length; ++i ) {
            if ( buttonArea.children[i].checked == true ) {
                buttonArea.children[i].checked = false;
            }
            buttonArea.children[i].labelText=qsTr("NO_OPTION")
            result = "Undetermined";
            correctAnswer = "Undetermined";
            showResult = false;
        }
        flagPath = "";
    }

    function setQuestion( answerOptions, imageSource, rightAnswer ) {
        if(maximumQuestions > 0){
            resetOptions();
            ++totalQuestionsAsked;
            scoreDetermined = false;
            flagPath = imageSource;
            correctAnswer = rightAnswer;
            for ( var i = 0; i < buttonArea.children.length; ++i ) {
                buttonArea.children[i].labelText = answerOptions[i];
            }
        }
        else {
            gameQuitRequested()
        }
        maximumQuestions = maximumQuestions - 1;
    }
    function setMaximumQuestions( questionsCount ) {
        maximumQuestions = questionsCount;
    }
    
}  
