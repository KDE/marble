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

Item {
    id: root
    height: column.height + Screen.pixelDensity * 4

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    Settings {
        id: settings

        Component.onDestruction: {
            settings.setValue("Developer", "inertialGlobeRotation", marbleMaps.inertialGlobeRotation)
            settings.setValue("Developer", "positionProvider", marbleMaps.currentPositionProvider)
            settings.setValue("Developer", "runtimeTrace", runtimeTrace.checked ? "true" : "false")
            settings.setValue("Developer", "debugTags", debugTags.checked ? "true" : "false")
            settings.setValue("Developer", "debugPlacemarks", debugPlacemarks.checked ? "true" : "false")
            settings.setValue("Developer", "debugPolygons", debugPolygons.checked ? "true" : "false")
            settings.setValue("Developer", "debugBatches", debugBatches.checked ? "true" : "false")
            settings.setValue("Developer", "debugOutput", debugOutputEnabled ? "true" : "false")
        }
    }

    Rectangle {
        anchors.fill: parent
        color: palette.base
    }

    Column {
        id: column
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: Screen.pixelDensity * 2
        }

        spacing: Screen.pixelDensity * 1

        Grid {
            columns: 2
            flow: Grid.TopToBottom
            spacing: Screen.pixelDensity * 2

            Column {
                spacing: Screen.pixelDensity * 0.5

                Text {
                    text: "Tools"
                }

                CheckBox {
                    text: "Inertial Rotation"
                    checked: settings.value("Developer", "inertialGlobeRotation") === "true"
                    onCheckedChanged: marbleMaps.inertialGlobeRotation = checked
                }

                CheckBox {
                    text: "GPS Simulation"
                    checked: settings.value("Developer", "positionProvider") === "RouteSimulationPositionProviderPlugin"
                    onCheckedChanged: marbleMaps.currentPositionProvider = checked ? "RouteSimulationPositionProviderPlugin" : "QtPositioning"
                }

                CheckBox {
                    id: runtimeTrace
                    text: "Render Performance"
                    checked: settings.value("Developer", "runtimeTrace") === "true"
                    onCheckedChanged: marbleMaps.setShowRuntimeTrace(checked)
                }

                CheckBox {
                    text: "Shell Output"
                    checked: settings.value("Developer", "debugOutput") === "true"
                    onCheckedChanged: settings.debugOutputEnabled = checked
                }
            }

            Column {
                spacing: Screen.pixelDensity * 0.5

                Text {
                    text: "Information"
                }

                CheckBox {
                    id: debugTags
                    text: "OSM Tags"
                    checked: settings.value("Developer", "debugTags") === "true"
                    onCheckedChanged: placemarkDialog.showTags = checked
                }

                CheckBox {
                    id: debugPlacemarks
                    text: "Placemarks"
                    checked: settings.value("Developer", "debugPlacemarks") === "true"
                    onCheckedChanged: marbleMaps.setShowDebugPlacemarks(checked)
                }

                CheckBox {
                    id: debugPolygons
                    text: "Polygons"
                    checked: settings.value("Developer", "debugPolygons") === "true"
                    onCheckedChanged: marbleMaps.setShowDebugPolygons(checked)
                }

                CheckBox {
                    id: debugBatches
                    text: "Batches"
                    checked: settings.value("Developer", "debugBatches") === "true"
                    onCheckedChanged: marbleMaps.setShowDebugBatches(checked)
                }
            }
        }
    }
}
