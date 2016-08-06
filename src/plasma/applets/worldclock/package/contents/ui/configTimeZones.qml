/*
 * Copyright 2016  Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>.
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

