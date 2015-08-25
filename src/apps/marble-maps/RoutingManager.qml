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

import org.kde.edu.marble 0.20

Item {
    id: root

    property var marbleItem: null
    property alias routingProfile: routing.routingProfile
    property alias hasRoute: routing.hasRoute
    property alias routingModel: routing.routingModel
    property alias routeRequestModel: routing.routeRequestModel
    property var navigationSetup: null
    property var selectedPlacemark: null
    property string profileIcon: "qrc:///car.png"

    function removeVia(index)
    {
        routing.removeVia(index);
    }

    function waypointCount()
    {
        return routing.waypointCount();
    }

    function swapVias(index1, index2)
    {
        routing.swapVias(index1, index2);
    }

    function clearRoute()
    {
        routing.clearRoute();
    }


    onNavigationSetupChanged: {
        if (navigationSetup != null) {
            navigationSetup.routeToDestinationRequested.connect(addPlacemarkAsDestination);
            navigationSetup.routeFromDepartureRequested.connect(addPlacemarkAsDeparture);
            navigationSetup.routeThroughWaypointRequested.connect(addPlacemarkAsWaypoint);
        }
    }

    Routing {
        id: routing

        anchors.fill: parent
        marbleMap: marbleItem.marbleMap
        waypointDelegate: Waypoint {visible: false}
        onRoutingProfileChanged: {
            switch (routingProfile) {
            case "Car (fastest)":
                root.profileIcon = "qrc:///car.png";
                break;
            case "Bicycle":
                root.profileIcon = "qrc:///bike.png";
                break;
            case "Pedestrian":
                root.profileIcon = "qrc:///walk.png";
                break;
            }
            updateRoute();
        }
    }

    function addSearchResultAsPlacemark(placemark)
    {
        if (marbleItem) {
            var index = routing.addSearchResultPlacemark(placemark);
            routing.showMenuOfSearchResult(index);
        }
    }
}
