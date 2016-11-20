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
import QtQuick.Window 2.2

import org.kde.marble 0.20

ApplicationWindow {
    id: root
    title: qsTr("Marble Maps")
    visible: true

    width: 600
    height: 400

    color: palette.window

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Item {
        id: mapItem
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: dialogContainer.top
        }

        PinchArea {
            anchors.fill: parent
            enabled: true

            onPinchStarted: marbleMaps.handlePinchStarted(pinch.center)
            onPinchFinished: marbleMaps.handlePinchFinished(pinch.center)
            onPinchUpdated: marbleMaps.handlePinchUpdated(pinch.center, pinch.scale);

            MarbleMaps {
                id: marbleMaps

                property string currentPositionProvider: "QtPositioning"
                property bool wlanOnly: false
                property bool smallZoom : radius < 2 * Math.max(root.width, root.height)

                anchors.fill: parent

                visible: true

                // Theme settings.
                projection: smallZoom ? MarbleItem.Spherical : MarbleItem.Mercator
                mapThemeId: "earth/vectorosm/vectorosm.dgml"

                // Visibility of layers/plugins.
                showFrameRate: false
                showAtmosphere: smallZoom
                showCompass: false
                showClouds: false
                showCrosshairs: false
                showGrid: smallZoom
                showOverviewMap: false
                showOtherPlaces: false
                showScaleBar: false
                showBackground: smallZoom
                positionProvider: suspended ? "" : currentPositionProvider
                keepScreenOn: !suspended && navigationManager.visible
                showPositionMarker: false

                placemarkDelegate: Image {
                    property int xPos: 0
                    property int yPos: 0
                    property var placemark: null
                    x: xPos - 0.5 * width
                    y: yPos - height
                    width: Screen.pixelDensity*6
                    height: width
                    source: "qrc:///ic_place.png"
                    onPlacemarkChanged: {
                        placemarkDialog.placemark = placemark
                    }
                }

                onPositionAvailableChanged: {
                    updateIndicator();
                }
                onPositionVisibleChanged: {
                    updateIndicator();
                }
                onVisibleLatLonAltBoxChanged: {
                    !panningDetectionTimer.restart();
                    updateIndicator();
                }
                onCurrentPositionChanged: {
                    updateIndicator();
                }

                onZoomChanged: {
                    zoomDetectionTimer.restart()
                }

                Component.onCompleted: {
                    setPluginSetting("coordinate-grid", "gridColor", "#999999");
                    setPluginSetting("coordinate-grid", "tropicsColor", "#888888");
                    setPluginSetting("coordinate-grid", "equatorColor", "#777777");
                    setPluginSetting("coordinate-grid", "primaryLabels", "false");
                    setPluginSetting("coordinate-grid", "secondaryLabels", "false");
                    marbleMaps.loadSettings()
                }
                Component.onDestruction: marbleMaps.writeSettings()

                function updateIndicator() {
                    if ( !positionVisible && positionAvailable ) {
                        zoomToPositionButton.updateIndicator();
                    }
                }

                RoutingManager {
                    id: routing
                    anchors.fill: parent
                    marbleItem: marbleMaps
                    routingProfile: routeEditor.routingProfile
                    visible: hasRoute
                }

                Timer {
                    id: zoomDetectionTimer
                    interval: 1000
                }
                Timer {
                    id: panningDetectionTimer
                    interval: 1000
                }

                PositionMarker {
                    id: positionMarker
                    x: navigationManager.snappedPositionMarkerScreenPosition.x
                    y: navigationManager.snappedPositionMarkerScreenPosition.y
                    angle: marbleMaps.angle
                    visible: marbleMaps.positionAvailable
                             && x + radius > 0 && x - radius < marbleMaps.width
                             && y + radius > 0 && y - radius < marbleMaps.height
                    radius: navigationManager.screenAccuracy / 2
                    showAccuracy: navigationManager.deviated
                    allowRadiusAnimation: !zoomDetectionTimer.running
                    allowPositionAnimation: !panningDetectionTimer.running
                    speed: marbleMaps.speed
                }

                MouseArea {
                    anchors.fill: parent
                    propagateComposedEvents: true
                    onPressed: {
                        marbleMaps.focus = true;
                        mouse.accepted = false;
                    }
                }

                Search {
                    id: search
                    anchors.fill: parent
                    marbleQuickItem: marbleMaps
                    routingManager: routing
                    visible: !navigationManager.visible
                }
            }

            NavigationManager {
                id: navigationManager
                width: parent.width
                height: parent.height
                visible: false
                marbleItem: marbleMaps
                hasRoute: routing.hasRoute
            }
        }

        BoxedText {
            id: distanceIndicator
            text: qsTr("%1 km").arg(zoomToPositionButton.distance < 10 ? zoomToPositionButton.distance.toFixed(1) : zoomToPositionButton.distance.toFixed(0))
            anchors {
                bottom: zoomToPositionButton.top
                horizontalCenter: zoomToPositionButton.horizontalCenter
            }

            visible: marbleMaps.positionAvailable && !marbleMaps.positionVisible
        }

        PositionButton {
            id: zoomToPositionButton
            anchors {
                right: parent.right
                rightMargin: Screen.pixelDensity * 1
                bottom: routeEditorButton.top
                bottomMargin: 10
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

        CircularButton {
            id: routeEditorButton
            anchors {
                bottom: parent.bottom
                horizontalCenter: zoomToPositionButton.horizontalCenter
                bottomMargin: Screen.pixelDensity * 4
            }

            onClicked: {
                if (dialogContainer.currentIndex === dialogContainer.routing) {
                    dialogContainer.currentIndex = dialogContainer.none
                    navigationManager.visible = true
                } else if (dialogContainer.currentIndex === dialogContainer.place) {
                    dialogContainer.currentIndex = dialogContainer.routing
                    placemarkDialog.addToRoute()
                } else {
                    dialogContainer.currentIndex = dialogContainer.routing
                    navigationManager.visible = false
                }
            }
            iconSource: "qrc:///material/directions.svg";

            states: [
                State {
                    name: ""
                    PropertyChanges { target: routeEditorButton; iconSource: "qrc:///material/directions.svg"; }
                },
                State {
                    name: "routingAction"
                    when: dialogContainer.currentIndex === dialogContainer.routing
                    PropertyChanges { target: routeEditorButton; iconSource: "qrc:///material/navigation.svg"; }
                },
                State {
                    name: "placeAction"
                    when: dialogContainer.currentIndex === dialogContainer.place
                    PropertyChanges { target: routeEditorButton; iconSource: placemarkDialog.actionIconSource }
                }
            ]
        }
    }

    Item {
        id: dialogContainer
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        visible: currentIndex >= 0

        property var contentItem: routeEditor

        height: visible ? contentItem.height : 0

        readonly property int none: -1
        readonly property int routing: 0
        readonly property int place: 1
        readonly property int about: 2
        readonly property int settings: 3
        readonly property int developer: 4

        property int currentIndex: none

        onCurrentIndexChanged:
        {
            switch (currentIndex) {
            case none:
            case routing: contentItem = routeEditor; break;
            case place: contentItem = placemarkDialog; break;
            case about: contentItem = aboutDialog; break;
            case settings: contentItem = settingsDialog; break;
            case developer: contentItem = developerDialog; break;
            }
        }

        RouteEditor {
            id: routeEditor
            visible: dialogContainer.currentIndex === dialogContainer.routing
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
        PlacemarkDialog {
            id: placemarkDialog
            visible: dialogContainer.currentIndex === dialogContainer.place
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            map: marbleMaps
        }
        AboutDialog {
            id: aboutDialog
            visible: dialogContainer.currentIndex === dialogContainer.about
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
        SettingsDialog {
            id: settingsDialog
            visible: dialogContainer.currentIndex === dialogContainer.settings
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
        DeveloperDialog {
            id: developerDialog
            visible: dialogContainer.currentIndex === dialogContainer.developer
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }

    BorderImage {
        visible: dialogContainer.visible
        anchors.fill: dialogContainer
        anchors.margins: -14
        border { top: 14; left: 14; right: 14; bottom: 14 }
        source: "qrc:///border_shadow.png"
    }

    BoxedText {
        id: quitHelper
        visible: false
        text: qsTr("Press again to close.")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Screen.pixelDensity * 5
        anchors.horizontalCenter: parent.horizontalCenter
        onVisibleChanged: {
            if (visible) {
                quitTimer.restart()
            }
        }

        Timer {
            id: quitTimer
            interval: 3000;
            running: false;
            repeat: false
            onTriggered: {
                root.aboutToQuit = false
                quitHelper.visible = false
            }
        }
    }

    property bool aboutToQuit: false

    onClosing: {
        if (root.aboutToQuit === true) {
            close.accepted = true // we will quit
            return
        } else if (navigationManager.visible) {
            navigationManager.visible = false
        } else if (dialogContainer.visible) {
            dialogContainer.currentIndex = dialogContainer.none
        } else {
            root.aboutToQuit = true
            quitHelper.visible = true
        }
        close.accepted = false
    }
}
