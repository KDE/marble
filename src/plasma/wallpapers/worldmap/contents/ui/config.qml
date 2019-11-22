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
import QtQuick.Controls 2.5 as QQC2
import org.kde.kirigami 2.5 as Kirigami


Kirigami.FormLayout {
    id: root
    twinFormLayouts: globalSettingsLayout

    property int cfg_projection: wallpaper.configuration.projection // Enum needs manual set/get for now
    property int cfg_centerMode: wallpaper.configuration.centerMode // Enum needs manual set/get for now
    property alias cfg_fixedLongitude: longitudeSpinBox.value

    QQC2.ComboBox {
        id: projectionComboBox
        Kirigami.FormData.label: i18nd("plasma_wallpaper_org.kde.plasma.worldmap", "Projection:")
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

    QQC2.ComboBox {
        id: centerModeComboBox
        Kirigami.FormData.label: i18nd("plasma_wallpaper_org.kde.plasma.worldmap", "Center on:")
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

    QQC2.SpinBox {
        id: longitudeSpinBox
        Kirigami.FormData.label: i18nd("plasma_wallpaper_org.kde.plasma.worldmap", "Longitude:")
        from: -180
        to: 180
        enabled: (cfg_centerMode === 1)
    }
}
