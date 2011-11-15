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
            iconId: "toolbar-view-menu" }
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

        Column {
            anchors.fill: parent
            anchors.margins: 5
            spacing: 5

            ToolBarLayout {
                width: parent.width
                height: 40

                Label {
                    text: "Source: Current Location"
                }

//                ToolIcon {
//                    iconId: "toolbar-edit"
//                }
            }

            ToolBarLayout {
                width: parent.width
                height: 40

                Label {
                    id: destinationInputLabel
                    text: editing ? "Destination: Select a point" : "Destination: Point in map"

                    property bool editing: false
                    property real longitude: 0.0
                    property real latitude: 0.0

                    function retrieveInput( lon, lat ) {
                        if ( editing ) {
                            longitude = lon
                            latitude = lat
                            editing = false

                            marbleWidget.getRouting().clearRoute()
                            marbleWidget.getRouting().routingProfile = routingTypeOptions.routingType
                            marbleWidget.getRouting().setVia( 0, marbleWidget.getTracking().lastKnownPosition.longitude, marbleWidget.getTracking().lastKnownPosition.latitude )
                            marbleWidget.getRouting().setVia( 1, lon, lat )
                        }
                    }

                    Component.onCompleted: marbleWidget.mouseClickGeoPosition.connect(retrieveInput)
                }

                ToolIcon {
                    iconId: "toolbar-edit"
                    onClicked: {
                        inputSelectionDialog.open()
                    }
                }
            }

            SelectionDialog {
                id: inputSelectionDialog
                titleText: "Select via point"
                model: ListModel {
                    //ListElement { name: "Use current location" }
                    ListElement { name: "Select in map" }
                }

                onAccepted: {
                    destinationInputLabel.editing = true
                }
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

        Component.onCompleted: {
            marbleWidget.parent = mapContainer
            settings.projection = "Mercator"
            var plugins = settings.defaultRenderPlugins
            settings.removeElementsFromArray(plugins, ["coordinate-grid", "sun", "stars", "compass"])
            settings.activeRenderPlugins = plugins
            settings.mapTheme = "earth/openstreetmap/openstreetmap.dgml"
            settings.gpsTracking = false
            settings.showPosition = false
            settings.showTrack = false
            marbleWidget.visible = true
        }

        Component.onDestruction: {
            marbleWidget.parent = null
            marbleWidget.visible = false
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
                    platformStyle: LabelStyle { fontPixelSize: 14 }
                    text: display;
                    width: parent.width - 48
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                }
            }

            Rectangle {
                x: 5
                width: parent.width - 20
                height: 1
                color: "darkgray"
            }
        }
    }
}
