import QtQuick 2.3
import QtQuick.Window 2.2

Item {
    id: root
    width: 1.1 * label.width
    height: 1.1 * label.height
    property alias color: background.color
    property alias textColor: label.color
    property alias text: label.text

    Rectangle {
        id: background
        anchors.fill: parent
        radius: Screen.pixelDensity*0.5
        color: "black"
        opacity: 0.63
    }

    Text {
        id: label
        color: "white"
        anchors {
            horizontalCenter: background.horizontalCenter
            verticalCenter: background.verticalCenter
        }
    }
}
