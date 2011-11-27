// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11
import org.kde.edu.marble.qtcomponents 0.12

/*
 * Page for routing activity.
 */
Page {
    id: routingActivityPage
    anchors.fill: parent

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back";
            onClicked: pageStack.pop()
        }
        ToolIcon {
            iconId: "toolbar-home"
            onClicked: {
                marbleWidget.centerOn( marbleWidget.getTracking().lastKnownPosition.longitude, marbleWidget.getTracking().lastKnownPosition.latitude )
                if (marbleWidget.zoom < 2000 ) {
                    marbleWidget.zoom = 2773
                }
            }
        }
        ToolIcon {
            iconId: "toolbar-view-menu"
            onClicked: pageMenu.open()
        }
    }

    Menu {
        id: pageMenu
        content: MenuLayout {
            MenuItem {
                text: "Map Theme"
                onClicked: {
                    pageStack.push( "qrc:/MapThemeSelectionPage.qml" )
                }
            }
            MenuItem {
                text: "Save Route"
                onClicked: {
                    saveRouteDialog.filename = "route-" + Qt.formatDateTime(new Date(), "yyyy-MM-dd_hh.mm.ss") + ".kml"
                    saveRouteDialog.open()
                }
            }
            MenuItem {
                text: "Open Route"
                onClicked: openRouteDialog.open()
            }
            MenuItemSwitch {
                text: "Online"
                checked: !settings.workOffline
                onClicked: settings.workOffline = !settings.workOffline
            }
            MenuItemSwitch {
                text: "Elevation Profile"
                checked: false
                onCheckedChanged: {
                    var plugins = settings.activeRenderPlugins
                    if ( checked ) {
                        plugins.push("elevationprofile")
                    } else {
                        settings.removeElementsFromArray(plugins, ["elevationprofile"])
                    }
                    settings.activeRenderPlugins = plugins
                }
            }
        }
    }

    Rectangle {
        id: searchResultView
        radius: 10
        color: "lightsteelblue"
        border.width: 1
        border.color: "lightgray"
        z: 10
        opacity: 0.9
        property string searchTerm: ""

        function calculateRoute() {
            marbleWidget.getRouting().clearRoute()
            marbleWidget.getRouting().routingProfile = routingTypeOptions.routingType
            if ( sourcePoint.isCurrentPosition ) {
                marbleWidget.getRouting().setVia( 0, marbleWidget.getTracking().lastKnownPosition.longitude, marbleWidget.getTracking().lastKnownPosition.latitude )
            } else {
                marbleWidget.getRouting().setVia( 0, sourcePoint.longitude, sourcePoint.latitude )
            }
            if ( destinationPoint.isCurrentPosition ) {
                marbleWidget.getRouting().setVia( 1, marbleWidget.getTracking().lastKnownPosition.longitude, marbleWidget.getTracking().lastKnownPosition.latitude )
            } else {
                marbleWidget.getRouting().setVia( 1, destinationPoint.longitude, destinationPoint.latitude )
            }
        }

        Column {
            anchors.fill: parent
            anchors.margins: 5
            spacing: 5

            ViaPointEditor {
                id: sourcePoint
                width: parent.width
                height: 40
                isCurrentPosition: true
                name: "From"

                Component.onCompleted: marbleWidget.mouseClickGeoPosition.connect(retrieveInput)
                onPositionChanged: searchResultView.calculateRoute()
            }

            ViaPointEditor {
                id: destinationPoint
                width: parent.width
                height: 40
                isCurrentPosition: false
                name: "To"

                Component.onCompleted: marbleWidget.mouseClickGeoPosition.connect(retrieveInput)
                onPositionChanged: searchResultView.calculateRoute()
            }

            ButtonRow {
                width: parent.width
                id: routingTypeOptions
                checkedButton: routingMotorcarButton
                property string routingType: checkedButton.routingType
                onRoutingTypeChanged: {
                    marbleWidget.getRouting().routingProfile = routingTypeOptions.routingType
                    marbleWidget.getRouting().updateRoute()
                }
                Button {
                    id: routingMotorcarButton
                    iconSource: "qrc:/icons/routing-motorcar.svg"
                    property string routingType: "Motorcar"
                }

                Button {
                    id: routingBikeButton
                    iconSource: "qrc:/icons/routing-bike.svg"
                    property string routingType: "Bicycle"
                }

                Button {
                    id: routingPedestrianButton
                    iconSource: "qrc:/icons/routing-pedestrian.svg"
                    property string routingType: "Pedestrian"
                }
            }

            ListView {
                id: searchResultListView
                anchors.top: routingTypeOptions.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 5
                model: marbleWidget.waypointModel()
                delegate: turnTypeDelegate
                highlight: Rectangle { color: "white"; radius: 5 }
                focus: true
                spacing: 4
                clip: true
            }

            ScrollDecorator {
                flickableItem: searchResultListView
            }
        }
    }

    Item {
        id: mapContainer
        clip: true
        width: parent.width
        height: parent.height

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Mercator"
            var plugins = settings.defaultRenderPlugins
            settings.removeElementsFromArray(plugins, ["coordinate-grid", "sun", "stars", "compass"])
            settings.activeRenderPlugins = plugins
            settings.mapTheme = "earth/openstreetmap/openstreetmap.dgml"
            settings.gpsTracking = true
            settings.showPosition = true
            settings.showTrack = false
            marbleWidget.visible = true
        }

        Component.onDestruction: {
            if ( marbleWidget.parent === mapContainer ) {
                marbleWidget.parent = null
                marbleWidget.visible = false
            }
        }
    }

    StateGroup {
        states: [
            State { // Horizontal
                when: (routingActivityPage.width / routingActivityPage.height) > 1.20
                AnchorChanges { target: searchResultView; anchors.top: routingActivityPage.top }
                PropertyChanges { target: searchResultView; width: routingActivityPage.width / 2; height: routingActivityPage.height }
                AnchorChanges { target: mapContainer; anchors.left: searchResultView.right; anchors.bottom: routingActivityPage.bottom; anchors.top: routingActivityPage.top; }
                PropertyChanges { target: mapContainer; width: routingActivityPage.width / 2; height: routingActivityPage.height }
            },
            State { // Vertical
                when: (true)
                AnchorChanges { target: mapContainer; anchors.left: routingActivityPage.left; anchors.bottom: searchResultListView.top; anchors.top: routingActivityPage.top }
                PropertyChanges { target: mapContainer; width: routingActivityPage.width; height: routingActivityPage.height / 2 }
                AnchorChanges { target: searchResultView; anchors.right: routingActivityPage.right; anchors.top: mapContainer.bottom }
                PropertyChanges { target: searchResultView; width: routingActivityPage.width; height: routingActivityPage.height / 2 }
            }
        ]
    }

    Component {
        id: turnTypeDelegate

        Column {
            width: parent.width
            spacing: 4

            Row {
                id: row
                width: parent.width
                Image {
                    width: 32; height: 32
                    source: "qrc" + turnTypeIcon
                }

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    platformStyle: LabelStyle { fontPixelSize: 14 }
                    text: display;
                    width: parent.width - 48
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            searchResultListView.currentIndex = index
                            marbleWidget.centerOn(longitude, latitude)
                        }
                    }
                }
            }

            Rectangle {
                x: 5
                width: parent.width - 20
                height: 1
                color: "white"
            }
        }
    }

    FileSaveDialog {
        id: saveRouteDialog
        anchors.fill: parent
        folder: "/home/user/MyDocs"
        filename: ""
        nameFilters: [ "*.kml" ]

        onAccepted: { marbleWidget.getRouting().saveRoute( folder + "/" + filename ); }
    }

    FileOpenDialog {
        id: openRouteDialog
        anchors.fill: parent
        folder: "/home/user/MyDocs"
        nameFilters: [ "*.kml", "*.gpx" ]

        onAccepted: { marbleWidget.getRouting().openRoute( folder + "/" + filename ); }
    }

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
        }
    }
}
