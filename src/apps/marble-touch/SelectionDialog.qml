import QtQuick 2.3
import org.kde.edu.marble 0.20
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

Item {
    id: root
    anchors.fill: parent
    visible: false

    property alias titleText: title.text
    property alias model: listView.model
    property alias currentIndex: listView.currentIndex
    property alias delegate: listView.delegate

    signal accepted()

    function open() {
        root.visible = true
    }

    Rectangle {
        anchors.fill: parent
        color: "white"
    }

    Label {
        id: title
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5
        font.pointSize: 22
    }

    ListView {
        id: listView
        anchors.top: title.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: button.top

        clip: true
    }

    Button {
        id: button
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        text: "OK"
        onClicked: {
            root.visible = false
            root.accepted()
        }
    }
}
