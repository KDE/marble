import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2

Item {
    id: root

    signal routeToDestinationRequested()
    signal routeFromDepartureRequested()
    signal routeThroughWaypointRequested()
    signal profileSelected(string profile)

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: palette.base
        border {
            width: Screen.pixelDensity * 0.2
            color: palette.shadow
        }

        RowLayout {
            id: typeSelector
            anchors.fill: parent
            spacing: 0
            visible: true

            NavigationSetupButton {
                imageSource: "qrc:///navigation.png"
                text: qsTr("As destination")
                onClicked: { routeToDestinationRequested(); typeSelector.nextScreen() }
            }

            NavigationSetupButton {
                imageSource: "qrc:///waypoint.png"
                text: qsTr("As waypoint")
                onClicked: { routeThroughWaypointRequested();  }
            }

            NavigationSetupButton {
                imageSource: "qrc:///map.png"
                text: qsTr("As departure")
                onClicked: { routeFromDepartureRequested();  }
            }

            function nextScreen()
            {
                profileSelector.visible = true;
                typeSelector.visible = false;
            }
        }

        RowLayout {
            id: profileSelector
            anchors.fill: parent
            spacing: 0
            visible: false

            NavigationSetupButton {
                imageSource: "qrc:///car.png"
                text: qsTr("With car")
                onClicked: { profileSelected("Car (fastest)"); profileSelector.previousScreen() }
            }

            NavigationSetupButton {
                imageSource: "qrc:///bike.png"
                text: qsTr("With bike")
                onClicked: { profileSelected("Bicycle"); profileSelector.previousScreen() }
            }

            NavigationSetupButton {
                imageSource: "qrc:///walk.png"
                text: qsTr("Walking")
                onClicked: { profileSelected("Pedestrian"); profileSelector.previousScreen() }
            }

            function previousScreen()
            {
                typeSelector.visible = true;
                profileSelector.visible = false;
            }
        }
   }
}
