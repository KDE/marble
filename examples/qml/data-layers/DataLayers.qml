// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>

import QtQuick 1.1
import org.kde.edu.marble 0.11

MarbleWidget {
    width: 800;
    height: 600

    center: Coordinate { longitude: 30; latitude: 33 }

    dataLayers: [
        DataLayer{
            // The model defines the data that will appear. The lon and lat
            // property of its items define their position
            // See SevenWondersOfTheAncientWorld.qml for details
            model: SevenWondersOfTheAncientWorld {}

            // The delegate is the component that shows the items from the
            // model on top of the map. Their position and visibility is managed
            // automatically by Marble
            delegate: Rectangle {
                width: 100;
                height: image.height + label.height + 12
                border.color: "darkgray"; border.width: 2
                radius: 4; smooth: true
                Column {
                    id: column; x: 4; y: 4; spacing: 4
                    Image {
                        id: image
                        width: 92; smooth: true
                        fillMode: Image.PreserveAspectFit
                        // "picture" is a property defined by items in the model
                        source: picture
                    }
                    Text {
                        id: label
                        // "name" is a property defined by items in the model
                        text: name
                        width: 92
                        wrapMode: Text.Wrap
                        font.bold: true
                    }
                }
            }
        }
    ]
}
