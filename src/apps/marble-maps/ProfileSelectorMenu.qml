// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.marble

Controls.Control {
    id: root

    property string profileIcon

    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0

    Controls.ButtonGroup {
        id: profileGroup
    }

    component SelectionDelegate : FormCard.AbstractFormDelegate {
        id: selectionDelegate

        required property int profile

        Controls.ButtonGroup.group: profileGroup

        checkable: true
        checked: Config.profile === profile
        background: FormCard.FormDelegateBackground {
            control: selectionDelegate
            color:  if (selectionDelegate.highlighted || selectionDelegate.checked || (selectionDelegate.down && !root.checked) || selectionDelegate.visualFocus) {
                const highlight = Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.highlightColor, 0.3);
                if (selectionDelegate.hovered) {
                    return Kirigami.ColorUtils.tintWithAlpha(highlight, Kirigami.Theme.textColor, 0.10);
                } else {
                    return highlight;
                }
            } else if (selectionDelegate.hovered) {
                return Kirigami.ColorUtils.tintWithAlpha(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.10);
            } else {
                return 'transparent';
            }

            corners {
                topLeftRadius: profile === Config.Car ? Kirigami.Units.cornerRadius + 1: 0
                topRightRadius: profile === Config.Pedestrian ? Kirigami.Units.cornerRadius + 1: 0
                bottomLeftRadius: profile === Config.Car ? Kirigami.Units.cornerRadius + 1: 0
                bottomRightRadius: profile === Config.Pedestrian ? Kirigami.Units.cornerRadius + 1: 0
            }
        }
        icon {
            width: Kirigami.Units.iconSizes.small
            height: Kirigami.Units.iconSizes.small
        }
        contentItem: RowLayout {
            Kirigami.Icon {
                visible: selectionDelegate.icon.name.length > 0 || selectionDelegate.icon.source.toString().length > 0
                source: selectionDelegate.icon.name.length > 0 ? selectionDelegate.icon.name : selectionDelegate.icon.source
                color: selectionDelegate.icon.color
                implicitWidth: visible ? selectionDelegate.icon.width : 0
                implicitHeight: visible ? selectionDelegate.icon.height : 0
            }
        }

        Controls.ToolTip.text: text
        Controls.ToolTip.visible: hovered
        Controls.ToolTip.delay: Kirigami.Units.toolTipDelay

        onClicked: {
            Config.profile = profile;
            Config.save();
            root.profileIcon = icon.source;
        }
    }

    background: Kirigami.ShadowedRectangle {
        readonly property real borderWidth: 1
        readonly property bool isDarkColor: {
            const temp = Qt.darker(Kirigami.Theme.backgroundColor, 1);
            return temp.a > 0 && getDarkness(Kirigami.Theme.backgroundColor) >= 0.4;
        }

        Kirigami.Theme.colorSet: Kirigami.Theme.View
        Kirigami.Theme.inherit: false

        // only have card radius if it isn't filling the entire width
        radius: Kirigami.Units.cornerRadius
        color: Kirigami.Theme.backgroundColor

        function getDarkness(background: color): real {
            // Thanks to Gojir4 from the Qt forum
            // https://forum.qt.io/topic/106362/best-way-to-set-text-color-for-maximum-contrast-on-background-color/
            var temp = Qt.darker(background, 1);
            var a = 1 - ( 0.299 * temp.r + 0.587 * temp.g + 0.114 * temp.b);
            return a;
        }

        border {
            color: isDarkColor ? Qt.darker(Kirigami.Theme.backgroundColor, 1.2) : Kirigami.ColorUtils.linearInterpolation(Kirigami.Theme.backgroundColor, Kirigami.Theme.textColor, 0.15)
            width: borderWidth
        }

        shadow {
            size: isDarkColor ? Kirigami.Units.smallSpacing : Kirigami.Units.largeSpacing
            color: Qt.alpha(Kirigami.Theme.textColor, 0.10)
        }
    }

    contentItem: RowLayout {
        spacing: 0

        SelectionDelegate {
            profile: Config.Car
            text: i18n("Car (fastest)")
            icon.source: Qt.resolvedUrl("images/transport-mode-car.svg")
        }

        Kirigami.Separator {
            Layout.fillHeight: true
        }

        SelectionDelegate {
            profile: Config.Bicycle
            text: i18n("Bicycle")
            icon.source: Qt.resolvedUrl("images/transport-mode-bike.svg")
        }

        Kirigami.Separator {
            Layout.fillHeight: true
        }

        SelectionDelegate {
            profile: Config.Pedestrian
            text: i18n("Pedestrian")
            icon.source: Qt.resolvedUrl("images/transport-mode-walk.svg")
        }
    }
}
