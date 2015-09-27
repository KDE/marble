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
import QtGraphicalEffects 1.0

Item {
    id: root

    Rectangle {
        id: background
        anchors.fill: parent
        color: palette.base
    }

    DropShadow {
        anchors.fill: root
        horizontalOffset: landscape ? 4 : 0
        verticalOffset: landscape ? 0 : -4
        radius: 4.0
        samples: 16
        color: "#666"
        cached: true
        fast: true
        source: background
        transparentBorder: true
    }

    TabView {
        id: tabView
        anchors.fill: parent
        tabPosition: Qt.BottomEdge

        Tab {
            id: infoTab
            anchors.margins: Screen.pixelDensity * 2
            title: "Info"

            Flickable {
                anchors.fill: parent
                contentWidth: infoText.width
                contentHeight: infoText.height
                flickableDirection: Flickable.VerticalFlick
                clip: true

                Text {
                    id: infoText
                    text: "<h3>Martin Behaim's Erdapfel</h3>\
                       <p>The oldest existent globe of the Earth.<br/>\
                       Martin Behaim and collaborators created the globe at the time of Columbus' first sea travel to the west.\
                       Hence the American continent is missing on this globe.\
                       Also note the detailed inscriptions in early modern German.</p>"
                    width: infoTab.width
                    wrapMode: Text.WordWrap
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }

        }
        Tab {
            id: variantsTab
            anchors.margins: Screen.pixelDensity * 2
            title: "Variants"

            Flickable {
                anchors.fill: parent
                contentWidth: themeColumn.width
                contentHeight: themeColumn.height
                flickableDirection: Flickable.VerticalFlick
                clip: true

                Column {
                    id: themeColumn
                    width: variantsTab.width
                    anchors {
                        margins: Screen.pixelDensity * 2
                        topMargin: Screen.pixelDensity * 14
                    }

                    Text {
                        wrapMode: Text.Wrap
                        text: "<h3>Globe Variant</h3>"
                    }

                    ExclusiveGroup {
                        id: layerGroup
                        onCurrentChanged: current.apply()
                    }
                    RadioButton {
                        text: "Original (1492)"
                        checked: true
                        exclusiveGroup: layerGroup
                        property string description: "Digital imagery taken directly from the original Behaim globe."
                        function apply() {
                            marbleMaps.setPropertyEnabled("ravenstein", false)
                            marbleMaps.setPropertyEnabled("ghillany", false)
                        }
                    }

                    RadioButton {
                        text: "Ghillany (1853)"
                        property string description: "A (rough) facsimile created by Friedrich Wilhelm Ghillany in 1853."
                        exclusiveGroup: layerGroup

                        function apply() {
                            marbleMaps.setPropertyEnabled("ravenstein", false)
                            marbleMaps.setPropertyEnabled("ghillany", true)
                        }
                    }

                    RadioButton {
                        text: "Ravenstein (1908)"
                        property string description: "A (rough) facsimile created by Ernest George Ravenstein in 1908."
                        exclusiveGroup: layerGroup
                        function apply() {
                            marbleMaps.setPropertyEnabled("ghillany", false)
                            marbleMaps.setPropertyEnabled("ravenstein", true)
                        }
                    }

                    Item { width: 1; height: Screen.pixelDensity * 2; }

                    Text {
                        text: layerGroup.current.description
                        width: parent.width
                        wrapMode: Text.Wrap
                    }
                }
            }
        }

        Tab {
            anchors.margins: Screen.pixelDensity * 2
            title: "Settings"

            Flickable {
                anchors.fill: parent
                contentWidth: settingsColumn.width
                contentHeight: settingsColumn.height
                flickableDirection: Flickable.VerticalFlick
                clip: true

                Column {
                    id: settingsColumn
                    anchors {
                        margins: Screen.pixelDensity * 2
                        topMargin: Screen.pixelDensity * 14
                    }

                    Text {
                        wrapMode: Text.Wrap
                        text: "<h3>Globe Settings</h3>"
                    }

                    CheckBox {
                        text: "Show Behaim places"
                        onCheckedChanged: marbleMaps.setPropertyEnabled("cities", checked)
                    }
                    CheckBox {
                        text: "Show texts and illustrations"
                        onCheckedChanged: marbleMaps.setPropertyEnabled("otherplaces", checked)

                    }
                    CheckBox {
                        text: "Show the accurate coastline"
                        onCheckedChanged: marbleMaps.setPropertyEnabled("coastlines", checked)
                    }
                }
            }
        }

        Tab {
            id: aboutTab
            anchors.margins: Screen.pixelDensity * 2
            title: "About"

            Flickable {
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

                    text: "<h3>Germanisches Nationalmuseum</h3>\
                   <p>The original Behaim globe can be visited in the
                   <a href=\"http://www.gnm.de/\">Germanisches Nationalmuseum</a> in Nuremberg, Germany.\
                   It was digitized in an elaborate process over several years and made available to the public under the CC-BY-SA license.</p>\
                   <h3>KDE Marble</h3>\
                   <p>This app is part of the <a href=\"http://marble.kde.org\">Marble</a> Project.\
                   The Marble community works on maps and virtual globes with the goal to produce visually appealing, easy-to-use Free Software.</p>"
                    width: aboutTab.width
                    wrapMode: Text.WordWrap
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }
        }
    }
}
