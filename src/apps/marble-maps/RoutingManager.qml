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
        waypointDelegate: WaypointImage {visible: false}

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

    function addPlacemarkAsDestination()
    {
        if ( selectedPlacemark != null ) {
            routing.addViaByPlacemark(selectedPlacemark);
        }

        if (navigationSetup) {
            navigationSetup.visible = false;
        }

        routing.updateRoute();
    }

    function addPlacemarkAsDeparture()
    {
        if ( selectedPlacemark != null ) {
            routing.addViaByPlacemarkAtIndex(0, selectedPlacemark);
        }

        if (navigationSetup) {
            navigationSetup.visible = false;
        }

        routing.updateRoute();
    }

    function addPlacemarkAsWaypoint()
    {
        if ( selectedPlacemark != null ) {
            if ( routing.waypointCount() == 0 ) {
                routing.addViaByPlacemark(selectedPlacemark);
            }
            else {
                routing.addViaByPlacemarkAtIndex(routing.waypointCount() - 1, selectedPlacemark);
            }
        }

        if (navigationSetup) {
            navigationSetup.visible = false;
        }

        routing.updateRoute();
    }

    function addPositionAsDeparture()
    {
        if (marbleItem) {
            routing.addViaByCoordinateAtIndex(0, marbleItem.currentPosition);
        }
    }
}
