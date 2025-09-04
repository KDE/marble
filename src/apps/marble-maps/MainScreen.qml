// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2015 Mikhail Ivchenko <ematirov@gmail.com>
//

import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts

import org.kde.marble as Marble
import org.kde.marble.maps
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.components as Components

Kirigami.ApplicationWindow {
    id: app
    title: i18n("Marble Maps")

    width: 600
    height: 400

    property string state

    property var selectedPlacemark
    property bool showOsmTags: false
    property int currentWaypointIndex: 0

    onSelectedPlacemarkChanged: {
        if (!selectedPlacemark) {
            app.state = "none"
        }
    }

    Marble.Settings {
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

        property alias showAccessibility: accessibilityAction.checked

        isMenu: true

        Marble.Settings {
            id: sidePanelSettings
            property bool showUpdateInfo: Number(value("MarbleMaps", "updateInfoVersion", "0")) < 1
            Component.onDestruction: {
                sidePanelSettings.setValue("MarbleMaps", "showAccessibility", accessibilityAction.checked ? "true" : "false")
            }
        }

        actions: [
            Kirigami.Action {
                id: publicTransportAction
                text: i18n("Public Transport")
                checkable: true
                checked: marbleMaps.showPublicTransport
                icon.source: Qt.resolvedUrl("images/transport-mode-bus.svg")
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
                text: i18n("Outdoor Activities")
                visible: true
                icon.source: Qt.resolvedUrl("images/transport-mode-walk.svg")
                onTriggered: {
                    sidePanel.close()
                    marbleMaps.showOutdoorActivities = checked
                }
            },
            Kirigami.Action {
                id: accessibilityAction
                checkable: true
                checked: settings.value("MarbleMaps", "showAccessibility", "false") === "true"
                text: i18nc("@action:button", "Accessibility")
                visible: true
                icon.name: 'preferences-desktop-accessibility-symbolic'
                onTriggered: {
                    sidePanelSettings.value("MarbleMaps", "showAccessibility", "false") === "true"
                }
            },
            Kirigami.Action{ separator: true },
            Kirigami.Action {
                text: i18nc("@action:button", "About")
                icon.name: 'help-about-symbolic'
                visible: true
                onTriggered: {
                    app.state = "about"
                    sidePanel.close()
                    source = ""
                    app.pageStack.pushDialogLayer(Qt.createComponent("org.kde.kirigamiaddons.formcard", "AboutPage"))
                }
            },
            Kirigami.Action {
                text: i18nc("@action:button", "Bookmarks")
                icon.name: 'bookmarks-symbolic'
                onTriggered: {
                    app.state = "bookmarks"
                    sidePanel.close()
                    app.pageStack.layers.push(Qt.createComponent("org.kde.marble.maps", "Bookmarks"), {
                        marbleMaps: marbleMaps
                    })
                }
            },
            Kirigami.Action {
                text: i18nc("@action:button", "Layer Options")
                icon.name: 'settings-configure-symbolic'
                onTriggered: {
                    app.state = "options"
                    sidePanel.close()
                    app.pageStack.layers.push(Qt.createComponent("org.kde.marble.maps", "Options"), {
                        marbleMaps: marbleMaps
                    })
                }
            },
            Kirigami.Action {
                text: i18nc("@action:button", "Theme Options")
                icon.name: 'settings-configure-symbolic'
                onTriggered: {
                    app.state = "options"
                    sidePanel.close()
                    app.pageStack.layers.push(Qt.createComponent("org.kde.marble.maps", "ThemeConfigPage"), {
                        marbleMaps: marbleMaps
                    })
                }
            },
            Kirigami.Action {
                text: i18n("Routing")
                icon.source: Qt.resolvedUrl("images/directions.svg")
                onTriggered: {
                    app.state = "route"
                    app.pageStack.layers.push(Qt.createComponent("org.kde.marble.maps", "RouteEditorPage"), {
                        routingManager: routingManager,
                        marbleMaps: marbleMaps,
                    })
                }
            }
        ]
    }

    pageStack.initialPage: page

    Kirigami.Page {
        id: page

        padding: 0
        topPadding: 0
        leftPadding: 0
        rightPadding: 0
        bottomPadding: 0
        title: i18n("Marble Maps")

        titleDelegate: Kirigami.SearchField {
            id: searchField

            Layout.fillWidth: true
            autoAccept: false
            onAccepted: {
                if (text.length === 0) {
                    searchResultPopup.visible = false;
                    return;
                }
                backend.search(text)
            }

            onTextChanged: fireSearchDelay.restart();

            Timer {
                id: fireSearchDelay
                interval: Kirigami.Units.shortDuration
                running: false
                repeat: false
                onTriggered: {
                    searchField.accepted();
                }
            }
        }


        SearchResultView {
            id: searchResultPopup

            anchors.fill: parent

            visible: false
            searchBackend: Marble.SearchBackend {
                id: backend
                marbleQuickItem: marbleMaps
                onSearchResultChanged: {
                    searchResultPopup.searchResults.model = model;
                    searchResultPopup.visible = true;
                }
            }

            background: Rectangle {
                Kirigami.Theme.colorSet: Kirigami.Theme.View
                Kirigami.Theme.inherit: false
                color: Kirigami.Theme.backgroundColor
            }

            z: 2

            onClicked: (placemark) => {
                searchResultPopup.visible = false;
                app.selectedPlacemark = placemark;
                app.state = "place"
            }
        }

        contentItem: Item {
            id: mapItem

            PinchArea {
                anchors.fill: parent
                enabled: true

                onPinchStarted: marbleMaps.handlePinchStarted(pinch.center)
                onPinchFinished: marbleMaps.handlePinchFinished(pinch.center)
                onPinchUpdated: marbleMaps.handlePinchUpdated(pinch.center, pinch.scale);

                MarbleMapsApplication {
                    id: marbleMapsApplication
                    keepScreenOn: !suspended && navigationManager.guidanceModeEnabled
                }

                Marble.MarbleItem {
                    id: marbleMaps

                    property string currentPositionProvider: "QtPositioning"
                    property bool wlanOnly: false
                    property bool smallZoom : radius < 2 * Math.max(app.width, app.height)

                    anchors.fill: parent
                    visible: true

                    // Theme settings.
                    projection: smallZoom ? Marble.MarbleItem.Spherical : Marble.MarbleItem.Mercator
                    mapThemeId: Config.mapThemeId

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
                    positionProvider: marbleMapsApplication.suspended ? "" : currentPositionProvider
                    showPositionMarker: false
                    animationViewContext: dialogAnimation.running

                    placemarkDelegate: Kirigami.Icon {
                        id: balloon
                        property int xPos: 0
                        property int yPos: 0
                        property real animationOffset: 0
                        property var placemark: null
                        x: xPos - 0.5 * width
                        y: yPos - height - 30 * animationOffset
                        opacity: 1.0 - animationOffset

                        Connections {
                            target: app
                            onSelectedPlacemarkChanged: balloonAnimation.restart()
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

                        width: Kirigami.Units.iconSizes.medium
                        height: width
                        source: "add-placemark-symbolic"
                        onPlacemarkChanged: {
                            app.selectedPlacemark = placemark
                            if (placemark) {
                                placemarkDialog.open();
                            } else {
                                placemarkDialog.close();
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

                        routingProfile: switch (Config.profile) {
                        case Config.Car:
                            return Marble.Routing.Motorcar;
                        case Config.Bicycle:
                            return Marble.Routing.Bicycle;
                        case Config.Pedestrian:
                            return Marble.Routing.Pedestrian;
                        }


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
                text: i18n("%1 km", zoomToPositionButton.distance < 10 ? zoomToPositionButton.distance.toFixed(1) : zoomToPositionButton.distance.toFixed(0))
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
                    rightMargin: Kirigami.Units.largeSpacing
                    bottom: parent.bottom
                    bottomMargin: Kirigami.Units.largeSpacing
                }

                enabled: marbleMaps.positionAvailable

                icon.source: marbleMaps.positionAvailable ? Qt.resolvedUrl("images/gps_fixed.png") : Qt.resolvedUrl("images/gps_not_fixed.png")

                onClicked: marbleMaps.centerOnCurrentPosition()

                property real distance: 0

                function updateIndicator(): void {
                    const point = marbleMaps.mapFromItem(zoomToPositionButton, diameter * 0.5, diameter * 0.5);
                    distance = 0.001 * marbleMaps.distanceFromPointToCurrentLocation(point);
                    angle = marbleMaps.angleFromPointToCurrentLocation(point);
                }

                showDirection: marbleMaps.positionAvailable && !marbleMaps.positionVisible
            }
        }

        PlacemarkDialog {
            id: placemarkDialog

            visible: false
            placemark: app.selectedPlacemark
            bookmarks: bookmarks
            showOsmTags: app.showOsmTags
            showAccessibility: sidePanel.showAccessibility
            parent: app.Overlay.overlay
        }

        /*
        Kirigami.OverlayDrawer {
            id: drawer

            handleVisible: false
            edge: Qt.BottomEdge

            height: parent.height

            visible: app.state === "place"

            contentItem: ColumnLayout {
                Kir
                RowLayout {
                    Layout.fillWidth: true

                    ToolButton {
                        id: routeEditorButton
                        property string currentProfileIcon: Qt.resolvedUrl("./images/transport-mode-car.svg")
                        enabled: app.state !== "route" || routingManager.hasRoute
                        icon.source: Qt.resolvedUrl("./images/directions.svg")

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
                                PropertyChanges {
                                    target: routeEditorButton
                                    icon.source: Qt.resolvedUrl("images/directions.svg")
                                }
                            },
                            State {
                                name: "routingAction"
                                when: app.state === "route"
                                PropertyChanges { target: routeEditorButton; icon.source: "qrc:///material/navigation.svg"; }
                            },
                            State {
                                name: "placeAction"
                                when: app.state === "place"
                                PropertyChanges { target: routeEditorButton; icon.source: Qt.resolvedUrl("images/directions.svg") }
                            }
                        ]
                    }

                    ToolButton {
                        id: bookmarkButton
                        property bool bookmark: bookmarks.isBookmark(app.selectedPlacemark.longitude, app.selectedPlacemark.latitude)
                        enabled: app.state === "place"
                        visible: app.state === "place"
                        icon.name: bookmark ? 'bookmarks-bookmarked-symbolic' : 'bookmarks-symbolic'
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

                Loader {
                    id: dialogLoader

                    Layout.fillWidth: true

                    onLoaded: {
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

                Item {
                    Layout.fillHeight: true
                }
            }
        } */

        /* BoxedText {
            id: quitHelper
            visible: false
            text: i18n("Press again to close.")
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
        }*/
    }

    Marble.Bookmarks {
        id: bookmarks
        map: marbleMaps
    }
}

