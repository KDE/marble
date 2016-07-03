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

import org.kde.edu.marble 0.20

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

    StackView {
        id: itemStack

        anchors.fill: parent
        focus: true

        initialItem: mapItem

        delegate: StackViewDelegate {
            function transitionFinished(properties)
            {
                properties.exitItem.visible = true
            }
        }

        Item {
            id: mapItem

            PinchArea {
                anchors.fill: parent
                enabled: true

                onPinchStarted: marbleMaps.handlePinchStarted(pinch.center)
                onPinchFinished: marbleMaps.handlePinchFinished(pinch.center)
                onPinchUpdated: marbleMaps.handlePinchUpdated(pinch.center, pinch.scale);

                MarbleMaps {
                    id: marbleMaps

                    property string currentPositionProvider: "QtPositioning"

                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                        bottom: dialogContainer.top
                    }

                    visible: true
                    focus: true

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
                        routingProfile: routeEditor.routingProfile
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

                    MouseArea{
                        anchors.fill: parent
                        propagateComposedEvents: true
                        onPressed: {
                            search.focus = true;
                            mouse.accepted = false;
                            marbleMaps.selectPlacemarkAt(mouse.x, mouse.y)
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
                            onTriggered: itemStack.state = ""
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

                BorderImage {
                    anchors.fill: dialogContainer
                    anchors.margins: -14
                    border { top: 14; left: 14; right: 14; bottom: 14 }
                    source: "qrc:///border_shadow.png"
                }

                Item {
                    id: dialogContainer
                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }
                    height: routeEditor.visible ? routeEditor.height : (placemarkDialog.visible ? placemarkDialog.height : 0)

                    RouteEditor {
                        id: routeEditor
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                        visible: false
                    }

                    PlacemarkDialog {
                        id: placemarkDialog
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }

                    DeveloperDialog {
                        id: developerDialog
                        visible: false
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }
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

                PositionButton {
                    id: zoomToPositionButton
                    anchors {
                        right: parent.right
                        rightMargin: 0.005 * root.width
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
                        bottom: dialogContainer.height > 0 ? undefined : parent.bottom
                        verticalCenter: dialogContainer.height > 0 ? dialogContainer.top : undefined
                        horizontalCenter: zoomToPositionButton.horizontalCenter
                        margins: 0.01 * root.width
                        bottomMargin: 25
                    }

                    onClicked: {
                        if (itemStack.state === "routing") {
                            itemStack.state = "navigation"
                        } else if (itemStack.state === "place") {
                            placemarkDialog.addToRoute()
                        } else {
                            itemStack.state = "routing"
                        }
                    }
                    iconSource: "qrc:///directions.png";

                    states: [
                        State {
                            name: ""
                            AnchorChanges { target: routeEditorButton; anchors.bottom: parent.bottom; anchors.verticalCenter: undefined; }
                            PropertyChanges { target: routeEditorButton; iconSource: "qrc:///directions.png"; }
                        },
                        State {
                            name: "routingAction"
                            when: itemStack.state == "routing"
                            AnchorChanges { target: routeEditorButton; anchors.bottom: undefined; anchors.verticalCenter: dialogContainer.top; }
                            PropertyChanges { target: routeEditorButton; iconSource: "qrc:///navigation.png"; }
                        },
                        State {
                            name: "placeAction"
                            when: itemStack.state == "place"
                            AnchorChanges { target: routeEditorButton; anchors.bottom: undefined; anchors.verticalCenter: dialogContainer.top; }
                            PropertyChanges { target: routeEditorButton; iconSource: placemarkDialog.actionIconSource }
                        }
                    ]
                }
            }
        }

        states: [
            State {
                name: ""
                PropertyChanges { target: quitHelper; visible: false }
                PropertyChanges { target: search; visible: true }
                PropertyChanges { target: placemarkDialog; visible: false }
                PropertyChanges { target: routeEditor; visible: false }
                PropertyChanges { target: navigationManager; guidanceMode: false }
                StateChangeScript { script: itemStack.pop(mapItem); }
            },
            State {
                name: "place"
                PropertyChanges { target: search; visible: true }
                PropertyChanges { target: placemarkDialog; visible: true }
                PropertyChanges { target: routeEditor; visible: false }
                PropertyChanges { target: navigationManager; guidanceMode: false }
                StateChangeScript { script: itemStack.pop(mapItem); }
            },
            State {
                name: "routing"
                PropertyChanges { target: search; visible: true }
                PropertyChanges { target: placemarkDialog; visible: false }
                PropertyChanges { target: routeEditor; visible: true }
                PropertyChanges { target: navigationManager; guidanceMode: false }
                StateChangeScript { script: itemStack.pop(mapItem); }
            },
            State {
                name: "navigation"
                PropertyChanges { target: search; visible: false }
                PropertyChanges { target: placemarkDialog; visible: false }
                PropertyChanges { target: routeEditor; visible: false }
                PropertyChanges { target: navigationManager; guidanceMode: true }
                PropertyChanges { target: marbleMaps; anchors.bottomMargin: navigationManager.speedDistancePanelheight }
                PropertyChanges { target: routeEditorButton; anchors.bottomMargin: navigationManager.speedDistancePanelheight + Screen.pixelDensity*4 }
                StateChangeScript { script: itemStack.push(navigationManager); }
            },
            State {
                name: "aboutToQuit"
                PropertyChanges { target: quitHelper; visible: true }
            }

        ]

        Keys.onBackPressed: {
            if (itemStack.state === "aboutToQuit") {
                event.accepted = false // we will quit
            }
            else if (itemStack.state === "") {
                itemStack.state = "aboutToQuit"
                event.accepted = true
            }
            else {
                itemStack.state = ""
                event.accepted = true
            }
        }
    }
}
