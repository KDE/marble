//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//


import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2

Item {
    id: root

    signal routeToDestinationRequested()
    signal routeFromDepartureRequested()
    signal routeThroughWaypointRequested()
    signal profileSelected(string profile)

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: palette.base
        border {
            width: Screen.pixelDensity * 0.2
            color: palette.shadow
        }

        RowLayout {
            id: typeSelector
            anchors.fill: parent
            spacing: 0
            visible: true

            NavigationSetupButton {
                imageSource: "qrc:///navigation.png"
                text: qsTr("As destination")
                onClicked: { routeToDestinationRequested();  }
            }

            NavigationSetupButton {
                imageSource: "qrc:///waypoint.png"
                text: qsTr("As waypoint")
                onClicked: { routeThroughWaypointRequested();  }
            }

            NavigationSetupButton {
                imageSource: "qrc:///map.png"
                text: qsTr("As departure")
                onClicked: { routeFromDepartureRequested();  }
            }
        }
    }
}
