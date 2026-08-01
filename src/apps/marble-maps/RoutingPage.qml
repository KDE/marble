// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.marble as Marble

FormCard.FormCardPage {
    id: root

    Marble.Settings {
        id: settings
        Component.onDestruction: {
            settings.setValue("Routing", "profile", root.selectedProfile)
        }
    }

    FormCard.FormCard {

    }
}
