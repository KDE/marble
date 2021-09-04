//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.8
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.0 as Kirigami

import org.kde.marble 0.20

Kirigami.ScrollablePage {
    id: optionsPage
    padding: 0
    topPadding: 0
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0

    signal backTriggered()

    Column {
        anchors {
            fill: parent
            margins: Kirigami.Units.gridUnit
        }

        Label {
            text: qsTr("<h3>Layer Options</h3>")
        }

        Label {
            text: qsTr("<h4>Public Transport Layers</h4>")
        }

        Item{
            implicitHeight: publicTransportLoader.height + Kirigami.Units.gridUnit * 4
            width: parent.width

            Loader {
                anchors.fill: parent
                id: publicTransportLoader
                source: "PublicTransport.qml"

                onLoaded: {
                    item.implicitWidth = parent.width
                    item.marbleMaps = marbleMaps
                }
            }
        }

        Label {
            topPadding: Kirigami.Units.gridUnit
            text: qsTr("<h4>Outdoor Activities Layers</h4>")
        }

        Item{
            implicitHeight: outdoorActivitiesLoader.height + Kirigami.Units.gridUnit * 6
            width: parent.width

            Loader {
                anchors.fill: parent
                id: outdoorActivitiesLoader
                source: "OutdoorActivities.qml"

                onLoaded: {
                    item.implicitWidth = parent.width
                    item.marbleMaps = marbleMaps
                }
            }
        }
    }
}
