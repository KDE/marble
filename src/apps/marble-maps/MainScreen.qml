import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import MarbleItem 1.0

ApplicationWindow {
    id: root
    title: qsTr("Marble Maps")
    visible: true

    menuBar: MenuBar {
        id: menuBar
        Menu {
            title: "Marble Maps"
            MenuItem{
                text: qsTr("Search")
            }
        }
    }

    toolBar: ToolBar {
        id: toolBar
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "black"
    }

    PinchArea {
        anchors.fill: parent
        enabled: true

        onPinchStarted: marbleMaps.handlePinchStarted(pinch.center)
        onPinchFinished: marbleMaps.handlePinchFinished(pinch.center)
        onPinchUpdated: marbleMaps.handlePinchUpdated(pinch.center, pinch.scale)

        MarbleItem {
            id: marbleMaps

            anchors.fill: parent
            visible: true
            focus: true

            // Theme settings.
            projection: MarbleItem.Mercator
            mapThemeId: "earth/openstreetmap/openstreetmap.dgml"

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
        }
    }
}
