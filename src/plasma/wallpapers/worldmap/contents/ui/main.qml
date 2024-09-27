/*
    SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras
import org.kde.plasma.plasmoid
import org.kde.plasma.plasma5support as P5Support

import org.kde.marble

WallpaperItem {
    id: root

    readonly property int centerMode: root.configuration.centerMode
    readonly property double fixedLongitude: root.configuration.fixedLongitude
    readonly property double locationLongitude: geolocationDataSource.data.longitude

    onLocationLongitudeChanged: if (centerMode === 2) {
        marbleMap.centerOn(locationLongitude, 0.0);
    }

    onFixedLongitudeChanged: if (centerMode === 1) {
        marbleMap.centerOn(fixedLongitude, 0.0);
    }

    onCenterModeChanged: handleCenterModeChange()

    function handleCenterModeChange(): void {
        if (centerMode === 0) {
            marbleMap.setLockToSubSolarPoint(true);
        } else if (centerMode === 1)  {
            marbleMap.setLockToSubSolarPoint(false);
            marbleMap.centerOn(fixedLongitude, 0.0);
        } else {
            marbleMap.setLockToSubSolarPoint(false);
            marbleMap.centerOn(locationLongitude, 0.0);
        }
    }

    MarbleItem {
        id: marbleItem

        anchors.fill: parent
        enabled: false // do not handle input

        radius: {
            var ratio = width/height;
            if (projection === MarbleItem.Equirectangular) {
                if (ratio > 2) {
                    return height / 2;
                }
                return width / 4;
            } else {
                if (ratio > 1) {
                    return height / 4;
                }
                return width / 4
            }
        }

        // Theme settings.
        projection: (root.configuration.projection === 0) ? MarbleItem.Equirectangular : MarbleItem.Mercator
        mapThemeId: "earth/bluemarble/bluemarble.dgml"

        // Visibility of layers/plugins.
        showAtmosphere: false
        showClouds: false
        showBackground: false

        showGrid: false
        showCrosshairs: false
        showCompass: false
        showOverviewMap: false
        showScaleBar: false

        Component.onCompleted: {
            marbleMap.setShowSunShading(true);
            marbleMap.setShowCityLights(true);

            marbleMap.setShowPlaces(false);
            marbleMap.setShowCities(false);
            marbleMap.setShowTerrain(false);
            marbleMap.setShowOtherPlaces(false);

            handleCenterModeChange();
        }

        P5Support.DataSource {
            id: geolocationDataSource
            engine: "geolocation"
            connectedSources: (marbleItem.centerMode === 2) ? ["location"] : []
            interval: 10 * 60 * 1000 // every 30 minutes, might be still too large for users on the ISS :P
        }
    }
}
