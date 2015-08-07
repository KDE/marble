import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Button {
    id: root

    property string imageSource

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    style: ButtonStyle {
        background: Rectangle {
            anchors.fill: parent
            color: root.pressed ? palette.highlight : palette.base
            Image {
                id: icon
                anchors.fill: parent
                source: root.imageSource
            }
        }
    }
}
