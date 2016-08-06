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
    id: mapDisplayPage

    property int cfg_projection: plasmoid.configuration.projection // Enum needs manual set/get for now
    property alias cfg_showDate: showDateCheckBox.checked

    GridLayout {
        columns: 2

        QtControls.Label {
            Layout.row: 0
            Layout.column: 0
            Layout.alignment: Qt.AlignRight
            anchors {
                verticalCenter: projectionComboBox.verticalCenter
            }
            text: i18n("Projection:")
        }

        QtControls.ComboBox {
            id: projectionComboBox
            model: [
                i18n("Equirectangular"),
                i18n("Mercator")
            ]
            onCurrentIndexChanged: {
                cfg_projection = currentIndex;
            }
            Component.onCompleted: {
                currentIndex = plasmoid.configuration.projection;
            }
        }

        QtControls.Label {
            Layout.row: 4
            Layout.column: 0
            Layout.alignment: Qt.AlignRight
            text: i18n("Show date:")
        }

        QtControls.CheckBox {
            Layout.row: 4
            Layout.column: 1
            id: showDateCheckBox
        }
    }

    Item { // tighten layout
        Layout.fillHeight: true
    }
}
