// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.ki18n
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.AboutPage {
    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Germanisches Nationalmuseum")
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            text: KI18n.i18n("The original Behaim globe can be visited in the <a href=\"https://www.gnm.de/\">Germanisches Nationalmuseum</a> in Nuremberg, Germany.")
            textItem.wrapMode: Text.WordWrap
        }
    }

    FormCard.FormHeader {
        title: KI18n.i18nc("@title:group", "Map Content")
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            text: KI18n.i18n("Digitized map based on orthophotographic gores by TU Vienna, 1990. Germanisches Nationalmuseum and Friedrich-Alexander-Universität Erlangen-Nürnberg, CC BY-SA 3.0. Ghillany map based on two planiglobes which are provided as a map supplement to F.W. Ghillany's \"Geschichte des Seefahrers Ritter Martin Behaim nach den ältesten vorhandenen Urkunden\", Nuremberg 1853. CC BY-SA 3.0.")
            textItem.wrapMode: Text.WordWrap
        }
    }
}
