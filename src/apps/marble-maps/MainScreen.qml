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
import QtQuick.Controls 2.0
import QtQuick.Window 2.2

import org.kde.marble 0.20

ApplicationWindow {
    id: app
    title: qsTr("Marble Maps")
    visible: true

    width: 600
    height: 400

    color: "#f9f9f9" // Keep the background white while no dialog is loaded

    property alias state: stateTracker.state

    property var selectedPlacemark
    property bool showOsmTags: false
    property int currentWaypointIndex: 0

    property real animatedMargin: app.state === "none" ? 0 : -dialogLoader.height
    property bool dialogExpanded: animatedMargin === -dialogLoader.height
    property real mapOffset: !dialogExpanded ? animatedMargin / 2 : 0

    Behavior on animatedMargin {
        NumberAnimation {
            id: dialogAnimation
            duration: 200
            easing.type: Easing.OutQuart
        }
    }

    onSelectedPlacemarkChanged: {
        if (!selectedPlacemark) {
            app.state = "none"
        }
    }

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Settings {
        id: settings
    }

    Item {
        id: mapItem
        anchors {
            top: parent.top
            topMargin: mapOffset
            left: parent.left
            right: parent.right
        }

        height: !dialogExpanded
                ? parent.height
                : parent.height + animatedMargin

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
                property bool smallZoom : radius < 2 * Math.max(app.width, app.height)

                anchors.fill: parent

                visible: true

                // Theme settings.
                projection: smallZoom ? MarbleItem.Spherical : MarbleItem.Mercator
                mapThemeId: settings.value("MarbleMaps", "mapThemeId", "earth/vectorosm/vectorosm.dgml")

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
                showPublicTransport: settings.value("MarbleMaps", "showPublicTransport", "false") === "true"
                positionProvider: suspended ? "" : currentPositionProvider
                keepScreenOn: !suspended && navigationManager.guidanceModeEnabled
                showPositionMarker: false
                animationViewContext: dialogAnimation.running

                placemarkDelegate: Image {
                    id: balloon
                    property int xPos: 0
                    property int yPos: 0
                    property real animationOffset: 0
                    property var placemark: null
                    x: xPos - 0.5 * width
                    y: yPos - height - 30 * Screen.pixelDensity * animationOffset
                    opacity: 1.0 - animationOffset

                    Connections {
                        target: app
                        onSelectedPlacemarkChanged:  balloonAnimation.restart()
                    }

                    NumberAnimation {
                      id: balloonAnimation
                      target: balloon
                      property: "animationOffset"
                      from: 1
                      to: 0
                      duration: 1000
                      easing.type: Easing.OutBounce
                    }


                    width: Screen.pixelDensity*6
                    height: width
                    source: "qrc:///ic_place.png"
                    onPlacemarkChanged: {
                        app.selectedPlacemark = placemark
                        if (placemark) {
                            app.state = "place"
                        } else {
                            app.state = "none"
                        }
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

                Connections {
                    target: Qt.application
                    onStateChanged: {
                        if (Qt.application.state === Qt.ApplicationInactive || Qt.application.state === Qt.ApplicationSuspended) {
                            marbleMaps.writeSettings()
                        }
                    }
                }

                function updateIndicator() {
                    if ( !positionVisible && positionAvailable ) {
                        zoomToPositionButton.updateIndicator();
                    }
                }

                RoutingManager {
                    id: routingManager
                    anchors.fill: parent
                    marbleItem: marbleMaps
                    visible: hasRoute

                    function addToRoute() {
                        ensureRouteHasDeparture()
                        routingManager.addViaByPlacemarkAtIndex(routingManager.waypointCount(), selectedPlacemark)
                        routingManager.clearSearchResultPlacemarks()
                        selectedPlacemark = null
                        app.state = "route"
                    }
                    function ensureRouteHasDeparture() {
                        if (routingManager.routeRequestModel.count === 0) {
                            if (marbleMaps.positionAvailable) {
                                routingManager.addViaByPlacemark(marbleMaps.currentPosition)
                            }
                        }
                    }

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
                    x: navigationManager.snappedPositionMarkerScreenPosition.x - positionMarker.width / 2
                    y: navigationManager.snappedPositionMarkerScreenPosition.y - positionMarker.height / 2
                    angle: marbleMaps.angle
                    visible: marbleMaps.positionAvailable && marbleMaps.positionVisible
                    radius: navigationManager.screenAccuracy / 2
                    showAccuracy: navigationManager.deviated
                    allowRadiusAnimation: !zoomDetectionTimer.running
                    allowPositionAnimation: !panningDetectionTimer.running
                    speed: marbleMaps.speed

                    MouseArea {
                        anchors.fill: parent
                        onPressed: app.state = "position"
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    propagateComposedEvents: true
                    onPressed: {
                        marbleMaps.focus = true;
                        mouse.accepted = false;
                    }
                }


            }

            NavigationManager {
                id: navigationManager
                width: parent.width
                height: parent.height
                visible: false
                marbleItem: marbleMaps
                hasRoute: routingManager.hasRoute
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

            enabled: marbleMaps.positionAvailable

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

            property string currentProfileIcon: "qrc:///material/directions-car.svg"

            anchors {
                bottom: parent.bottom
                bottomMargin: Screen.pixelDensity * 4 - mapOffset
                horizontalCenter: zoomToPositionButton.horizontalCenter
            }

            enabled: app.state !== "route" || routingManager.hasRoute

            onClicked: {
                if (app.state === "route") {
                    app.state = "none"
                    navigationManager.visible = true
                } else if (app.state === "place") {
                    app.state = "route"
                    routingManager.addToRoute()
                } else {
                    app.state = "route"
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
                    when: app.state === "route"
                    PropertyChanges { target: routeEditorButton; iconSource: "qrc:///material/navigation.svg"; }
                },
                State {
                    name: "placeAction"
                    when: app.state === "place"
                    PropertyChanges { target: routeEditorButton; iconSource: currentProfileIcon }
                }
            ]
        }
    }

    Search {
        id: search
        anchors.fill: parent
        marbleQuickItem: marbleMaps
        visible: !navigationManager.visible

        onItemSelected: {
            if (routingManager) {
                routingManager.addSearchResultAsPlacemark(suggestedPlacemark);
            }
            app.selectedPlacemark = suggestedPlacemark;
            app.state = "place"
        }
        onMenuButtonClicked: drawer.open()
    }

    Loader {
        id: dialogLoader

        focus: true

        anchors {
            left: parent.left
            right: parent.right
            top: parent.bottom
            topMargin: app.animatedMargin
        }

        onLoaded: {
          if (app.state === "place") {
              dialogLoader.item.map = marbleMaps
              dialogLoader.item.placemark = app.selectedPlacemark
              dialogLoader.item.showOsmTags = app.showOsmTags
              dialogLoader.item.showAccessibility = drawer.showAccessibility
          } else if (app.state === "route") {
              item.routingManager = routingManager
              item.routingProfile = routingManager.routingProfile
              item.currentIndex =  Qt.binding(function() { return app.currentWaypointIndex })
          } else if (app.state == "position") {
              dialogLoader.item.map = marbleMaps
              dialogLoader.item.navigationManager = navigationManager
          }
        }

        Connections {
          target: dialogLoader.item
          onCurrentProfileIconChanged: routeEditorButton.currentProfileIcon = dialogLoader.item.currentProfileIcon
          ignoreUnknownSignals: true
        }
    }

    BorderImage {
        visible: app.state != "none"
        anchors.fill: dialogLoader
        anchors.margins: -14
        border { top: 14; left: 14; right: 14; bottom: 14 }
        source: "qrc:///border_shadow.png"
    }

    SidePanel {
        id: drawer
        width: 0.67 * app.width
        height: app.height
        marbleMaps: marbleMaps

        onAboutActionTriggered: {
            app.state = "about"
            dialogLoader.focus = true
        }
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
                app.aboutToQuit = false
                quitHelper.visible = false
            }
        }
    }

    property bool aboutToQuit: false

    onClosing: {
        if (app.aboutToQuit === true) {
            close.accepted = true // we will quit
            return
        } else if (navigationManager.visible) {
            navigationManager.visible = false
        } else if (app.state !== "none") {
            app.state = "none"
        } else {
            app.aboutToQuit = true
            quitHelper.visible = true
        }
        close.accepted = false
    }

    Item {
      id: stateTracker

      state: "none"

      states: [
          State {
              name: "none"
              PropertyChanges { target: dialogLoader; source: "" }
          },
          State {
              name: "position"
              PropertyChanges { target: dialogLoader; source: "CurrentPosition.qml" }
          },
          State {
              name: "route"
              PropertyChanges { target: dialogLoader; source: "RouteEditor.qml" }
          },
          State {
              name: "place"
              PropertyChanges { target: dialogLoader; source: "PlacemarkDialog.qml" }
          },
          State {
              name: "about"
              PropertyChanges { target: dialogLoader; source: "AboutDialog.qml" }
          },
          State {
              name: "settings"
              PropertyChanges { target: dialogLoader; source: "SettingsDialog.qml" }
          },
          State {
              name: "developer"
              PropertyChanges { target: dialogLoader; source: "DeveloperDialog.qml" }
          }
      ]
    }
}
