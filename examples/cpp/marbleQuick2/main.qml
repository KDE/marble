import QtQuick 2.0
import org.kde.marble 0.20

Rectangle {
    id: mainRect
    width: 600
    height: 600
    color: "transparent"
    MarbleItem
    {
        anchors.fill: parent
        id: marble
        visible: true
        focus: true
        PinchArea
        {
            anchors.fill: parent
            enabled: true
            objectName: "pinchArea"
            onPinchStarted: { marble.handlePinchStart(pinch.center) }
            onPinchUpdated: { marble.handlePinchUpdate(pinch.center, pinch.scale) }
            onPinchFinished:{ marble.handlePinchEnd(pinch.center, false) }
        }
        width: 600
        height: 600
        showFrameRate: false
        projection: MarbleItem.Spherical
        mapThemeId: "earth/openstreetmap/openstreetmap.dgml"
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
