// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <nienhueser@kde.org>

import QtQuick 2.3
import org.kde.edu.marble 0.20
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    id: root
    z: 10
    height: content.height
    property string searchTerm: ""

    function calculateRoute() {
        marbleWidget.routing.routingProfile = routingTypeOptions.routingType
        marbleWidget.routing.updateRoute()
    }

    RouteRequestModel {
        id: routeRequestModel
        routing: marbleWidget.routing;
    }

    Column {
        id: content
        width: parent.width
        anchors.margins: 5
        spacing: 5

        Repeater {
            id: listView
            width: parent.width
            model: routeRequestModel

            ViaPointEditor {
                id: sourcePoint
                width: content.width

                Component.onCompleted: marbleWidget.mouseClickGeoPosition.connect(retrieveInput)
                onPositionChanged: {
                    routeRequestModel.setPosition(index, lon, lat)
                    root.calculateRoute()
                }
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

        RowLayout {
            id: routingTypeOptions
            width: parent.width - 20
            ExclusiveGroup { id: routingGroup }
            property string routingType: checkedButton.routingType
            RadioButton {
                id: routingMotorcarButton
                exclusiveGroup: routingGroup
                checked: true
                //iconSource: "qrc:/icons/routing-motorcar.svg"
                text: "Car"
                property string routingType: "Motorcar"
            }

            RadioButton {
                id: routingBikeButton
                exclusiveGroup: routingGroup
                //iconSource: "qrc:/icons/routing-bike.svg"
                text: "Bike"
                property string routingType: "Bicycle"
            }

            RadioButton {
                id: routingPedestrianButton
                exclusiveGroup: routingGroup
                //iconSource: "qrc:/icons/routing-pedestrian.svg"
                text: "Foot"
                property string routingType: "Pedestrian"
            }
        }
    }

    Connections { target: routingTypeOptions; onRoutingTypeChanged: root.calculateRoute() }

    Component.onCompleted: {
        if (routeRequestModel.count === 0) {
            marbleWidget.routing.addVia(marbleWidget.tracking.lastKnownPosition.longitude, marbleWidget.tracking.lastKnownPosition.latitude)
        }
    }
}
