// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//
import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.marble

Kirigami.ApplicationWindow {
    id: root

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    globalDrawer: Kirigami.OverlayDrawer {
        id: drawer

        edge: Qt.application.layoutDirection === Qt.RightToLeft ? Qt.RightEdge : Qt.LeftEdge
        modal: Kirigami.Settings.isMobile || (applicationWindow().width < Kirigami.Units.gridUnit * 50 && !collapsed) // Only modal when not collapsed, otherwise collapsed won't show.
        z: modal ? Math.round(position * 10000000) : 100
        drawerOpen: !Kirigami.Settings.isMobile && enabled
        enabled: pageStack.currentItem
        onEnabledChanged: drawerOpen = !Kirigami.Settings.isMobile && enabled
        width: Kirigami.Units.gridUnit * 16
        Behavior on width {
            NumberAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutQuad
            }
        }

        handleClosedIcon.source: modal ? null : "sidebar-expand-left"
        handleOpenIcon.source: modal ? null : "sidebar-collapse-left"
        handleVisible: modal
        onModalChanged: if (!modal) {
            drawerOpen = true;
        }

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        contentItem: ColumnLayout {
            spacing: 0

            ToolBar {
                Layout.fillWidth: true
                Layout.preferredHeight: root.pageStack.globalToolBar.preferredHeight

                leftPadding: Kirigami.Units.largeSpacing
                rightPadding: Kirigami.Units.largeSpacing
                topPadding: Kirigami.Units.smallSpacing
                bottomPadding: Kirigami.Units.smallSpacing

                contentItem: Kirigami.Heading {
                    text: qsTr("Settings")
                }
            }

            Kirigami.ScrollablePage {
                Layout.fillWidth: true
                Layout.fillHeight: true

                leftPadding: 0
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0

                ColumnLayout {
                    spacing: 0

                    FormCard.FormCheckDelegate {
                        text: qsTr("Show Behaim places")
                        onCheckedChanged: marbleMaps.setPropertyEnabled("cities", checked)
                    }

                    FormCard.FormDelegateSeparator {}

                    FormCard.FormCheckDelegate {
                        text: qsTr("Show texts and illustrations")
                        onCheckedChanged: marbleMaps.setPropertyEnabled("otherplaces", checked)
                    }

                    FormCard.FormDelegateSeparator {}

                    FormCard.FormCheckDelegate {
                        text: qsTr("Show the accurate coastline")
                        onCheckedChanged: marbleMaps.setPropertyEnabled("coastlines", checked)
                    }

                    FormCard.FormHeader {
                        title: qsTr("Globe Variant")
                        Layout.fillWidth: true
                    }

                    FormCard.FormRadioDelegate {
                        text: qsTr("Original (1492)")
                        checked: true
                        description: checked ? qsTr("Digital imagery taken directly from the original Behaim globe.") : ''

                        onToggled: if (checked) {
                            marbleMaps.setPropertyEnabled("ravenstein", false)
                            marbleMaps.setPropertyEnabled("ghillany", false)
                        }
                    }

                    FormCard.FormRadioDelegate {
                        text: qsTr("Ghillany (1853)")
                        description: checked ? qsTr("A (rough) facsimile created by Friedrich Wilhelm Ghillany in 1853.") : ''

                        onToggled: if (checked) {
                            marbleMaps.setPropertyEnabled("ravenstein", false)
                            marbleMaps.setPropertyEnabled("ghillany", true)
                        }
                    }

                    FormCard.FormRadioDelegate {
                        text: qsTr("Ravenstein (1908)")
                        description: checked ? qsTr("A (rough) facsimile created by Ernest George Ravenstein in 1908.") : ''

                        onToggled: if (checked) {
                            marbleMaps.setPropertyEnabled("ghillany", false)
                            marbleMaps.setPropertyEnabled("ravenstein", true)
                        }
                    }
                }
            }
        }
    }

    pageStack.initialPage: Kirigami.Page {
        id: page

        property alias marbleMaps: mainLayout.marbleMaps
        property bool landscape: root.width > root.height

        title: qsTr("Behaim Globe")

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

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
                icon.name: 'menu'
                onTriggered: {
                    aboutDrawer.open()
                    if(infoDrawer.drawerOpen){
                        infoDrawer.close()
                    }
                }
            }
        ]

        Grid {
            id: mainLayout

            property alias marbleMaps: mapItem.marbleMaps

            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }

            columns: root.landscape ? 2 : 1
            columnSpacing: 0
            rows: root.landscape ? 1 : 2
            rowSpacing: 0
            layoutDirection: root.landscape ? Qt.RightToLeft : Qt.LeftToRight

            Item {
                id: mapItem

                property alias marbleMaps: pinchArea.marbleItem
                width: page.availableWidth
                height: page.availableHeight

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
                        Layout.fillWidth: true
                        Layout.fillHeight: true
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
                        Layout.fillWidth: true
                        Layout.fillHeight: true
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
                           <a href=\"https://www.gnm.de/\">Germanisches Nationalmuseum</a> in Nuremberg, Germany.</p>\
                           <h3>KDE Marble</h3>\
                           <p>This app is part of the <a href=\"https://marble.kde.org\">Marble</a> project.\
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
