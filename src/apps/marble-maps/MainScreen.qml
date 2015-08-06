import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import org.kde.edu.marble 0.20

ApplicationWindow {
    id: root
    title: qsTr("Marble Maps")
    visible: true

    menuBar: MenuBar {
        id: menuBar
        Menu {
            title: qsTr("Marble Maps")
            MenuItem{
                text: qsTr("Search")
                onTriggered: search.visible = !search.visible
            }

            MenuItem {
                text: qsTr("Delete Route")
                onTriggered: {routing.clearRoute(); navigationSettings.departureIsSet = false; instructions.visible = false;}
                visible: routing.hasRoute
            }

            MenuItem {
                text: qsTr("Navigation Instructions")
                onTriggered: {instructions.visible = true}
                visible: routing.hasRoute && !instructions.visible
            }

            MenuItem {
                text: qsTr("Show the Map")
                onTriggered: {instructions.visible = false}
                visible: instructions.visible
            }
        }
    }

    toolBar: ToolBar {
        id: toolBar
    }

    width: 600
    height: 400

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: palette.window
    }

    PinchArea {
        anchors.fill: parent
        enabled: true

        onPinchStarted: marbleMaps.handlePinchStarted(pinch.center)
        onPinchFinished: marbleMaps.handlePinchFinished(pinch.center)
        onPinchUpdated: marbleMaps.handlePinchUpdated(pinch.center, pinch.scale);

        MarbleMaps {
            id: marbleMaps

            anchors.fill: parent
            visible: true
            focus: true

            // Theme settings.
            projection: MarbleItem.Mercator
            mapThemeId: "earth/openstreetmap/openstreetmap.dgml"

            // Visibility of layers/plugins.
            showFrameRate: false
            showAtmosphere: false
            showCompass: false
            showClouds: false
            showCrosshairs: false
            showGrid: false
            showOverviewMap: false
            showOtherPlaces: false
            showScaleBar: false
            showBackground: false
            positionProvider: suspended ? "" : "QtPositioning"
            showPositionMarker: true

            onPositionAvailableChanged: updateIndicator()
            onPositionVisibleChanged: updateIndicator()
            onViewportChanged: updateIndicator()

            function updateIndicator() {
                if ( !positionVisible && positionAvailable ) {
                    zoomToPositionButton.updateIndicator();
                }
            }

            Routing {
                id: routing
                anchors.fill: parent
                marbleMap: marbleMaps.marbleMap
            }
        }

        MouseArea{
            anchors.fill: parent
            propagateComposedEvents: true
            onPressed: {
                search.focus = true;
                mouse.accepted = false;
            }
        }
    }

    PositionButton {
        id: zoomToPositionButton
        anchors {
            bottom: parent.bottom
            right: parent.right
            bottomMargin: startNavigationButton.height
            rightMargin: 0.005 * root.width
        }

        iconSource: marbleMaps.positionAvailable ? "qrc:///gps_fixed.png" : "qrc:///gps_not_fixed.png"

        onClicked: marbleMaps.centerOnCurrentPosition()

        property real distance: 0

        function updateIndicator() {
            var point = marbleMaps.mapFromItem(zoomToPositionButton, diameter * 0.5, diameter * 0.5);
            distance = 0.001 * marbleMaps.distanceFromPointToCurrentLocation(point);
            angle = marbleMaps.angleFromPointToCurrentLocation(point);
        }

        showDirection: marbleMaps.positionAvailable && !marbleMaps.positionVisible
    }

    BoxedText {
        id: distanceIndicator
        text: "%1 km".arg(zoomToPositionButton.distance < 10 ? zoomToPositionButton.distance.toFixed(1) : zoomToPositionButton.distance.toFixed(0))
        anchors {
            bottom: zoomToPositionButton.top
            horizontalCenter: zoomToPositionButton.horizontalCenter
        }

        visible: marbleMaps.positionAvailable && !marbleMaps.positionVisible
    }

    Search {
        id: search
        anchors.fill: parent
        marbleQuickItem: marbleMaps
        onNavigationRequested: { startNavigationButton.visible = true }
    }

    CircularButton {
        id: startNavigationButton
        visible: false

        anchors {
            top: zoomToPositionButton.bottom
            horizontalCenter: zoomToPositionButton.horizontalCenter
        }

        iconSource: "qrc:///navigation.png"

        onClicked: { navigationSettings.visible = true; startNavigationButton.visible = false; }
    }

    NavigationSetup {
        id: navigationSettings
        visible: false
        property bool departureIsSet: false
        property bool destinationIsSet: false

        height: Screen.pixelDensity * 9
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        onRouteToDestinationRequested: {
            if (!departureIsSet || (departureIsSet && destinationIsSet)) {
                routing.clearRoute();
                if (marbleMaps.positionAvailable) {
                    departureIsSet = true;
                    routing.addVia(marbleMaps.currentPosition);
                }
            }

            if (departureIsSet) {
                routing.addVia(search.searchResultCoordinates);
                routing.updateRoute();
                destinationIsSet = true;
            }
        }

        onRouteThroughWaypointRequested: {
            if (!departureIsSet || (departureIsSet && destinationIsSet) ) {
                routing.clearRoute();
                if (marbleMaps.positionAvailable) {
                    departureIsSet = true;
                    routing.addVia(marbleMaps.currentPosition);
                }
                departureIsSet = true;
                destinationIsSet = false;
            }

            routing.addVia(search.searchResultCoordinates);
            routing.updateRoute();

            visible = false;
        }

        onRouteFromDepartureRequested: {
            visible = false;
            routing.clearRoute();
            routing.addVia(search.searchResultCoordinates);
            departureIsSet = true;
            destinationIsSet = false;
        }

        onProfileSelected: {
            visible = false;
            routing.routingProfile = profile;
            routing.updateRoute();
        }
    }

    RoutePlanViewer{
        id: instructions
        visible: false
        anchors.fill: parent
        model: routing.routingModel
    }
}
