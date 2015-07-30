import QtQuick 2.3
import QtQuick.Controls 1.3

Item {
    id: root

    property var model: []

    property color background: palette.window

    property alias count: view.count

    property int delegateHeight: 0

    signal itemSelected(int index, string name)

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: root.background
    }

    ListView {
        id: view
        anchors.fill: parent
        snapMode: ListView.SnapToItem
        model: root.model
        delegate: Item {
            width: view.width
            height: placemarkName.height + 20

            Rectangle {
                id: delegateBackground
                anchors.fill: parent
                color: mouseArea.pressed ? palette.highlight : root.background
            }

            Text {
                id: placemarkName
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                font.pointSize: 18
                color: palette.text
                text: name
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    root.itemSelected(index, name);
                }
            }

            Component.onCompleted: {
                if( root.delegateHeight != height ) {
                    root.delegateHeight = height;
                }
            }
        }
    }
}
