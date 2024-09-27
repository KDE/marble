/*
    SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick

import org.kde.plasma.configuration

ConfigModel {
    ConfigCategory {
        name: i18n("Map Display")
        icon: "preferences-desktop-display"
        source: "configMapDisplay.qml"
    }

/* TODO: readd later
    ConfigCategory {
        name: i18n("Time Zones")
        icon: "preferences-system-time"
        source: "configTimeZones.qml"
    }
*/
}
