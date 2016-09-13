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
import QtQuick.Layouts 1.1

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

import org.kde.marble.private.plasma 0.20

Item {
    id: root

    readonly property date currentDateTime: timeDataSource.data.Local ? timeDataSource.data.Local.DateTime : new Date()

    PlasmaCore.DataSource {
        id: timeDataSource
        engine: "time"
        connectedSources: ["Local"]
        interval: 60000
        intervalAlignment: PlasmaCore.Types.AlignToMinute
    }
    PlasmaCore.DataSource {
        id: geolocationDataSource
        engine: "geolocation"
        connectedSources: (marbleItem.centerMode === 2) ? ["location"] : []
        interval: 10 * 60 * 1000 // every 30 minutes, might be still too large for users on the ISS :P
    }

    Plasmoid.preferredRepresentation: Plasmoid.fullRepresentation

    Plasmoid.fullRepresentation: MarbleItem {
        id: marbleItem

        readonly property int centerMode: plasmoid.configuration.centerMode
        readonly property double fixedLongitude: plasmoid.configuration.fixedLongitude
        readonly property double locationLongitude: geolocationDataSource.data.longitude || 0

        enabled: false // do not handle input
        Layout.minimumWidth: units.gridUnit * 20
        Layout.minimumHeight: units.gridUnit * 10

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
        projection: (plasmoid.configuration.projection === 0) ? MarbleItem.Equirectangular : MarbleItem.Mercator
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
        // TODO: showCredit: false

        onCenterModeChanged: handleCenterModeChange()
        function handleCenterModeChange() {
            if (centerMode === 0) {
                marbleMap.setLockToSubSolarPoint(true);
            } else if (centerMode === 1) {
                marbleMap.setLockToSubSolarPoint(false);
                marbleMap.centerOn(fixedLongitude, 0.0);
            } else {
                marbleMap.setLockToSubSolarPoint(false);
                marbleMap.centerOn(locationLongitude, 0.0);
            }
        }

        onFixedLongitudeChanged: {
            if (centerMode === 1) {
                marbleMap.centerOn(fixedLongitude, 0.0);
            }
        }

        onLocationLongitudeChanged: {
            if (centerMode === 2) {
                marbleMap.centerOn(locationLongitude, 0.0);
            }
        }

        Component.onCompleted: {
            marbleMap.setShowSunShading(true);
            marbleMap.setShowCityLights(true);

            marbleMap.setShowPlaces(false);
            marbleMap.setShowOtherPlaces(false);
            marbleMap.setShowCities(false);
            marbleMap.setShowTerrain(false);

            handleCenterModeChange();
        }

        ColumnLayout {
            anchors.centerIn: parent

            PlasmaExtras.Heading  {
                id: timeLabel

                Layout.alignment: Qt.AlignHCenter

                level: 1
                text: plasmoid.configuration.showDate ? Qt.formatDateTime(currentDateTime) : Qt.formatTime(currentDateTime)

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            /*
            PlasmaExtras.Heading {
                id: timezoneLabel

                Layout.alignment: Qt.AlignHCenter

                level: 3
                text: "Internet"

                visible: text.length > 0
                horizontalAlignment: Text.AlignHCenter
            }
            */
        }
    }
}
