// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2015 Mikhail Ivchenko <ematirov@gmail.com>
//

import QtQuick
import QtQuick.Controls

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

    ToolButton {
        id: menuButton
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
        height: 0.7 * field.height
        width: height
        icon.name: 'application-menu-symbolic'

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
        font.pointSize: 16
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
        }

        ToolButton {
            id: clearButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: searchButton.visible ? searchButton.left : parent.right
            anchors.rightMargin: 10
            height: 0.7 * field.height
            width: height
            visible: field.text !== ""
            icon.name: 'edit-clear-list-symbolic'

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

        ToolButton {
            id: searchButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10
            height: 0.7 * field.height
            width: height
            visible: !root.busy
            enabled: field.text !== ""
            icon.name: 'search-symbolic'
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
