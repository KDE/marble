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

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: palette.window
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

                anchors.fill: parent

                visible: true

                // Theme settings.
                projection: MarbleItem.Mercator
                mapThemeId: "earth/vectorosm/vectorosm.dgml"

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
                        placemarkDialog.item.placemark = placemark
                    }
                }

                onPositionAvailableChanged: {
                    updateIndicator();
                }
                onPositionVisibleChanged: {
                    updateIndicator();
                }
                onVisibleLatLonAltBoxChanged: {
                    updateIndicator();
                }
                onCurrentPositionChanged: {
                    updateIndicator();
                }

                Component.onCompleted: marbleMaps.loadSettings()
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
                    routingProfile: routeEditor.item.routingProfile
                }

                PositionMarker {
                    id: positionMarker
                    posX: navigationManager.snappedPositionMarkerScreenPosition.x
                    posY: navigationManager.snappedPositionMarkerScreenPosition.y
                    angle: marbleMaps.angle
                    visible: marbleMaps.positionAvailable && marbleMaps.positionVisible
                    radius: navigationManager.screenAccuracy
                    color: navigationManager.deviated ? "#40ff0000" : "transparent"
                    border.color: navigationManager.deviated ? "red" : "transparent"
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
                            marbleMaps.state = ""
                            quitHelper.visible = false
                        }
                    }
                }

                Keys.onBackPressed: {
                    if (marbleMaps.state === "aboutToQuit") {
                        event.accepted = false // we will quit
                    } else if (navigationManager.visible) {
                        navigationManager.visible = false
                        event.accepted = true
                    } else if (dialogContainer.visible) {
                        dialogContainer.currentIndex = dialogContainer.none
                        event.accepted = true
                    } else if (marbleMaps.state === "") {
                        marbleMaps.state = "aboutToQuit"
                        quitHelper.visible = true
                        event.accepted = true
                    } else {
                        marbleMaps.state = ""
                        event.accepted = true
                    }
                }
            }

            NavigationManager {
                id: navigationManager
                width: parent.width
                height: parent.height
                visible: false
                marbleItem: marbleMaps
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
                    placemarkDialog.item.addToRoute()
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
                    PropertyChanges { target: routeEditorButton; iconSource: placemarkDialog.item.actionIconSource }
                }
            ]
        }
    }

    TabView {
        id: dialogContainer
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        tabsVisible: false
        frameVisible: false
        visible: currentIndex >= 0
        height: visible ? 0.33 * root.height : 0

        readonly property int none: -1
        readonly property int routing: 0
        readonly property int place: 1
        readonly property int about: 2
        readonly property int settings: 3
        readonly property int developer: 4

        currentIndex: none
        Keys.onBackPressed: currentIndex = none

        Tab {
            active: true
            id: routeEditor
            RouteEditor {
                anchors.fill: parent
            }
        }

        Tab {
            active: true
            id: placemarkDialog
            PlacemarkDialog {
                anchors.fill: parent
                map: marbleMaps
            }
        }

        Tab { AboutDialog {} }
        Tab { active: true; SettingsDialog {} }
        Tab { active: true; DeveloperDialog {} }
    }

    BorderImage {
        visible: dialogContainer.visible
        anchors.fill: dialogContainer
        anchors.margins: -14
        border { top: 14; left: 14; right: 14; bottom: 14 }
        source: "qrc:///border_shadow.png"
    }

    Keys.onBackPressed: event.accepted = true
}
