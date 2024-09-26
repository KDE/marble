// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//
import QtQuick
import QtQuick.Controls as Controls
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

            Controls.ToolBar {
                Layout.fillWidth: true
                Layout.preferredHeight: root.pageStack.globalToolBar.preferredHeight

                leftPadding: Kirigami.Units.largeSpacing
                rightPadding: Kirigami.Units.largeSpacing
                topPadding: Kirigami.Units.smallSpacing
                bottomPadding: Kirigami.Units.smallSpacing

                contentItem: Kirigami.Heading {
                    text: i18nc("@title", "Marble")
                }
            }

            Kirigami.ScrollablePage {
                Layout.fillWidth: true
                Layout.fillHeight: true

                leftPadding: 0
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0

                Controls.ButtonGroup {
                    id: mapGroup
                }

                ColumnLayout {
                    spacing: 0

                    FormCard.FormHeader {
                        title: i18nc("@title:group", "Globe Settings")
                        Layout.fillWidth: true
                    }

                    FormCard.FormCheckDelegate {
                        text: i18nc("@option:check", "Show Behaim places")
                        onCheckedChanged: marbleMaps.setPropertyEnabled("cities", checked)
                    }

                    FormCard.FormDelegateSeparator {}

                    FormCard.FormCheckDelegate {
                        text: i18nc("@option:check", "Show texts and illustrations")
                        onCheckedChanged: marbleMaps.setPropertyEnabled("otherplaces", checked)
                    }

                    FormCard.FormDelegateSeparator {}

                    FormCard.FormCheckDelegate {
                        text: i18nc("@option:check", "Show the accurate coastline")
                        onCheckedChanged: marbleMaps.setPropertyEnabled("coastlines", checked)
                    }

                    FormCard.FormHeader {
                        title: i18nc("@title:group", "Globe Variant")
                        Layout.fillWidth: true
                    }

                    FormCard.FormRadioDelegate {
                        Controls.ButtonGroup.group: mapGroup

                        text: i18nc("@option:radio", "Original (1492)")
                        checked: true
                        description: i18n("Digital imagery taken directly from the original Behaim globe.")

                        onToggled: if (checked) {
                            marbleMaps.setPropertyEnabled("ravenstein", false)
                            marbleMaps.setPropertyEnabled("ghillany", false)
                        }
                    }

                    FormCard.FormRadioDelegate {
                        Controls.ButtonGroup.group: mapGroup

                        text: i18nc("@option:radio", "Ghillany (1853)")
                        description: i18n("A (rough) facsimile created by Friedrich Wilhelm Ghillany in 1853.")

                        onToggled: if (checked) {
                            marbleMaps.setPropertyEnabled("ravenstein", false)
                            marbleMaps.setPropertyEnabled("ghillany", true)
                        }
                    }

                    FormCard.FormRadioDelegate {
                        Controls.ButtonGroup.group: mapGroup

                        text: i18nc("@option:radio", "Ravenstein (1908)")
                        description: i18n("A (rough) facsimile created by Ernest George Ravenstein in 1908.")

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

        title: mapGroup.checkedButton?.text ?? i18nc("@title:window", "Behaim Globe")

        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        actions: Kirigami.Action {
            text: i18nc("@action:button", "Information")
            onTriggered: {
                root.pageStack.pushDialogLayer(Qt.createComponent('org.kde.marble.behaim', 'About'));
            }
            icon.name: 'documentinfo-symbolic'
        }

        PinchArea {
            id: pinchArea

            anchors.fill: parent
            enabled: true
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
