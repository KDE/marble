//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Window 2.2

RadioButton {
    id: root
    property string imageSource

    SystemPalette{
        id: palette
        colorGroup: SystemPalette.Active
    }

    style: RadioButtonStyle {
        indicator: Rectangle {
            color: control.checked ? palette.highlight : palette.base
            width: image.width
            height: image.height + Screen.pixelDensity * 1
            radius: Screen.pixelDensity * 1

            Image {
                anchors.centerIn: parent
                id: image
                sourceSize.height: Screen.pixelDensity * 5
                fillMode: Image.PreserveAspectFit
                source: root.imageSource
            }
        }
    }
}
