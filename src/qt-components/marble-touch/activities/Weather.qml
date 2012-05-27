// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
import com.nokia.meego 1.0
import org.kde.edu.marble 0.11
import org.kde.edu.marble.qtcomponents 0.12

/*
 * Page for the weather activity.
 */
Page {
    id: weatherActivityPage
    anchors.fill: parent

    tools: ToolBarLayout {
        MarbleToolIcon {
            iconSource: main.icon( "actions/go-previous-view", 48 );
            onClicked: pageStack.pop()
        }
        MarbleToolIcon {
            iconSource: main.icon( "places/user-identity", 48 );
            onClicked: {
                marbleWidget.centerOn( marbleWidget.tracking.lastKnownPosition.longitude, marbleWidget.tracking.lastKnownPosition.latitude )
                if (marbleWidget.zoom < 403 ) {
                    marbleWidget.zoom = 22026
                }
            }
        }
        ToolButton {
            id: searchButton
            checkable: true
            checked: true
            width: 60
            flat: true
            iconSource: main.icon( "actions/edit-find", 48 );
        }
    }

    SearchField {
        id: searchField
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        visible: searchButton.checked
        onSearch: {
            searchField.busy = true
            marbleWidget.find( term )
        }

        Component.onCompleted: {
            marbleWidget.search.searchFinished.connect( searchFinished )
        }

        function searchFinished() {
            searchField.busy = false
        }
    }

    Item {
        id: mapContainer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: searchButton.checked ? searchField.bottom : parent.top
        anchors.bottom: parent.bottom
        clip: true

        function embedMarbleWidget() {
            marbleWidget.parent = mapContainer
            settings.projection = "Spherical"
            var plugins = settings.defaultRenderPlugins
            plugins.push( "weather" )
            settings.activeRenderPlugins =  plugins
            settings.mapTheme = settings.streetMapTheme
            settings.gpsTracking = true
            settings.showPositionIndicator = false
            settings.showTrack = false
            marbleWidget.tracking.positionMarkerType = Tracking.Circle
            marbleWidget.visible = true
            marbleWidget.setDataPluginDelegate( "weather", weatherDelegate )
        }

        Component.onDestruction: {
            if ( marbleWidget.parent === mapContainer ) {
                marbleWidget.setDataPluginDelegate( "weather", 0 )
                marbleWidget.parent = null
                marbleWidget.visible = false
            }
        }
    }

    onStatusChanged: {
        if ( status === PageStatus.Activating ) {
            mapContainer.embedMarbleWidget()
        }
    }

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 30
        anchors.leftMargin: 15
        width: legend.width
        height: legend.height
        radius: 5
        color: Qt.rgba(192/255, 192/255, 192/255, 192/255)
        border.width: 1
        border.color: "black"
        smooth: true

        Column {
            id: legend
            x: 6
            y: 6
            width: 60
            height: items*(10+spacing)+2*x
            property int items: 22
            spacing: 2

            Repeater {
                id: repeater
                model: parent.items

                Row {
                    spacing: 4
                    height: 10
                    property int temperature: -3*index+36

                    Rectangle {
                        anchors.verticalCenter: parent.verticalCenter
                        width: 10
                        height: 10
                        radius: 3
                        smooth: true
                        color: Qt.hsla( weatherActivityPage.temperatureToHue(parent.temperature)/255.0, 1.0, 0.5, 1.0 )
                    }

                    Text {
                        text: parent.temperature + " °C"
                        width: 34
                        horizontalAlignment: Text.AlignRight
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }

    Component {
        id: weatherDelegate
        Item {
            id: containerItem
            width: Math.max(icon.width, label.width)
            height: Math.max(icon.height, label.height)
            clip: true
            property bool showDetails: false

            Image {
                id: icon
                z: 0
                width: 64
                scale: 0.0
                smooth: true
                fillMode: Image.PreserveAspectFit
                source: image !== "" ? ("file://" + image) : ""

                MouseArea {
                    anchors.fill: parent
                    onClicked: containerItem.showDetails = true
                }
            }

            Text {
                id: label
                z: 0
                visible: !icon.visible
                anchors.centerIn: parent
                font.pixelSize: 24
                style: Text.Outline;
                font.bold: true
                color: Qt.hsla( weatherActivityPage.temperatureToHue(temperature)/255.0, 1.0, 0.5, 1.0 )
                styleColor: "black"
                text: temperature.toFixed(0) + " °C"

                MouseArea {
                    anchors.fill: parent
                    onClicked: containerItem.showDetails = true
                }
            }

            SequentialAnimation {
                id: appearAnimation
                PauseAnimation { duration: Math.random() * 100 }
                NumberAnimation {
                    target: icon
                    property: "scale"
                    to: 1.0
                    duration: 150
                }
            }

            Component.onCompleted: {
                icon.visible = image !== "" && Math.random() >= 0.3
                appearAnimation.running = true
            }
        }
    }

    function temperatureToHue(temperature) {
        var factor = temperature > 20 ? 1.8 : ( temperature > 5 ? 2.1 : 2.7 )
        return -factor*(Math.max(-40, Math.min(40, temperature))-40)
    }
}
