// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

import org.kde.marble as Marble

Components.BottomDrawer {
    id: root

    required property MainScreen app
    required property Marble.MarbleItem map
    property Marble.Placemark placemark: null
    property alias showOsmTags: tagsView.visible
    property bool showAccessibility: false
    property Marble.Bookmarks bookmarks

    onMapChanged: bookmarks.map = map

    Kirigami.ScrollablePage {
        leftPadding: Kirigami.Units.mediumSpacing
        rightPadding: Kirigami.Units.mediumSpacing
        topPadding: Kirigami.Units.mediumSpacing
        bottomPadding: Kirigami.Units.mediumSpacing

        ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Heading {
                    text: if (!root.placemark) {
                        return '';
                    } else if (root.placemark.name.length > 0) {
                        return root.placemark.name;
                    } else if (root.placemark.description) {
                        return root.placemark.description;
                    } else {
                        return KI18n.i18nc("placeholder", "No Name");
                    }
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Controls.ToolButton {
                    id: bookmarkButton

                    property bool bookmark: root.placemark && root.bookmarks.isBookmark(root.placemark.longitude, root.placemark.latitude)
                    onBookmarkChanged: checked = bookmark;

                    icon.name: bookmark ? 'starred-symbolic' : 'non-starred-symbolic'
                    onClicked: {
                        if (bookmarkButton.bookmark) {
                            root.bookmarks.removeBookmark(root.app.selectedPlacemark.longitude, root.app.selectedPlacemark.latitude)
                        } else {
                            root.bookmarks.addBookmark(root.app.selectedPlacemark, "Default")
                        }
                        checked = root.placemark && root.bookmarks.isBookmark(root.placemark.longitude, root.placemark.latitude);
                    }
                }
            }

            IconText {
                Layout.fillWidth: true

                visible: text.length > 0 && root.placemark && root.placemark.name.length > 0
                icon: 'description-symbolic'
                text: root.placemark?.description ?? ''
            }

            IconText {
                Layout.fillWidth: true

                visible: text.length > 0
                icon: 'mark-location-symbolic'
                text: root.placemark?.address ?? ''
                maximumLineCount: 4
            }

            /** TODO(routing)
            IconText {
                Layout.fillWidth: true

                visible: routesItem.count > 0
                text: "<a href=\"#\">Part of " + routesItem.count + " routes</a>"
                maximumLineCount: 4
                onLinkActivated: routesDialog.open()
            }*/

            IconText {
                Layout.fillWidth: true

                visible: url.length > 0
                property string url: root.placemark === null ? "" : root.placemark.website
                text: "<a href=\"" + url + "\">" + url + "</a>"
                icon: 'internet-web-browser-symbolic'
                maximumLineCount: 4
                onLinkActivated: link => Qt.openUrlExternally(link)
            }

            IconText {
                Layout.fillWidth: true

                visible: phone.length > 0
                property string phone: root.placemark?.phone ?? ''
                text: "<a href=\"tel:" + phone + "\">" + phone + "</a>"
                icon: 'phone-symbolic'
                maximumLineCount: 1
                onLinkActivated: link => Qt.openUrlExternally(link)
            }

            IconText {
                Layout.fillWidth: true

                visible: url.length > 0
                property string url: root.placemark?.wikipedia ?? ''
                text:  "<a href=\"" + url + "\">Wikipedia</a>"
                icon: 'internet-web-browser-symbolic'
                maximumLineCount: 4
                onLinkActivated: link => Qt.openUrlExternally(link)
            }

            IconText {
                Layout.fillWidth: true

                visible: text.length > 0
                text: root.placemark?.openingHours ?? ''
                icon: 'accept_time_event-symbolic'
            }

            IconText {
                Layout.fillWidth: true

                visible: root.showAccessibility && text.length > 0
                text: root.placemark?.wheelchairInfo ?? ''
                icon: 'preferences-desktop-accessibility-symbolic'
            }

            IconText {
                Layout.fillWidth: true

                visible: text.length > 0
                text: root.placemark?.wifiAvailable ?? ''
                icon: 'network-wireless-symbolic'
            }

            IconText {
                Layout.fillWidth: true

                visible: text.length > 0
                property string coordinates: root.placemark?.coordinates ?? ''
                text: "<a href=\"#\"#>" + coordinates + "</a>"
                icon: 'add-placemark-symbolic'
                onLinkActivated: root.map.centerOnCoordinates(root.placemark.longitude, root.placemark.latitude)
            }

            Kirigami.Heading {
                level: 4
                text: KI18n.i18nc("@title:group", "Tags:")
                visible: tagsView.count > 0
                Layout.topMargin: Kirigami.Units.mediumSpacing
            }

            Flow {
                Layout.fillWidth: true

                spacing: Kirigami.Units.smallSpacing
                Repeater {
                    id: tagsView
                    model: visible && root.placemark ? root.placemark.tags : undefined
                    delegate: Kirigami.Chip {
                        required property string modelData

                        text: modelData
                        checkable: false
                        closable: false
                    }
                }
            }
        }
    }

    // TODO(routing)
    //Kirigami.OverlaySheet {
    //    id: routesDialog
    //    ColumnLayout {
    //        property int implicitWidth: root.width
    //        id: columnLayout
    //        Kirigami.Heading {
    //            Layout.fillWidth: true
    //            level: 2
    //            text: KI18n.i18nc("@title", "Routes")

    //        }
    //        RoutesItem {
    //            id: routesItem
    //            Layout.fillWidth: true
    //            placemark: root.placemark
    //            onHighlightChanged: map.highlightRouteRelation(oid, enabled)
    //        }

    //    }
    //}
}
