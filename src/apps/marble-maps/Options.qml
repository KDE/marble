// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.marble

FormCard.FormCardPage {
    id: root

    property var marbleMaps

    title: i18nc("@title:window", "Layer Options")

    FormCard.FormHeader {
        title: i18nc("@title:group", "Public Transport Layers")
    }

    FormCard.FormCard {
        FormCard.FormCheckDelegate {
            text: i18nc("@option:check", "Train")
            icon.source: Qt.resolvedUrl("images/transport-mode-train.svg")
            checked: root.marbleMaps.isRelationTypeVisible("train")
            onCheckedChanged: root.marbleMaps.setRelationTypeVisible("train", checked)
        }

        FormCard.FormCheckDelegate {
            text: i18nc("@option:check", "Subway")
            icon.source: Qt.resolvedUrl("images/transport-mode-subway.svg")
            checked: root.marbleMaps.isRelationTypeVisible("subway")
            onCheckedChanged: root.marbleMaps.setRelationTypeVisible("subway", checked)
        }

        FormCard.FormCheckDelegate {
            text: i18nc("@option:check", "Tram")
            icon.source: Qt.resolvedUrl("images/transport-mode-tram.svg")
            checked: root.marbleMaps.isRelationTypeVisible("tram")
            onCheckedChanged: root.marbleMaps.setRelationTypeVisible("tram", checked)
        }

        FormCard.FormCheckDelegate {
            text: i18nc("@option:check", "Bus")
            icon.source: Qt.resolvedUrl("images/transport-mode-bus.svg")
            checked: root.marbleMaps.isRelationTypeVisible("bus")
            onCheckedChanged: root.marbleMaps.setRelationTypeVisible("bus", checked)
        }

        FormCard.FormCheckDelegate {
            text: i18nc("@option:check", "Trolley Bus")
            icon.source: Qt.resolvedUrl("images/transport-mode-bus.svg")
            checked: root.marbleMaps.isRelationTypeVisible("trolley-bus")
            onCheckedChanged: root.marbleMaps.setRelationTypeVisible("trolley-bus", checked)
        }
    }

    FormCard.FormHeader {
        title: i18n("Outdoor Activities Layers")
    }

    FormCard.FormCard {
        FormCard.FormCheckDelegate {
            text: i18nc("@option:check", "Walkways")
            icon.source: Qt.resolvedUrl("images/transport-mode-walk.svg")
            checked: root.marbleMaps.isRelationTypeVisible("foot")
            onCheckedChanged: root.marbleMaps.setRelationTypeVisible("foot", checked)
        }

        FormCard.FormCheckDelegate {
            text: i18nc("@option:check", "Hiking Routes")
            icon.source: Qt.resolvedUrl("images/transport-mode-walk.svg")
            checked: root.marbleMaps.isRelationTypeVisible("hiking")
            onCheckedChanged: root.marbleMaps.setRelationTypeVisible("hiking", checked)
        }

        FormCard.FormCheckDelegate {
            text: i18nc("@option:check", "Bicycle Routes")
            icon.source: Qt.resolvedUrl("images/transport-mode-bike.svg")
            checked: root.marbleMaps.isRelationTypeVisible("bicycle")
            onCheckedChanged: root.marbleMaps.setRelationTypeVisible("bicycle", checked)
        }

        FormCard.FormCheckDelegate {
            text: i18nc("@option:check", "Mountainbike Routes")
            icon.source: Qt.resolvedUrl("images/transport-mode-bike.svg")
            checked: root.marbleMaps.isRelationTypeVisible("mountainbike")
            onCheckedChanged: root.marbleMaps.setRelationTypeVisible("mountainbike", checked)
        }

        FormCard.FormCheckDelegate {
            text: i18nc("@option:check", "Inline Skating Routes")
            icon.source: Qt.resolvedUrl("images/transport-mode-inline-skater.svg")
            checked: root.marbleMaps.isRelationTypeVisible("inline-skates")
            onCheckedChanged: root.marbleMaps.setRelationTypeVisible("inline-skates", checked)
        }

        FormCard.FormCheckDelegate {
            text: i18nc("@option:check", "Bridleways")
            icon.source: Qt.resolvedUrl("images/transport-mode-horse-riding.svg")
            checked: root.marbleMaps.isRelationTypeVisible("horse")
            onCheckedChanged: root.marbleMaps.setRelationTypeVisible("horse", checked)
        }
    }
}
