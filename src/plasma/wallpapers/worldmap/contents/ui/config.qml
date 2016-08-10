/*
 * Copyright 2016  Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0 as QtControls

// for "units"
import org.kde.plasma.core 2.0 as PlasmaCore


ColumnLayout {
    id: root
    property int cfg_projection: wallpaper.configuration.projection // Enum needs manual set/get for now
    property int cfg_centerMode: wallpaper.configuration.centerMode // Enum needs manual set/get for now
    property alias cfg_fixedLongitude: longitudeSpinBox.value

    RowLayout {
        spacing: units.largeSpacing / 2

        // To allow aligned integration in the settings form,
        // "formAlignment" is a property injected by the config containment
        // which defines the offset of the value fields
        QtControls.Label {
            Layout.minimumWidth: width
            Layout.maximumWidth: width
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            anchors {
                verticalCenter: projectionComboBox.verticalCenter
            }

            // use i18nd, as textdomain otherwise would be defined by config containment
            text: i18nd("plasma_wallpaper_org.kde.plasma.worldmap", "Projection:")
        }
        QtControls.ComboBox {
            id: projectionComboBox
            model: [
                i18nd("plasma_wallpaper_org.kde.plasma.worldmap", "Equirectangular"),
                i18nd("plasma_wallpaper_org.kde.plasma.worldmap", "Mercator")
            ]
            onCurrentIndexChanged: {
                cfg_projection = currentIndex;
            }
            Component.onCompleted: {
                currentIndex = wallpaper.configuration.projection;
            }
        }
    }
    RowLayout {
        spacing: units.largeSpacing / 2

        QtControls.Label {
            Layout.minimumWidth: width
            Layout.maximumWidth: width
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            anchors {
                verticalCenter: centerModeComboBox.verticalCenter
            }
            text: i18nd("plasma_wallpaper_org.kde.plasma.worldmap", "Center on:")
        }

        QtControls.ComboBox {
            id: centerModeComboBox
            model: [
                i18nd("plasma_wallpaper_org.kde.plasma.worldmap", "Daylight"),
                i18nd("plasma_wallpaper_org.kde.plasma.worldmap", "Longitude"),
                i18nd("plasma_wallpaper_org.kde.plasma.worldmap", "Location")
            ]
            onCurrentIndexChanged: {
                cfg_centerMode = currentIndex;
            }
            Component.onCompleted: {
                currentIndex = wallpaper.configuration.centerMode;
            }
        }

    }
    RowLayout {
        spacing: units.largeSpacing / 2

         QtControls.Label {
            Layout.minimumWidth: width
            Layout.maximumWidth: width
            width: formAlignment - units.largeSpacing
            horizontalAlignment: Text.AlignRight
            anchors {
                verticalCenter: longitudeSpinBox.verticalCenter
            }
            enabled: (cfg_centerMode === 1)
            text: i18nd("plasma_wallpaper_org.kde.plasma.worldmap", "Longitude:")
        }

        QtControls.SpinBox {
            enabled: (cfg_centerMode === 1)
            id: longitudeSpinBox
            maximumValue: 180.0
            minimumValue: -180.0
            decimals: 5
        }
    }

    Item { // tighten layout
        Layout.fillHeight: true
    }
}
