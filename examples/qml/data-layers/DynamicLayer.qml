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

    center: Coordinate { longitude: 142.2; latitude: 11.35 }

    dataLayers: [
        DataLayer{
            id: layer
            // The model defines the data that will appear. The lon and lat
            // property of its items define their position
            // See EarthquakesModel.qml for details
            model: EarthquakesModel {
                id: earthquakes
            }

            // The delegate is the component that shows the items from the
            // model on top of the map. Their position and visibility is managed
            // automatically by Marble
            delegate: Rectangle {
                width: magnitude * 10;
                height: width
                radius: width / 2;
                color: magnitude < 5.0 ? "green" : ( magnitude < 6.0 ? "orange" : "red" )
                opacity: 0.67

                Text {
                    anchors.centerIn: parent
                    font.bold: true
                    text: magnitude
                }
            }

            // Marble informs us with this signal that new data is needed for the given
            // bounding box (north, south, east, west, each in degree). We retrieve new
            // data from our model (which calls geonames.org) in that case
            onDataRequest: {
                earthquakes.north = north
                earthquakes.south = south
                earthquakes.east = east
                earthquakes.west = west
                earthquakes.update()
            }
        }
    ]

    /** @todo FIXME: Currently we update our model here, but ideally Marble
     * detects the arrival of new data by itself in the future and this won't be
     * needed anymore then.
     */
    Connections {
        target: earthquakes
        onStatusChanged: {
            if ( earthquakes.status == XmlListModel.Ready ) {
                layer.model = earthquakes
            }
        }
    }
}
