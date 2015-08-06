import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2

Item {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true

    property alias imageSource: image.source
    property alias text: label.text
    signal clicked()

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        color: touchArea.pressed ? palette.highlight : palette.button

        anchors.fill: parent
        border {
            color: palette.shadow
            width: Screen.pixelDensity * 0.1
        }

        Item {
            width: label.width
            height: label.height * 2 + image.anchors.topMargin

            anchors {
                centerIn: parent
            }

            Text {
                id: label
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                color: palette.text
            }

            Image {
                id: image
                anchors {
                    top: label.bottom
                    horizontalCenter: label.horizontalCenter
                    topMargin: Screen.pixelDensity * 0.5
                }

                width: label.height
                height: label.height
            }
        }

        MouseArea {
            id: touchArea
            anchors.fill: parent
            onClicked: root.clicked();
        }
    }
}
