import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Item {
    id: root
    height: field.height

    property alias query: field.text

    property alias completionModel: completion.model

    signal searchRequested(string query)
    signal completionRequested(string query)

    function search(query) {
        query = query.trim();
        if(query !== "") {
            searchRequested(query);
            field.focus = false;
        }
    }

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        anchors.fill: parent
        color: palette.base
        border.color: palette.shadow
        border.width: 1
    }

    TextField {
        id: field
        anchors {
            left: parent.left
            right: searchButton.left
            margins: 5
        }
        placeholderText: qsTr("Search")
        font.pointSize: 18
        textColor: palette.text
        inputMethodHints: Qt.ImhNoPredictiveText
        onAccepted: root.search(text)
        onTextChanged: root.completionRequested(text)
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
                color: searchButton.pressed ? palette.highlight : palette.base
                Image {
                    anchors.fill: parent
                    source: searchButton.imageSource
                }
            }
        }
    }

    Completion {
        id: completion
        anchors {
            top: parent.bottom
            left: parent.left
            right: parent.right
        }
        height: delegateHeight * Math.min(2,count)
        visible: count > 0 && field.activeFocus
        onItemSelected: {
            field.text = name;
            search(name);
        }
    }
}
