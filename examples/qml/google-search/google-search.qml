import Qt 4.7
import org.kde.edu.marble 0.11

Rectangle {
    width: 700
    height: 700

    SystemPalette { id: activePalette }

    Image {
        x: 113
        y: 0
        source: "marble.png"
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Rectangle {
        id: searchinput
        x: 38
        y: 117
        width: 565
        height: 49
        border.color: "black"
        border.width: 1
        anchors.horizontalCenter: parent.horizontalCenter

        TextInput {
            anchors.fill: parent
            anchors.margins: 10
            id: searchterm
            text: ""
            font.pointSize: 18
            focus: true
        }
    }

    Item {
        id: buttonlayouter
        anchors.top: searchinput.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 10
        height: button1.height

        Button {
            id: button1
            anchors.right: buttonlayouter.horizontalCenter
            anchors.margins: 10
            width: 126
            height: 25
            label: "Marble Search"

            onClicked: { map.search.find( searchterm.text ) }
        }

        Button {
            id: button2
            anchors.left: buttonlayouter.horizontalCenter
            anchors.margins: 10
            width: 140
            height: 25
            label: "I'm Feeling Lucky"
        }
    }

    Item {
        id: mapcontainer
        width: 600
        height: 400
        anchors.horizontalCenter: buttonlayouter.horizontalCenter
        anchors.top: buttonlayouter.bottom
        anchors.margins: 30
        clip: true

        MarbleWidget {
            id: map
            width: 600
            height: 400
            activeRenderPlugins: [ "navigation", "scalebar" ]

            search {
                placemarkDelegate: myDelegate
            }
        }

        Component {
            id: myDelegate

            Image {
                source: "marker.svg"
                fillMode: Image.PreserveAspectFit
                width: 64;
                height: 64

                property bool showDetails: false

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: parent.height
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    color: "white"
                    text: hit
                    horizontalAlignment: Text.AlignHCenter
                }

                Rectangle
                {
                    anchors.left: parent.right
                    id: itemdetails
                    scale: 0.75
                    width: 140
                    height: 60
                    color: "yellow"
                    radius: 10
                    border.width: 1
                    border.color: "gray"
                    z: 42

                    visible: parent.showDetails

                    Text {
                        id: itemdetailtext
                        x: 10
                        y: 5
                        width: parent.width - 20
                        height: parent.height - 10
                        text: name
                        wrapMode: "WrapAtWordBoundaryOrAnywhere"
                        clip: true
                    }

                    states: State {
                        name: "back"
                        PropertyChanges { target: itemdetails; scale: 1 }
                        when: itemdetailtext.visible
                    }

                    transitions: Transition {
                        NumberAnimation { properties: "scale"; duration: 100 }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: showDetails = !showDetails
                }
            }
        }
    }
}
