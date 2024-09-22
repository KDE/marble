// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Window
import QtQuick.Layouts
import QtQml.Models

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.marble

Kirigami.ScrollablePage {
    id: root

    title: i18nc("@title:window", "Bookmarks")

    property var marbleMaps

    Bookmarks {
        id: bookmarks
        map: marbleMaps
    }

    ListView {
        id: bookmarksView

        model: bookmarks.model

        delegate: Delegates.RoundedItemDelegate {
            id: bookmarkDelegate

            required property int index
            required property var model
            required property var coordinate

            icon.name: 'mark-location-symbolic'
            text: model.display

            onClicked: {
                root.marbleMaps.centerOn(coordinate)
                root.Controls.ApplicationWindow.window.pageStack.layers.pop();
            }

            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Delegates.DefaultContentItem {
                    itemDelegate: bookmarkDelegate
                    Layout.fillWidth: true
                }

                Controls.ToolButton {
                    icon.name: 'delete-symbolic'
                    onClicked: {
                        const currentBookmark = bookmarks.placemark(index)
                        bookmarks.removeBookmark(currentBookmark.longitude, currentBookmark.latitude)
                    }
                }
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: i18n("Your bookmarks will appear here.")
            visible: bookmarksView.count === 0
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
