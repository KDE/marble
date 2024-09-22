// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.marble

RowLayout {
    id: root

    property string selectedProfile
    property string profileIcon

    Settings {
        id: settings
        Component.onDestruction: {
            settings.setValue("Routing", "profile", root.selectedProfile)
        }
    }

    Controls.RadioButton {
        id: carProfileButton
        checked: settings.value("Routing", "profile") === "" || settings.value("Routing", "profile", profile) === profile
        text: i18n("Car (fastest)");
        onClicked: {
            root.selectedProfile = text;
            root.profileIcon = Qt.resolvedUrl("images/transport-mode-car.svg");
        }

        Component.onCompleted: clicked();
    }

    Controls.RadioButton {
        checked: settings.value("Routing", "profile") === profile
        text: i18n("Bicycle");
        onClicked: {
            root.selectedProfile = text;
            root.profileIcon = Qt.resolvedUrl("images/transport-mode-bike.svg");
        }
    }

    Controls.RadioButton {
        checked: settings.value("Routing", "profile") === profile
        text: i18n("Pedestrian")
        onClicked: {
            root.selectedProfile = text;
            root.profileIcon = Qt.resolvedUrl("images/transport-mode-walk.svg");
        }
    }
}
