// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

import QtQuick
import QtQuick.Controls

ToolButton {
    id: root

    // Can be 'departure', 'waypoint', 'destination', 'searchResult'
    property string type

    icon.name: 'add-placemark-symbolic'
}
