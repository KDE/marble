// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Window
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.marble as Marble

Kirigami.ScrollablePage {
    id: root

    title: KI18n.i18nc("@title:window", "Bookmarks")

    required property Marble.MarbleItem marbleMaps

    Marble.Bookmarks {
        id: bookmarks
        map: root.marbleMaps
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
                (root.Kirigami.PageStack.pageStack as Kirigami.PageRow).layers.pop();
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
                        const currentBookmark = bookmarks.placemark(bookmarkDelegate.index)
                        bookmarks.removeBookmark(currentBookmark.longitude, currentBookmark.latitude)
                    }
                }
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            text: KI18n.i18n("Your bookmarks will appear here.")
            visible: bookmarksView.count === 0
            width: parent.width - Kirigami.Units.gridUnit * 4
        }
    }
}
