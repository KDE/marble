// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kirigamiaddons.components as Components

import org.kde.marble as Marble

FormCard.FormCardPage {
    id: root

    title: i18nc("@title", "Routing")

    property alias currentProfileIcon: profileSelector.profileIcon

    property var routingManager
    property var marbleMaps

    ProfileSelectorMenu {
        id: profileSelector

        Layout.topMargin: Kirigami.Units.largeSpacing * 2
        Layout.alignment: Qt.AlignHCenter
    }

    data: Controls.Popup {
        id: searchResultPopup

        property int index: 0

        padding: 0
        visible: false

        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false

        background: Components.DialogRoundedBackground {}

        implicitHeight: Math.min(Kirigami.Units.gridUnit * 5, searchResultView.contentHeight)

        contentItem: SearchResultView {
            id: searchResultView

            background: null

            Controls.ScrollBar.vertical.interactive: false

            topPadding: 1
            bottomPadding: 1

            onClicked: (placemark, name) => {
                routingManager.routeRequestModel.setPosition(searchResultPopup.index, placemark.longitude, placemark.latitude, name);
                repeater.itemAt(searchResultPopup.index).field.text = name;
                searchResultPopup.close();
                routingManager.updateRoute();
            }

            searchBackend: Marble.SearchBackend {
                id: backend

                marbleQuickItem: root.marbleMaps
                onSearchResultChanged: {
                    searchResultView.searchResults.model = model;
                    searchResultPopup.open();
                }
            }
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing * 2

        Repeater {
            id: repeater

            model: routingManager.routeRequestModel

            delegate: ColumnLayout {
                id: routeDelegate

                required property int index
                required property string name
                property alias field: field

                spacing: 0

                FormCard.FormDelegateSeparator {
                    visible: index !== 0
                }

                FormCard.FormTextFieldDelegate {
                    id: field

                    label: if (routeDelegate.index === 0) {
                        return i18nc("@label:textbox", "Departure:");
                    } else if (routeDelegate.index === repeater.count - 1) {
                        return i18nc("@label:textbox", "Arrival:");
                    } else {
                        return i18nc("@label:textbox", "Waypoint:");
                    }
                    onPressed: searchResultPopup.index = routeDelegate.index;
                    text: routeDelegate.name
                    topPadding: Kirigami.Units.largeSpacing
                    bottomPadding: Kirigami.Units.largeSpacing
                    leftPadding: Kirigami.Units.largeSpacing
                    rightPadding: Kirigami.Units.largeSpacing

                    contentItem: RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Kirigami.SearchField {
                            id: textField
                            Accessible.name: field.label
                            Layout.fillWidth: true
                            placeholderText: field.placeholderText
                            text: field.text
                            onEditingFinished: field.editingFinished()
                            activeFocusOnTab: false

                            autoAccept: false
                            onAccepted: {
                                searchResultPopup.index = routeDelegate.index;
                                if (text.length === 0) {
                                    searchResultPopup.close();
                                    return;
                                }
                                backend.search(text)
                                searchResultPopup.parent = textField;
                                searchResultPopup.x = 0;
                                searchResultPopup.y = height;
                                searchResultPopup.width = width;
                            }

                            Keys.onDownPressed: {
                                searchResultView.searchResults.currentIndex = 0;
                                searchResultView.searchResults.forceActiveFocus()
                            }

                            onTextEdited: fireSearchDelay.restart();

                            Timer {
                                id: fireSearchDelay
                                interval: Kirigami.Units.shortDuration
                                running: false
                                repeat: false
                                onTriggered: {
                                    textField.accepted();
                                }
                            }
                        }

                        Controls.Button {
                            id: deleteAddButton

                            icon.name: if (routeDelegate.index === 0) {
                                return 'list-add-symbolic';
                            } else if (routeDelegate.index === repeater.count - 1) {
                                return 'reverse-symbolic';
                            } else {
                                return 'list-remove-symbolic';
                            }
                            onClicked: {
                                if (routeDelegate.index === 0) {
                                    routingManager.addViaByPlacemark(marbleMaps.currentPosition)
                                } else if (routeDelegate.index === repeater.count - 1) {
                                    routingManager.routeRequestModel.reverse();
                                } else {
                                    routingManager.removeVia(routeDelegate.index);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing * 2

        FormCard.FormComboBoxDelegate {
            topPadding: Kirigami.Units.largeSpacing
            bottomPadding: Kirigami.Units.largeSpacing
            leftPadding: Kirigami.Units.largeSpacing
            rightPadding: Kirigami.Units.largeSpacing

            text: i18nc("@label:listbox", "Alternatives:")
            model: routingManager.alternativeRoutesModel
            textRole: "routeName"
            currentIndex: 0
            onCurrentIndexChanged: routingManager.alternativeRoutesModel.setCurrentRoute(currentIndex);
        }
    }
}
