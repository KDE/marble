//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//


import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

import org.kde.marble 0.20

ApplicationWindow {
    id: root
    title: qsTr("Behaim Globe")
    visible: true

    width: 600
    height: 400

    property bool landscape: root.width > root.height

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: palette.window
    }

    Grid {
        id: mainLayout
        columns: root.landscape ? 2 : 1
        columnSpacing: 0
        rows: root.landscape ? 1 : 2
        rowSpacing: 0
        layoutDirection: root.landscape ? Qt.RightToLeft : Qt.LeftToRight

        Item {
            id: mapItem

            width: root.landscape ? root.width - infoItem.width : root.width
            height: root.landscape ? root.height : root.height - infoItem.height

            Rectangle {
                color: "black"
                anchors.fill: parent
            }

            PinchArea {
                anchors.fill: parent
                enabled: true

                onPinchStarted: marbleMaps.handlePinchStarted(pinch.center)
                onPinchFinished: marbleMaps.handlePinchFinished(pinch.center)
                onPinchUpdated: marbleMaps.handlePinchUpdated(pinch.center, pinch.scale);

                MarbleItem {
                    id: marbleMaps
                    anchors.fill: parent

                    focus: true
                    zoom: 1150
                    inertialGlobeRotation: true

                    // Theme settings.
                    projection: MarbleItem.Spherical
                    mapThemeId: "earth/behaim1492/behaim1492.dgml"

                    // Visibility of layers/plugins.
                    showFrameRate: false
                    showAtmosphere: true
                    showCompass: false
                    showClouds: false
                    showCrosshairs: false
                    showGrid: false
                    showOverviewMap: false
                    showOtherPlaces: false
                    showScaleBar: false
                    showBackground: true
                    showPositionMarker: false

                    Component.onCompleted: {
                        setPluginSetting("stars", "renderConstellationLines", "false");
                        setPluginSetting("stars", "renderConstellationLabels", "false");
                        setPluginSetting("stars", "renderDsoLabels", "false");
                        setPluginSetting("stars", "viewSolarSystemLabel", "false");
                        setPluginSetting("stars", "zoomSunMoon", "false");
                        setPluginSetting("stars", "renderEcliptic", "false");
                        setPluginSetting("stars", "renderCelestialEquator", "false");
                        setPluginSetting("stars", "renderCelestialPole", "false");
                    }
                }

                Button {
                    id: minimizeLandscapeButton
                    visible: root.landscape
                    anchors.left: marbleMaps.left
                    anchors.top: marbleMaps.top
                    anchors.leftMargin: Screen.pixelDensity * (infoItem.minimized ? 0.5 : 1.5)
                    anchors.topMargin: Screen.pixelDensity * 0.5
                    iconSource: "menu.png"
                    onClicked: infoItem.minimized = !infoItem.minimized
                }

                Button {
                    id: minimizePortraitButton
                    visible: !root.landscape
                    anchors.right: marbleMaps.right
                    anchors.bottom: marbleMaps.bottom
                    anchors.rightMargin: Screen.pixelDensity * 0.5
                    anchors.bottomMargin: Screen.pixelDensity * (infoItem.minimized ? 0.5 : 1.5)
                    iconSource: "menu.png"
                    onClicked: infoItem.minimized = !infoItem.minimized
                }
            }
        }

        Item {
            id: infoItem
            property bool minimized: false
            width: root.landscape ? (minimized ? 0 : root.width / 2.5) : root.width
            height: root.landscape ? root.height : (minimized ? 0 : root.height / 2.5)

            Legend {
                id: legend
                anchors.fill: parent
                visible: !infoItem.minimized
            }
        }
    }
}
