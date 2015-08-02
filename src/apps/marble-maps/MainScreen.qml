import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.2

import org.kde.edu.marble 0.20

ApplicationWindow {
    id: root
    title: qsTr("Marble Maps")
    visible: true

    menuBar: MenuBar {
        id: menuBar
        Menu {
            title: qsTr("Marble Maps")
            MenuItem{
                text: qsTr("Search")
                onTriggered: search.visible = !search.visible
            }
        }
    }

    toolBar: ToolBar {
        id: toolBar
    }

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: palette.window
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
            positionProvider: "QtPositioning"
            showPositionMarker: true

            Routing {
                anchors.fill: parent
                marbleMap: marbleMaps.marbleMap
            }
        }

        MouseArea{
            anchors.fill: parent
            propagateComposedEvents: true
            onPressed: {
                search.focus = true;
                mouse.accepted = false;
            }
        }
    }

    Search {
        id: search
        anchors.fill: parent
        marbleQuickItem: marbleMaps
    }
}
