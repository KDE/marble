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
            settings.setValue("Developer", "positionProvider", marbleMaps.currentPositionProvider)
            settings.setValue("Developer", "textureTiles", marbleMaps.isPropertyEnabled("mapnik") ? "true" : "false")
            settings.setValue("Developer", "runtimeTrace", runtimeTrace.checked ? "true" : "false")
            settings.setValue("Developer", "debugPolygons", debugPolygons.checked ? "true" : "false")
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

        Text {
            id: text
            text: "Developer Settings"
        }

        CheckBox {
            text: "Simulate GPS Position near Route"
            checked: settings.value("Developer", "positionProvider") === "RouteSimulationPositionProviderPlugin"
            onCheckedChanged: marbleMaps.currentPositionProvider = checked ? "RouteSimulationPositionProviderPlugin" : "QtPositioning"
        }

        CheckBox {
            text: "Show OSM Bitmap Tiles"
            checked: settings.value("Developer", "textureTiles") === "true"
            onCheckedChanged: marbleMaps.setMapThemeId(checked ? "earth/openstreetmap/openstreetmap.dgml" : "earth/vectorosm/vectorosm.dgml")
        }

        CheckBox {
            id: runtimeTrace
            text: "Show Render Performance"
            checked: settings.value("Developer", "runtimeTrace") === "true"
            onCheckedChanged: marbleMaps.setShowRuntimeTrace(checked)
        }

        CheckBox {
            id: debugPolygons
            text: "Render in Debug Mode"
            checked: settings.value("Developer", "debugPolygons") === "true"
            onCheckedChanged: {
                marbleMaps.setShowDebugPlacemarks(checked)
                marbleMaps.setShowDebugPolygons(checked)
            }
        }
    }
}
