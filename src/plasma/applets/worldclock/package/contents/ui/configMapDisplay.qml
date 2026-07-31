/*
    SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.ki18n
import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.plasma.plasmoid

KCM.SimpleKCM {
    id: mapDisplayPage

    property int cfg_projection: Plasmoid.configuration.projection // Enum needs manual set/get for now
    property int cfg_centerMode: Plasmoid.configuration.centerMode // Enum needs manual set/get for now
    property alias cfg_fixedLongitude: longitudeSpinBox.realValue
    property alias cfg_showDate: showDateCheckBox.checked

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.largeSpacing

        FormCard.FormComboBoxDelegate {
            text: KI18n.i18n("Projection:")
            model: [
                KI18n.i18n("Equirectangular"),
                KI18n.i18n("Mercator")
            ]
            onCurrentIndexChanged: {
                cfg_projection = currentIndex;
            }
            Component.onCompleted: {
                currentIndex = Plasmoid.configuration.projection;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormComboBoxDelegate {
            text: KI18n.i18n("Center on:")
            model: [
                KI18n.i18n("Daylight"),
                KI18n.i18n("Longitude"),
                KI18n.i18n("Location")
            ]
            onCurrentIndexChanged: {
                cfg_centerMode = currentIndex;
            }
            Component.onCompleted: {
                currentIndex = Plasmoid.configuration.centerMode;
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormSpinBoxDelegate {
            id: longitudeSpinBox

            enabled: (cfg_centerMode === 1)
            label: KI18n.i18n("Longitude:")
            to: decimalToInt(180.0)
            from: decimalToInt(-180.0)
            stepSize: decimalFactor

            validator: DoubleValidator {
                bottom: Math.min(longitudeSpinBox.from, longitudeSpinBox.to)
                top:  Math.max(longitudeSpinBox.from, longitudeSpinBox.to)
                decimals: longitudeSpinBox.decimals
                notation: DoubleValidator.StandardNotation
            }

            textFromValue: function(value, locale) {
                return Number(value / decimalFactor).toLocaleString(locale, 'f', longitudeSpinBox.decimals)
            }

            valueFromText: function(text, locale) {
                return Math.round(Number.fromLocaleString(locale, text) * decimalFactor)
            }

            property real realValue: value / decimalFactor
            property int decimals: 5
            readonly property int decimalFactor: Math.pow(10, decimals)

            function decimalToInt(decimal) {
                return decimal * decimalFactor
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormCheckDelegate {
            id: showDateCheckBox
            text: KI18n.i18n("Show date:")
        }
    }
}
