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
import QtQuick.Controls 2.0
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.0

import org.kde.marble 0.20

Item {
    id: root
    height: swipeView.height + Screen.pixelDensity * 4

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    Settings {
        id: settings

        Component.onDestruction: {
            settings.setValue("MarbleMaps", "mapThemeId", marbleMaps.mapThemeId)
            settings.setValue("localization", "translationsDisabled", ignoreTranslations.checked ? "true" : "false")
            settings.setValue("localization", "translationFile", localizationItem.translationFile)
            settings.setValue("Developer", "inertialGlobeRotation", marbleMaps.inertialGlobeRotation)
            settings.setValue("Developer", "positionProvider", marbleMaps.currentPositionProvider)
            settings.setValue("Developer", "runtimeTrace", runtimeTrace.checked ? "true" : "false")
            settings.setValue("Developer", "debugTags", debugTags.checked ? "true" : "false")
            settings.setValue("Developer", "debugPlacemarks", debugPlacemarks.checked ? "true" : "false")
            settings.setValue("Developer", "debugPolygons", debugPolygons.checked ? "true" : "false")
            settings.setValue("Developer", "debugBatches", debugBatches.checked ? "true" : "false")
            settings.setValue("Developer", "debugOutput", debugOutputEnabled ? "true" : "false")
        }
    }

    Rectangle {
        anchors.fill: parent
        color: palette.base
    }


    SwipeView {
        id: swipeView
        currentIndex: pageIndicator.currentIndex
        anchors.left: parent.left
        anchors.right: parent.right
        height: infoItem.height

        Item {
            id: tilesItem
            height: childrenRect.height

            Item {
                height: childrenRect.height
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: Screen.pixelDensity * 2

                Column {
                    spacing: Screen.pixelDensity * 1

                    Text {
                        text: "Tiles"
                    }
                    Grid {
                        columns: 2
                        flow: Grid.TopToBottom
                        rowSpacing: Screen.pixelDensity * 0.5
                        columnSpacing: Screen.pixelDensity * 2

                        CheckBox {
                            id: mapTheme
                            text: "Development Tiles"
                            checked: settings.value("MarbleMaps", "mapThemeId") === "earth/vectorosm-dev/vectorosm-dev.dgml"
                            onCheckedChanged: marbleMaps.mapThemeId = checked ? "earth/vectorosm-dev/vectorosm-dev.dgml" : "earth/vectorosm/vectorosm.dgml"
                        }

                        Button {
                            text: "Reload Tiles"
                            onClicked: marbleMaps.reloadTiles()
                        }
                    }
                }
            }
        }

        Item {
            id: toolsItem
            height: childrenRect.height

            Item {
                height: childrenRect.height
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: Screen.pixelDensity * 2

                Column {
                    spacing: Screen.pixelDensity * 1

                    Text {
                        text: "Tools"
                    }
                    Grid {
                        columns: 2
                        flow: Grid.TopToBottom
                        rowSpacing: Screen.pixelDensity * 0.5
                        columnSpacing: Screen.pixelDensity * 2

                        CheckBox {
                            id: debugTags
                            text: "OSM Tags"
                            checked: settings.value("Developer", "debugTags") === "true"
                            onCheckedChanged: app.showOsmTags = checked
                        }

                        CheckBox {
                            text: "Shell Output"
                            checked: settings.value("Developer", "debugOutput") === "true"
                            onCheckedChanged: settings.debugOutputEnabled = checked
                        }

                        CheckBox {
                            text: "Inertial Rotation"
                            checked: settings.value("Developer", "inertialGlobeRotation") === "true"
                            onCheckedChanged: marbleMaps.inertialGlobeRotation = checked
                        }

                        CheckBox {
                            text: "GPS Simulation"
                            checked: settings.value("Developer", "positionProvider") === "RouteSimulationPositionProviderPlugin"
                            onCheckedChanged: marbleMaps.currentPositionProvider = checked ? "RouteSimulationPositionProviderPlugin" : "QtPositioning"
                        }
                    }
                }
            }
        }

        Item {
            id: infoItem
            height: childrenRect.height

            Item {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: childrenRect.height
                anchors.margins: Screen.pixelDensity * 2

                Column {
                    spacing: Screen.pixelDensity * 1

                    Text {
                        text: "Debug Rendering"
                    }

                    Grid {
                        columns: 2
                        flow: Grid.TopToBottom
                        rowSpacing: Screen.pixelDensity * 0.5
                        columnSpacing: Screen.pixelDensity * 2

                        CheckBox {
                            id: runtimeTrace
                            text: "Performance"
                            checked: settings.value("Developer", "runtimeTrace") === "true"
                            onCheckedChanged: marbleMaps.setShowRuntimeTrace(checked)
                        }

                        CheckBox {
                            id: debugBatches
                            text: "Batches"
                            checked: settings.value("Developer", "debugBatches") === "true"
                            onCheckedChanged: marbleMaps.setShowDebugBatches(checked)
                        }

                        CheckBox {
                            id: debugPolygons
                            text: "Polygons"
                            checked: settings.value("Developer", "debugPolygons") === "true"
                            onCheckedChanged: marbleMaps.setShowDebugPolygons(checked)
                        }

                        CheckBox {
                            id: debugPlacemarks
                            text: "Placemarks"
                            checked: settings.value("Developer", "debugPlacemarks") === "true"
                            onCheckedChanged: marbleMaps.setShowDebugPlacemarks(checked)
                        }
                    }
                }
            }
        }

        Item {
            id: localizationItem
            height: childrenRect.height

            property string translationFile: settings.value("localization", "translationFile", "")
            property string displayFile: translationFile.length > 0 ? translationFile.replace(/^.*[\\\/]/, '') : "none"

            Item {
                height: childrenRect.height
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: Screen.pixelDensity * 2

                Column {
                    spacing: Screen.pixelDensity * 1

                    Text {
                        text: "Translations"
                    }

                    Grid {
                        columns: 2
                        flow: Grid.TopToBottom
                        rowSpacing: Screen.pixelDensity * 0.5
                        columnSpacing: Screen.pixelDensity * 3


                        Text {
                            text: "Custom file: " + localizationItem.displayFile
                        }

                        Row {
                            spacing: Screen.pixelDensity * 2

                            Button {
                                text: "Change"
                                onClicked: fileDialog.open()
                            }

                            Button {
                                text: "Remove"
                                onClicked: localizationItem.translationFile = ""
                            }
                        }

                        CheckBox {
                            id: ignoreTranslations
                            text: "Disable all translations"
                            checked: settings.value("localization", "translationsDisabled", "false") === "true"
                        }

                        Text {
                            text: "<i>Changes require a restart</i>"
                        }

                    }
                }
            }
        }
    }

    PageIndicator {
        id: pageIndicator
        interactive: true
        count: swipeView.count
        currentIndex: swipeView.currentIndex

        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    FileDialog {
        id: fileDialog
        title: "Choose a translation file"
        folder: shortcuts.home
        sidebarVisible: false
        nameFilters: [ "Translation files (*.qm)" ]
        onAccepted: localizationItem.translationFile = fileUrl
    }
}
