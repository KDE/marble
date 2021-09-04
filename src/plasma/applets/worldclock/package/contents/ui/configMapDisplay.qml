/*
    SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.1
import QtQuick.Controls 1.0 as QtControls
import QtQuick.Layouts 1.0

ColumnLayout {
    id: mapDisplayPage

    property int cfg_projection: plasmoid.configuration.projection // Enum needs manual set/get for now
    property int cfg_centerMode: plasmoid.configuration.centerMode // Enum needs manual set/get for now
    property alias cfg_fixedLongitude: longitudeSpinBox.value
    property alias cfg_showDate: showDateCheckBox.checked

    GridLayout {
        columns: 2

        QtControls.Label {
            Layout.row: 0
            Layout.column: 0
            Layout.alignment: Qt.AlignRight
            anchors {
                verticalCenter: projectionComboBox.verticalCenter
            }
            text: i18n("Projection:")
        }

        QtControls.ComboBox {
            id: projectionComboBox
            Layout.row: 0
            Layout.column: 1
            model: [
                i18n("Equirectangular"),
                i18n("Mercator")
            ]
            onCurrentIndexChanged: {
                cfg_projection = currentIndex;
            }
            Component.onCompleted: {
                currentIndex = plasmoid.configuration.projection;
            }
        }

        QtControls.Label {
            Layout.row: 1
            Layout.column: 0
            Layout.alignment: Qt.AlignRight
            anchors {
                verticalCenter: centerModeComboBox.verticalCenter
            }
            text: i18n("Center on:")
        }

        QtControls.ComboBox {
            id: centerModeComboBox
            Layout.row: 1
            Layout.column: 1
            model: [
                i18n("Daylight"),
                i18n("Longitude"),
                i18n("Location")
            ]
            onCurrentIndexChanged: {
                cfg_centerMode = currentIndex;
            }
            Component.onCompleted: {
                currentIndex = plasmoid.configuration.centerMode;
            }
        }

        QtControls.Label {
            Layout.row: 3
            Layout.column: 0
            Layout.alignment: Qt.AlignRight
            anchors {
                verticalCenter: longitudeSpinBox.verticalCenter
            }
            enabled: (cfg_centerMode === 1)
            text: i18n("Longitude:")
        }

        QtControls.SpinBox {
            Layout.row: 3
            Layout.column: 1
            enabled: (cfg_centerMode === 1)
            id: longitudeSpinBox
            maximumValue: 180.0
            minimumValue: -180.0
            decimals: 5
        }

        QtControls.Label {
            Layout.row: 4
            Layout.column: 0
            Layout.alignment: Qt.AlignRight
            text: i18n("Show date:")
        }

        QtControls.CheckBox {
            Layout.row: 4
            Layout.column: 1
            id: showDateCheckBox
        }
    }

    Item { // tighten layout
        Layout.fillHeight: true
    }
}
