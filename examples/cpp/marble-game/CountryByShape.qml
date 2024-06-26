// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//


import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {

    id: countryByShape
    objectName: "countryByShape"

    color: "#D6ADFF"

    signal nextQuestionRequested()
    signal gameQuitRequested()
    signal questionsTimeout()

    property int panelWidth: 200
    property int panelHeight: 600

    width: panelWidth
    height: panelHeight

    // No. of radio buttons in Column element
    property real nItemsInColumn: 4

    // Button Width and Height
    property real labelWidth: countryByShape.panelWidth*4/5
    property real labelHeight: countryByShape.panelHeight/16

    // Result Display
    property bool showResult: false

    // Display the button to show correct Answer
    property bool showCorrectAnswer: false
    property string answerToShow: qsTr("View Answer")

    property string result: qsTr("Undetermined")
    property int score: 0
    property bool scoreDetermined: false
    property int totalQuestionsAsked: 0  // Total no. of questions that have been asked to user
    property int maximumQuestions: 0

    //property string userAnswer: "Undetermined"
    property string correctAnswer: qsTr("Undetermined")

    Rectangle {
        id: gameDescription
        height: panelHeight*2/( 3 * ( nItemsInColumn + 2 ) )
        width: panelWidth
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        color: "#D6ADFF"

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

                text: qsTr("Identify The Highlighted Country")
            }
        }

        CustomButton {
            id: quitGameButton
            buttonWidth: parent.width/2
            buttonHeight: parent.height

            normalColor: "#696969"
            borderColor: "#000000"

            labelColor: "white"
            labelText: qsTr("Quit Game")
            labelSize: parent.width/15

            anchors.left: gameName.right
            anchors.leftMargin: 5

            anchors.top: parent.top
            anchors.right: parent.right
            anchors.rightMargin: 5

            onButtonClick: {
                resetOptions();
                gameQuitRequested();
            }
        }
    }

    Column {
        id: buttonArea
        anchors.top: gameDescription.bottom
        anchors.topMargin: 20
        spacing: 5

        ExclusiveGroup { id: group }
        CustomRadioButton {
            id: answerOption1
            radioButtonWidth: labelWidth
            radioButtonHeight: labelHeight

            normalColor: "#A9A9A9"
            labelColor: "white"

            labelText: qsTr("Option1")
            buttonGroup: group

            onRadioButtonClick: {
                showResult = true;
                if ( labelText == correctAnswer ) {
                    if ( timer.running == false ) {
                        timer.start();
                    }

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
                    showCorrectAnswer = true
                }
            }
        }

        CustomRadioButton {
            id: answerOption2
            radioButtonWidth: labelWidth
            radioButtonHeight: labelHeight

            normalColor: "#A9A9A9"
            labelColor: "white"

            labelText: qsTr("Option2")
            buttonGroup: group

            onRadioButtonClick: {
                showResult = true;
                if ( labelText == correctAnswer ) {
                    if ( timer.running == false ) {
                        timer.start();
                    }

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
                    showCorrectAnswer = true
                }
            }
        }

        CustomRadioButton {
            id: answerOption3
            radioButtonWidth: labelWidth
            radioButtonHeight: labelHeight

            normalColor: "#A9A9A9"
            labelColor: "white"

            labelText: qsTr("Option3")
            buttonGroup: group

            onRadioButtonClick: {
                showResult = true;
                if ( labelText == correctAnswer ) {
                    if ( timer.running == false ) {
                        timer.start();
                    }

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
                    showCorrectAnswer = true
                }
            }
        }

        CustomRadioButton {
            id: answerOption4
            radioButtonWidth: labelWidth
            radioButtonHeight: labelHeight

            normalColor: "#A9A9A9"
            labelColor: "white"

            labelText: qsTr("Option4")
            buttonGroup: group

            onRadioButtonClick: {
                showResult = true;
                if ( labelText == correctAnswer ) {
                    if ( timer.running == false ) {
                        timer.start();
                    }

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
                    showCorrectAnswer = true
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
        buttonHeight: labelHeight

        normalColor: "#696969"
        borderColor: "#000000"

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
        anchors.topMargin: 15

        height: panelHeight/6

        color: "#80FFFF"
        border.width: 1
        border.color: "#696969"
        radius: 40

        visible: showResult

        Text {
            id: resultContent

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
         anchors.topMargin: 15
         anchors.left: parent.left
         anchors.leftMargin: 20
         anchors.right: parent.right
         anchors.rightMargin: 20

         buttonHeight: labelHeight
         borderColor: "#696969"
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
        anchors.leftMargin:10
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: viewCorrectAnswer.bottom
        anchors.topMargin: 20

        height: 2*labelHeight

        border.width: 1
        border.color: "#696969"
        color: "darkgrey"

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
            buttonArea.children[i].labelText = qsTr("NO_OPTION")
            result = "Undetermined";
            correctAnswer = "Undetermined";
            showResult = false;

            showCorrectAnswer = false;
            answerToShow = "View Answer";
        }
    }

    function setQuestion( answerOptions, rightAnswer ) {
        if ( maximumQuestions > 0 ) {
            resetOptions();
            ++totalQuestionsAsked;
            scoreDetermined = false;
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
