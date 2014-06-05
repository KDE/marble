import QtQuick 2.0
import MarbleItem 1.0

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
    }
}
