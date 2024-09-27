// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.marble
import org.kde.marble.maps

Kirigami.ScrollablePage {
    id: root

    property MarbleItem marbleMaps

    title: i18nc("@title:window", "Themes")

    ListView {
        model: MapThemeModel {}
        delegate: Delegates.RoundedItemDelegate {
            id: themeDelegate

            required property int index
            required property string themeName
            required property string mapThemeId
            required property string description

            text: themeName

            icon {
                source: "image://maptheme/" + themeDelegate.mapThemeId
                width: Kirigami.Units.iconSizes.huge
                height: Kirigami.Units.iconSizes.huge
            }

            contentItem: Delegates.SubtitleContentItem {
                itemDelegate: themeDelegate
                subtitle: themeDelegate.description
                subtitleItem.wrapMode: Text.WordWrap
            }

            highlighted: Config.mapThemeId === mapThemeId
            onClicked: Config.mapThemeId = mapThemeId
        }
    }
}
