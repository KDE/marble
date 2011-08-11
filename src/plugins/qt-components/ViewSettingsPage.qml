// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>

import QtQuick 1.0
import com.nokia.meego 1.0

Page {
    id: viewSettings
    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin
    tools: commonToolBar

    ButtonColumn {
        Label {
            text: "Projection:"
        }
        RadioButton {
            id: equirectangularButton
            text: "Equirectangular"
            checked: text == settings.projection
            onClicked: { settings.projection = text }
        }
        RadioButton {
            id: mercatorButton
            text: "Mercator"
            checked: text == settings.projection
            onClicked: { settings.projection = text }
        }
        RadioButton {
            id: sphericalButton
            text: "Spherical"
            checked: text == settings.projection
            onClicked: { settings.projection = text }
        }
    }
    
}