/*
    SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.1
import QtQuick.Controls 1.0 as QtControls
import QtQuick.Layouts 1.0

ColumnLayout {
    id: timeZonesPage

    property alias cfg_customTimeZone: customTimeZoneCheckBox.checked

    RowLayout {
        QtControls.Label {
            text: i18n("Use custom time zone selection:")
        }

        QtControls.CheckBox {
            id: customTimeZoneCheckBox
        }
    }

    QtControls.TableView {
        id: timeZoneView
        Layout.fillHeight: true
    }
}

