import QtQuick 2.3
import MarbleItem 1.0

Rectangle {
    id: mainRect
    anchors.fill: parent
    color: "black"

    MarbleItem
    {
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
