// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick
import QtQuick.Window
import QtQuick.Controls as Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.components as Components

import org.kde.marble

Components.BottomDrawer {
    id: root

    property var placemark: null
    property variant map
    property alias showOsmTags: tagsView.visible
    property bool showAccessibility: false
    property Bookmarks bookmarks

    onMapChanged: bookmarks.map = root.map

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
                    text: if (!placemark) {
                        return '';
                    } else if (placemark.name.length > 0) {
                        return placemark.name;
                    } else if (placemark.description) {
                        return placemark.description;
                    } else {
                        return i18nc("placeholder", "No Name");
                    }
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Controls.ToolButton {
                    id: bookmarkButton

                    property bool bookmark: placemark && bookmarks.isBookmark(root.placemark.longitude, root.placemark.latitude)
                    onBookmarkChanged: checked = bookmark;

                    icon.name: bookmark ? 'starred-symbolic' : 'non-starred-symbolic'
                    onClicked: {
                        if (bookmarkButton.bookmark) {
                            bookmarks.removeBookmark(app.selectedPlacemark.longitude, app.selectedPlacemark.latitude)
                        } else {
                            bookmarks.addBookmark(app.selectedPlacemark, "Default")
                        }
                        checked = placemark && bookmarks.isBookmark(root.placemark.longitude, root.placemark.latitude);
                    }
                }
            }

            IconText {
                Layout.fillWidth: true

                visible: text.length > 0 && placemark && placemark.name.length > 0
                icon: 'description-symbolic'
                text: placemark?.description ?? ''
            }

            IconText {
                Layout.fillWidth: true

                visible: text.length > 0
                icon: 'mark-location-symbolic'
                text: placemark?.address ?? ''
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
                property string url: placemark === null ? "" : placemark.website
                text: "<a href=\"" + url + "\">" + url + "</a>"
                icon: 'internet-web-browser-symbolic'
                maximumLineCount: 4
                onLinkActivated: Qt.openUrlExternally(link)
            }

            IconText {
                Layout.fillWidth: true

                visible: phone.length > 0
                property string phone: placemark?.phone ?? ''
                text: "<a href=\"tel:" + phone + "\">" + phone + "</a>"
                icon: 'phone-symbolic'
                maximumLineCount: 1
                onLinkActivated: Qt.openUrlExternally(link)
            }

            IconText {
                Layout.fillWidth: true

                visible: url.length > 0
                property string url: placemark?.wikipedia ?? ''
                text:  "<a href=\"" + url + "\">Wikipedia</a>"
                icon: 'internet-web-browser-symbolic'
                maximumLineCount: 4
                onLinkActivated: Qt.openUrlExternally(link)
            }

            IconText {
                Layout.fillWidth: true

                visible: text.length > 0
                text: placemark?.openingHours ?? ''
                icon: 'accept_time_event-symbolic'
            }

            IconText {
                Layout.fillWidth: true

                visible: root.showAccessibility && text.length > 0
                text: placemark?.wheelchairInfo ?? ''
                icon: 'preferences-desktop-accessibility-symbolic'
            }

            IconText {
                Layout.fillWidth: true

                visible: text.length > 0
                text: placemark?.wifiAvailable ?? ''
                icon: 'network-wireless-symbolic'
            }

            IconText {
                Layout.fillWidth: true

                visible: text.length > 0
                property string coordinates: placemark?.coordinates ?? ''
                text: "<a href=\"#\"#>" + coordinates + "</a>"
                icon: 'add-placemark-symbolic'
                onLinkActivated: marbleMaps.centerOnCoordinates(placemark.longitude, placemark.latitude)
            }

            Kirigami.Heading {
                level: 4
                text: i18nc("@title:group", "Tags:")
                visible: tagsView.count > 0
                Layout.topMargin: Kirigami.Units.mediumSpacing
            }

            Flow {
                Layout.fillWidth: true

                spacing: Kirigami.Units.smallSpacing
                Repeater {
                    id: tagsView
                    model: placemark ? placemark.tags : undefined
                    delegate: Kirigami.Chip {
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
    //            text: i18nc("@title", "Routes")

    //        }
    //        RoutesItem {
    //            id: routesItem
    //            Layout.fillWidth: true
    //            model: placemark === null ? undefined : placemark.routeRelationModel
    //            onHighlightChanged: map.highlightRouteRelation(oid, enabled)
    //        }

    //    }
    //}
}
