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
                marbleWidget.centerOn( marbleWidget.tracking.lastKnownPosition.longitude, marbleWidget.tracking.lastKnownPosition.latitude )
                if (marbleWidget.zoom < 22026 ) {
                    marbleWidget.zoom = 86250
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
                text: "Start Navigation"
                onClicked: openActivity( "Navigation" )
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
                    marbleWidget.setGeoSceneProperty( "hillshading", checked )
                }
            }
        }
    }

    Flickable {
        id: searchResultView
        width: parent.width / 2
        height: parent.height
        contentWidth: width
        contentHeight: routeEditor.height + waypointListView.height
        anchors.margins: 5

        RouteEditor {
            id: routeEditor
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 5
        }

        ListView {
            id: waypointListView
            anchors.top: routeEditor.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 5
            height: 74 * count

            model: marbleWidget.routing.waypointModel()
            delegate: turnTypeDelegate
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            highlightMoveDuration: 200
            focus: true
            interactive: false
            spacing: 4
            clip: true
        }
    }

    ScrollDecorator {
        flickableItem: searchResultView
    }

    Item {
        id: mapContainer
        clip: true
        width: parent.width / 2
        height: parent.height

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Mercator"
            var plugins = settings.defaultRenderPlugins
            settings.removeElementsFromArray(plugins, ["coordinate-grid", "sun", "stars", "compass"])
            settings.activeRenderPlugins = plugins
            settings.mapTheme = settings.streetMapTheme
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
                AnchorChanges { target: mapContainer; anchors.left: routingActivityPage.left; anchors.top: routingActivityPage.top }
                PropertyChanges { target: mapContainer; width: routingActivityPage.width; height: routingActivityPage.height / 2 }
                AnchorChanges { target: searchResultView; anchors.right: routingActivityPage.right; anchors.top: mapContainer.bottom }
                PropertyChanges { target: searchResultView; width: routingActivityPage.width; height: routingActivityPage.height / 2 }
            }
        ]
    }

    FileSaveDialog {
        id: saveRouteDialog
        anchors.fill: parent
        folder: "/home/user/MyDocs"
        filename: ""
        nameFilters: [ "*.kml" ]

        onAccepted: { marbleWidget.routing.saveRoute( folder + "/" + filename ); }
    }

    FileOpenDialog {
        id: openRouteDialog
        anchors.fill: parent
        folder: "/home/user/MyDocs"
        nameFilters: [ "*.kml", "*.gpx" ]

        onAccepted: { marbleWidget.routing.openRoute( folder + "/" + filename ); }
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
                    id: turnIcon
                    width: 64; height: 64
                    source: "qrc" + turnTypeIcon
                }

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    platformStyle: LabelStyle { fontPixelSize: 18 }
                    text: display;
                    width: parent.width - turnIcon.width - 20
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            waypointListView.currentIndex = index
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

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
        }
    }
}
