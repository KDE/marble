//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//
import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.0 as Kirigami
import org.kde.marble 0.20

Kirigami.ApplicationWindow {
    id: root
    Layout.fillWidth: true


    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    globalDrawer: Kirigami.GlobalDrawer {
        id: drawer
        title: "Settings"
        handleVisible: !aboutDrawer.drawerOpen && !infoDrawer.drawerOpen
        topContent: [
            CheckBox {
                text: qsTr("Show Behaim places")
                onCheckedChanged: marbleMaps.setPropertyEnabled("cities", checked)
            },
            CheckBox {
                text: qsTr("Show texts and illustrations")
                onCheckedChanged: marbleMaps.setPropertyEnabled("otherplaces", checked)
            },
            CheckBox {
                text: qsTr("Show the accurate coastline")
                onCheckedChanged: marbleMaps.setPropertyEnabled("coastlines", checked)
            },
            Label {
                wrapMode: Text.WordWrap
                text: qsTr("<h4>Globe Variant</h4>")
                Layout.fillWidth: true
            },
            ExclusiveGroup {
                id: layerGroup
                onCurrentChanged: current.apply()
            },
            RadioButton {
                text: qsTr("Original (1492)")
                checked: true
                exclusiveGroup: layerGroup
                property string description: qsTr("Digital imagery taken directly from the original Behaim globe.")
                function apply() {
                    marbleMaps.setPropertyEnabled("ravenstein", false)
                    marbleMaps.setPropertyEnabled("ghillany", false)
                }
            },

            RadioButton {
                text: qsTr("Ghillany (1853)")
                property string description: qsTr("A (rough) facsimile created by Friedrich Wilhelm Ghillany in 1853.")
                exclusiveGroup: layerGroup

                function apply() {
                    marbleMaps.setPropertyEnabled("ravenstein", false)
                    marbleMaps.setPropertyEnabled("ghillany", true)
                }
            },

            RadioButton {
                text: qsTr("Ravenstein (1908)")
                property string description: qsTr("A (rough) facsimile created by Ernest George Ravenstein in 1908.")
                exclusiveGroup: layerGroup
                function apply() {
                    marbleMaps.setPropertyEnabled("ghillany", false)
                    marbleMaps.setPropertyEnabled("ravenstein", true)
                }
            },
            Item { width: 1; height: Screen.pixelDensity * 2; }
        ]
        actions: [
            Kirigami.Action {
                text: qsTr("Information")
                onTriggered: {
                    infoDrawer.open()
                    if(aboutDrawer.drawerOpen){
                        aboutDrawer.close()
                    }
                }
            },
            Kirigami.Action {
                text: qsTr("About")
                iconSource: "menu.png"
                onTriggered: {
                    aboutDrawer.open()
                    if(infoDrawer.drawerOpen){
                        infoDrawer.close()
                    }
                }
            }
        ]
    }

    Kirigami.Page {
        id: page
        property alias marbleMaps: mainLayout.marbleMaps
        anchors.fill: parent
        title: qsTr("Behaim Globe")
        visible: true

        property bool landscape: root.width > root.height

        Rectangle {
            id: background
            anchors.fill: parent
            color: palette.base
        }

        Grid {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            id: mainLayout
            property alias marbleMaps: mapItem.marbleMaps
            columns: root.landscape ? 2 : 1
            columnSpacing: 0
            rows: root.landscape ? 1 : 2
            rowSpacing: 0
            layoutDirection: root.landscape ? Qt.RightToLeft : Qt.LeftToRight

            Item {
                id: mapItem
                property alias marbleMaps: pinchArea.marbleItem
                width: root.width
                height: root.height

                Rectangle {
                    color: "black"
                    anchors.fill: parent
                }

                PinchArea {
                    id: pinchArea
                    anchors.fill: parent
                    enabled: true
                    property alias marbleItem: marbleMaps
                    onPinchStarted: marbleMaps.handlePinchStarted(pinch.center)
                    onPinchFinished: marbleMaps.handlePinchFinished(pinch.center)
                    onPinchUpdated: marbleMaps.handlePinchUpdated(pinch.center, pinch.scale);

                    MarbleItem {
                        id: marbleMaps
                        anchors.fill: parent

                        focus: true
                        zoom: 1150
                        inertialGlobeRotation: true

                        // Theme settings.
                        projection: MarbleItem.Spherical
                        mapThemeId: "earth/behaim1492/behaim1492.dgml"

                        // Visibility of layers/plugins.
                        showFrameRate: false
                        showAtmosphere: true
                        showCompass: false
                        showClouds: false
                        showCrosshairs: false
                        showGrid: false
                        showOverviewMap: false
                        showOtherPlaces: false
                        showScaleBar: false
                        showBackground: true
                        showPositionMarker: false

                        Component.onCompleted: {
                            setPluginSetting("stars", "renderConstellationLines", "false");
                            setPluginSetting("stars", "renderConstellationLabels", "false");
                            setPluginSetting("stars", "renderDsoLabels", "false");
                            setPluginSetting("stars", "viewSolarSystemLabel", "false");
                            setPluginSetting("stars", "zoomSunMoon", "false");
                            setPluginSetting("stars", "renderEcliptic", "false");
                            setPluginSetting("stars", "renderCelestialEquator", "false");
                            setPluginSetting("stars", "renderCelestialPole", "false");
                            marbleMaps.forceActiveFocus()
                        }

                    }
                }
            }
        }

        Kirigami.OverlayDrawer {
            id: infoDrawer
            edge: Qt.BottomEdge

            contentItem: Item {
                Layout.fillWidth: true
                implicitHeight: columnLayoutInfo.text.height + Kirigami.Units.gridUnit
                ColumnLayout {
                    id: columnLayoutInfo
                    property alias text: flickableInfo.text
                    anchors.fill: parent
                    Flickable {
                        id: flickableInfo
                        property alias text: infoText
                        anchors.fill: parent
                        contentWidth: infoText.width
                        contentHeight: infoText.height
                        flickableDirection: Flickable.VerticalFlick
                        clip: true

                        Text {
                            id: infoText
                            text: qsTr("<h3>Martin Behaim's Erdapfel</h3>\
                               <p>The oldest existent globe of the Earth.\
                               Martin Behaim and collaborators created the globe around 1492 at the time of \
                               <a href=\"https://en.wikipedia.org/wiki/Voyages_of_Christopher_Columbus\">Columbus'</a> first sea travel to the west.\
                               Hence the American continent is missing on this globe.\
                               Also note the detailed inscriptions in early modern German.</p>\
                               <p>Please see <a href=\"https://en.wikipedia.org/wiki/Erdapfel\">Wikipedia: Erdapfel</a> \
                               for further information about the Behaim globe.")
                            width: columnLayoutInfo.width
                            wrapMode: Text.WordWrap
                            onLinkActivated: Qt.openUrlExternally(link)
                        }
                    }
                    Item {
                        Layout.minimumHeight: Kirigami.Units.gridUnit * 4
                    }
                }
            }
        }

        Kirigami.OverlayDrawer {
            id: aboutDrawer
            edge: Qt.BottomEdge
            contentItem: Item {
                Layout.fillWidth: true
                implicitHeight: columnLayoutInfo.text.height + Kirigami.Units.gridUnit

                ColumnLayout {
                    id: columnLayoutAbout
                    property alias text: flickableAbout.text
                    anchors.fill: parent
                    Flickable {
                        id: flickableAbout
                        property alias text: aboutText
                        anchors.fill: parent
                        contentWidth: aboutText.width
                        contentHeight: aboutText.height
                        flickableDirection: Flickable.VerticalFlick
                        clip: true
                        Text {
                            id: aboutText
                            anchors {
                                margins: Screen.pixelDensity * 2
                                topMargin: Screen.pixelDensity * 14
                            }
                            text: qsTr("<h3>Germanisches Nationalmuseum</h3>\
                           <p>The original Behaim globe can be visited in the
                           <a href=\"http://www.gnm.de/\">Germanisches Nationalmuseum</a> in Nuremberg, Germany.</p>\
                           <h3>KDE Marble</h3>\
                           <p>This app is part of the <a href=\"http://marble.kde.org\">Marble</a> project.\
                           The Marble community works on maps and virtual globes with the goal to produce visually appealing, easy-to-use Free Software.</p>\
                           <h3>Map Content</h3>\
                           <p>Digitized map based on orthophotographic gores by TU Vienna, 1990. Germanisches Nationalmuseum and\
                           Friedrich-Alexander-Universität Erlangen-Nürnberg, CC BY-SA 3.0.\
                           Ghillany map based on two planiglobes which are provided as a map supplement\
                           to F.W. Ghillany's \"Geschichte des Seefahrers Ritter Martin Behaim nach den ältesten vorhandenen Urkunden\",\
                           Nuremberg 1853. CC BY-SA 3.0.</p>")
                            width: columnLayoutAbout.width
                            wrapMode: Text.WordWrap
                            onLinkActivated: Qt.openUrlExternally(link)
                        }
                    }

                }
            }
        }
    }
}


