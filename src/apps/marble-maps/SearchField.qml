import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    id: root
    height: field.height

    property alias query: field.text

    signal searchRequested(string query)

    function search(query) {
        query = query.trim();
        if(query !== "") {
            searchRequested(query);
        }
    }

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        anchors.fill: parent
        color: palette.window
    }

    TextField {
        id: field
        anchors {
            left: parent.left
            right: searchButton.left
            margins: 10
        }
        placeholderText: qsTr("Search")
        font.pointSize: 18
        textColor: palette.text
        inputMethodHints: Qt.ImhNoPredictiveText
        onAccepted: root.search(text)
    }

    Button {
        id: searchButton
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            margins: 10
        }
        width: height

        property string imageSource: "qrc:///search.png"

        onClicked: root.search(field.text)

        style: ButtonStyle {
            background: Rectangle {
                anchors.fill: parent
                color: searchButton.pressed ? palette.highlight : palette.window
                Image {
                    anchors.fill: parent
                    source: searchButton.imageSource
                }
            }
        }
    }
}
