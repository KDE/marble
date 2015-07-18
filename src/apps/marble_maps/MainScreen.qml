import QtQuick 2.3
import MarbleItem 1.0

Rectangle {
    id: mainRect
    anchors.fill: parent
    color: "black"

    MarbleItem
    {
        anchors.fill: parent
        id: marbleMaps
        visible: true
        focus: true
    }
}
