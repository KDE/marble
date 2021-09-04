//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2015 Mikhail Ivchenko <ematirov@gmail.com>
//

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Controls.Material 2.0


import org.kde.marble 0.20
import org.kde.kirigami 2.0 as Kirigami

Kirigami.AbstractApplicationWindow {
    id: app
    title: qsTr("Marble Maps")
    visible: true

    width: 600
    height: 400

    Material.theme: Material.Light
    Material.accent: Material.Blue

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
        else {
            bookmarkButton.bookmark = bookmarks.isBookmark(selectedPlacemark.longitude, selectedPlacemark.latitude)
        }
    }

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Settings {
        id: settings
    }


    property bool aboutToQuit: false

    onClosing: {
        if (app.aboutToQuit || Qt.platform.os !== "android") {
            close.accepted = true // we will quit
            return
        } else if (sidePanel.drawerOpen) {
            sidePanel.close()
        } else if (pageStack.depth > 1) {
            pageStack.pop()
        } else if (navigationManager.visible) {
            navigationManager.visible = false
        } else if (app.state !== "none") {
            app.state = "none"
        } else if(search.searchResultsVisible.visible){
            search.searchResultsVisible = false
        }
        else {
            if(search.searchResultsVisible){
                search.searchResultsVisible = false
            }
            app.aboutToQuit = true
            quitHelper.visible = true
        }
        close.accepted = false
    }

    globalDrawer: Kirigami.GlobalDrawer {
        id: sidePanel
        title: qsTr("Settings")

        handleVisible: false
        property alias showAccessibility: accessibilityAction.checked

        Settings {
            id: sidePanelSettings
            property bool showUpdateInfo: Number(value("MarbleMaps", "updateInfoVersion", "0")) < 1
            Component.onDestruction: {
                sidePanelSettings.setValue("MarbleMaps", "showAccessibility", accessibilityAction.checked ? "true" : "false")
            }
        }

        actions: [
            Kirigami.Action {
                id: publicTransportAction
                text: qsTr("Public Transport")
                checkable: true
                checked: marbleMaps.showPublicTransport
                iconName: "qrc:///material/directions-bus.svg"
                visible: true
                onTriggered: {
                    sidePanel.close()
                    marbleMaps.showPublicTransport = checked
                    publicTransportDialog.open()
                }
            },
            Kirigami.Action {
                id: outdoorActivitiesAction
                checkable: true
                checked: marbleMaps.showOutdoorActivities
                text: qsTr("Outdoor Activities")
                visible: true
                iconName: "qrc:///material/directions-run.svg"
                onTriggered: {
                    sidePanel.close()
                    marbleMaps.showOutdoorActivities = checked
                }
            },
            Kirigami.Action {
                id: accessibilityAction
                checkable: true
                checked: settings.value("MarbleMaps", "showAccessibility", "false") === "true"
                text: qsTr("Accessibility")
                visible: true
                iconName: "qrc:///material/wheelchair.svg"
                onTriggered: {
                    sidePanelSettings.value("MarbleMaps", "showAccessibility", "false") === "true"
                }
            },
            Kirigami.Action{ enabled: false},
            Kirigami.Action {
                text: qsTr("About")
                iconName: "qrc:///marble.svg"
                visible: true
                onTriggered: {
                    app.state = "about"
                    sidePanel.close()
                    source = ""
                    app.pageStack.push("qrc:///AboutDialog.qml")
                }
            },
            Kirigami.Action {
                text: qsTr("Bookmarks")
                iconName: "qrc:///material/star.svg"
                onTriggered: {
                    app.state = "bookmarks"
                    sidePanel.close()
                    app.pageStack.push("qrc:///Bookmarks.qml")
                }
            },
            Kirigami.Action {
                text: qsTr("Layer Options")
                iconName: "qrc:///settings.png"
                onTriggered: {
                    app.state = "options"
                    sidePanel.close()
                    app.pageStack.push("qrc:///Options.qml")
                }
            },
            Kirigami.Action {
                text: qsTr("Routing")
                iconName: "qrc:///material/directions.svg"
                onTriggered: {
                    app.state = "route"
                }
            }
        ]

        Binding {
            target: pageStack.currentItem
            property: "marbleQuickItem"
            value: marbleMaps
            when: app.state === "bookmarks"
        }
    }

    pageStack: StackView {
        anchors.fill: parent
        initialItem: page
    }

    Kirigami.Page {
        id: page
        padding: 0
        topPadding: 0
        leftPadding: 0
        rightPadding: 0
        bottomPadding: 0
        title: qsTr("Marble Maps")

        Item {
            id: mapItem

            width: parent.width
            height: parent.height - dialogLoader.height - bottomMenu.height

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
                    bottom: mapItem.bottom
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
        }


        Row {
            id: bottomMenu
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: dialogLoader.top
            width: parent.width
            height: bottomMenu.visible ? routeEditorButton.height + Screen.pixelDensity * 2 : 0
            anchors.topMargin: app.animatedMargin
            visible: app.state === "place" || app.state === "route"

            onVisibleChanged: bottomMenuAnimation.start()

            NumberAnimation {
                id: bottomMenuAnimation
                target: bottomMenu
                property: "y"
                from: app.height - bottomMenu.height
                to: 0
                duration: 500
                easing.type: Easing.InExpo
            }

            Item {
                id: bottomMenuBackground
                anchors.fill: parent
                Rectangle {
                    color: Material.accent
                    anchors.fill : parent
                }
            }

            Row {
                anchors.centerIn: parent
                spacing: Kirigami.Units.gridUnit * 2

                FlatButton {
                    id: routeEditorButton
                    property string currentProfileIcon: "qrc:///material/directions-car.svg"
                    height: Screen.pixelDensity * 6
                    width: height
                    enabled: app.state !== "route" || routingManager.hasRoute
                    imageSource: "qrc:///material/directions.svg"

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
                    states: [
                        State {
                            name: ""
                            PropertyChanges { target: routeEditorButton; imageSource: "qrc:///material/directions.svg"; }
                        },
                        State {
                            name: "routingAction"
                            when: app.state === "route"
                            PropertyChanges { target: routeEditorButton; imageSource: "qrc:///material/navigation.svg"; }
                        },
                        State {
                            name: "placeAction"
                            when: app.state === "place"
                            PropertyChanges { target: routeEditorButton; imageSource: "qrc:///material/directions.svg" }
                        }
                    ]
                }

                FlatButton {
                    id: bookmarkButton
                    anchors.verticalCenter: parent.verticalCenter
                    height: Screen.pixelDensity * 6
                    width: height
                    property bool bookmark: bookmarks.isBookmark(app.selectedPlacemark.longitude, app.selectedPlacemark.latitude)
                    enabled: app.state === "place"
                    visible: app.state === "place"
                    imageSource: bookmark ? "qrc:///material/star.svg" : "qrc:///material/star_border.svg"
                    onClicked: {
                        if (bookmarkButton.bookmark) {
                            bookmarks.removeBookmark(app.selectedPlacemark.longitude, app.selectedPlacemark.latitude)
                        } else {
                            bookmarks.addBookmark(app.selectedPlacemark, "Default")
                        }
                        bookmarkButton.bookmark = !bookmarkButton.bookmark
                    }
                }
            }
        }


        BorderImage {
            anchors.top: mapItem.bottom
            anchors.bottom: dialogLoader.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.margins: -14
            border { top: 14; left: 14; right: 14; bottom: 14 }
            source: "qrc:///border_shadow.png"
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
            onMenuButtonClicked: sidePanel.open()
        }

        Loader {
            id: dialogLoader
            focus: true
            width: childrenRect.width
            height : childrenRect.height

            anchors {
                left: parent.left
                right: parent.right
                top: parent.bottom
                bottom: bottomMenu.top
                topMargin: app.animatedMargin
                bottomMargin: Kirigami.Units.gridUnits * 10
            }

            NumberAnimation {
                id: loaderAnimation
                target: dialogLoader.item
                property: "y"
                from: dialogLoader.height === 0 ? app.height : app.height - dialogLoader.item.height
                to: 0
                duration: 500
                easing.type: Easing.InExpo
            }

            onLoaded: {
                app.state != "none" ? loaderAnimation.running = true : loaderAnimation.running = false
                if (app.state === "place") {
                    dialogLoader.item.map = marbleMaps
                    dialogLoader.item.placemark = app.selectedPlacemark
                    dialogLoader.item.showOsmTags = app.showOsmTags
                    dialogLoader.item.showAccessibility = sidePanel.showAccessibility
                } else if (app.state === "route") {
                    item.routingManager = routingManager
                    item.routingProfile = routingManager.routingProfile
                    item.currentIndex =  Qt.binding(function() { return app.currentWaypointIndex })
                } else if (app.state == "position") {
                    dialogLoader.item.map = marbleMaps
                    dialogLoader.item.navigationManager = navigationManager
                } else if (app.state == "none"){
                    dialogLoader.height = 0
                }
            }

            Connections {
                target: dialogLoader.item
                onCurrentProfileIconChanged: routeEditorButton.currentProfileIcon = dialogLoader.item.currentProfileIcon
                ignoreUnknownSignals: true
            }
        }

        Rectangle {
            width: parent.width
            color: Kirigami.Theme.textColor
            opacity: 0.4
            height: 1
            anchors.bottom: dialogLoader.top
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
                    PropertyChanges { target: dialogLoader; source: "" }
                },
                State {
                    name: "settings"
                    PropertyChanges { target: dialogLoader; source: "SettingsDialog.qml" }
                },
                State {
                    name: "developer"
                    PropertyChanges { target: dialogLoader; source: "DeveloperDialog.qml" }
                },
                State {
                    name: "options"
                    PropertyChanges { target: dialogLoader; source: "" }
                },
                State {
                    name: "bookmarks"
                    PropertyChanges { target: dialogLoader; source: "" }
                }
            ]
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

        Bookmarks {
            id: bookmarks
            map: marbleMaps
        }
    }
}

