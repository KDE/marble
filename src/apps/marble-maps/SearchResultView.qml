// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.marble as Marble

Controls.ScrollView {
    id: root

    required property Marble.SearchBackend searchBackend
    property alias searchResults: searchResults

    signal clicked(placemark: var, name: string)

    contentItem: ListView {
        id: searchResults

        clip: true

        delegate: Delegates.RoundedItemDelegate {
            id: searchDelegate

            required property int index
            required property string name
            required property string description
            required property string iconPath

            text: name
            icon.source: iconPath.substr(0,1) === '/' ? "file://" + iconPath : iconPath

            onClicked: {
                root.searchBackend.setSelectedPlacemark(index);
                root.clicked(root.searchBackend.selectedPlacemark, name)
            }
        }

        model: root.searchBackend.completionModel
    }
}
