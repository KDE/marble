// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11

Item {
    id: root
    z: 10
    height: content.height
    property string searchTerm: ""

    function calculateRoute() {
        marbleWidget.routing.routingProfile = routingTypeOptions.routingType
        marbleWidget.routing.updateRoute()
    }

    Column {
        id: content
        width: parent.width
        height: routingTypeOptions.height + routeActions.height + listView.height + 10
        anchors.margins: 5
        spacing: 5

        ListView {
            id: listView
            width: parent.width
            height: 40 * count
            interactive: false
            model: marbleWidget.routing.routeRequestModel()
            delegate: ViaPointEditor {
                id: sourcePoint
                width: content.width
                height: 40

                Component.onCompleted: marbleWidget.mouseClickGeoPosition.connect(retrieveInput)
                onPositionChanged: root.calculateRoute()
            }
        }

        Row {
            id: routeActions
            width: parent.width
            Button {
                text: "Add"
                width: parent.width / 3 - 5
                /** @todo: Ask user instead to click on a point? */
                onClicked: marbleWidget.routing.addVia(marbleWidget.getCenter().longitude, marbleWidget.getCenter().latitude)
            }

            Button {
                text: "Reverse"
                width: parent.width / 3 - 5
                onClicked: marbleWidget.routing.reverseRoute()
            }

            Button {
                text: "Clear"
                width: parent.width / 3 - 5
                onClicked: marbleWidget.routing.clearRoute()
            }
        }

        ButtonRow {
            width: parent.width - 20
            id: routingTypeOptions
            checkedButton: routingMotorcarButton
            property string routingType: checkedButton.routingType
            Button {
                id: routingMotorcarButton
                iconSource: "qrc:/icons/routing-motorcar.svg"
                property string routingType: "Motorcar"
            }

            Button {
                id: routingBikeButton
                iconSource: "qrc:/icons/routing-bike.svg"
                property string routingType: "Bicycle"
            }

            Button {
                id: routingPedestrianButton
                iconSource: "qrc:/icons/routing-pedestrian.svg"
                property string routingType: "Pedestrian"
            }
        }
    }

    Connections { target: routingTypeOptions; onRoutingTypeChanged: root.calculateRoute() }

    Component.onCompleted: {
        if (marbleWidget.routing.routeRequestModel().count === 0) {
            marbleWidget.routing.addVia(marbleWidget.tracking.lastKnownPosition.longitude, marbleWidget.tracking.lastKnownPosition.latitude)
        }
    }
}
