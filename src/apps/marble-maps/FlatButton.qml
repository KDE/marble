//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3

Button {
    id: root

    property string imageSource

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    style: ButtonStyle {
        background: Item {
            anchors.fill: parent
            Image {
                id: icon
                anchors.fill: parent
                source: root.imageSource
                smooth: true
                sourceSize.width: width
                sourceSize.height: height
            }
        }
    }
}
