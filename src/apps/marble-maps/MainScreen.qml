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
            positionProvider: "QtPositioning"
            showPositionMarker: true

            onPositionVisibleChanged: {
                if ( !positionVisible && positionAvailable ) {
                    zoomToPositionButton.updateIndicator();
                }
            }

            onViewportChanged: {
                if ( !positionVisible && positionAvailable ) {
                    zoomToPositionButton.updateIndicator();
                }
            }

            Routing {
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

    Search {
        id: search
        anchors.fill: parent
        marbleQuickItem: marbleMaps
    }

    PositionButton {
        id: zoomToPositionButton
        anchors {
            bottom: parent.bottom
            right: parent.right
            bottomMargin: Screen.pixelDensity * 9
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
}
