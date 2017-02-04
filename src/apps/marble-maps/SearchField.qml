//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2015      Mikhail Ivchenko <ematirov@gmail.com>
//

import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    id: root
    height: field.height

    property alias query: field.text
    property alias hasFocus: field.activeFocus

    property alias completionModel: completion.model
    property bool busy: false

    signal searchRequested(string query)
    signal completionRequested(string query)
    signal cleared()
    signal menuButtonClicked()

    function search(query) {
        routingManager.clearSearchResultPlacemarks();
        query = query.trim();
        if(query.toLowerCase() === "ok marble" || query.toLowerCase() === "okdbg") {
            app.state = "developer";
        } else if(query !== "") {
            root.busy = true;
            searchRequested(query);
            field.focus = false;
        }
    }

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        anchors.fill: parent
        color: palette.base
        border.color: palette.shadow
        border.width: 1
    }

    FlatButton {
        id: menuButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
        height: 0.7 * field.height
        width: height
        imageSource: "qrc:///menu.png"

        MouseArea {
            anchors.fill: parent
            onClicked: root.menuButtonClicked()
        }
    }

    TextField {
        id: field
        anchors.left: menuButton.right
        anchors.right: parent.right

        placeholderText: qsTr("Search")
        font.pointSize: 18
        textColor: palette.text
        inputMethodHints: Qt.ImhNoPredictiveText
        onAccepted: root.search(text)
        onTextChanged: root.completionRequested(text)

        BusyIndicator {
            id: searchBusyIndicator
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: clearButton.visible ? clearButton.left : clearButton.right
            anchors.rightMargin: 10
            visible: running
            height: 0.7 * field.height
            width: height
            running: root.busy

            style: BusyIndicatorStyle {
                indicator: Image {
                    visible: control.running
                    source: "busy_indicator.png"
                    RotationAnimator on rotation {
                        running: control.running
                        loops: Animation.Infinite
                        duration: 1500
                        from: 0 ; to: 360
                    }
                }
            }
        }

        FlatButton {
            id: clearButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: searchButton.visible ? searchButton.left : parent.right
            anchors.rightMargin: 10
            height: 0.7 * field.height
            width: height
            visible: field.text !== ""
            imageSource: "qrc:///clear.png"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    app.selectedPlacemark = null;
                    app.state = "none"
                    routingManager.clearSearchResultPlacemarks();
                    field.text = "";
                    field.focus = true;
                    cleared();
                }
            }
        }

        FlatButton {
            id: searchButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
            height: 0.7 * field.height
            width: height
            visible: !root.busy
            enabled: field.text !== ""
            imageSource: "qrc:///search.png"

            onClicked: root.search(field.text)
        }
    }

    Completion {
        id: completion
        anchors {
            top: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: delegateHeight * Math.min(2,count)
        visible: count > 0 && field.activeFocus
        onItemSelected: {
            field.text = name;
            search(name);
        }
    }
}
