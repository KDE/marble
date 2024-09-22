// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardPage {
    id: root

    Settings {
        id: settings
        Component.onDestruction: {
            settings.setValue("Routing", "profile", root.selectedProfile)
        }
    }

    FormCard.FormCard {
        
    }
}
