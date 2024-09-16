// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick
import QtQuick.Controls
import QtQuick.Window

RadioButton {
    id: root
    property string imageSource

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }
}
