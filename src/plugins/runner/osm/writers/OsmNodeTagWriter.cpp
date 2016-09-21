//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

//Self
#include "OsmNodeTagWriter.h"

//Qt
#include <QHash>

//Marble
#include "OsmElementDictionary.h"
#include "OsmObjectAttributeWriter.h"
#include "OsmTagTagWriter.h"
#include "GeoDataPoint.h"
#include "GeoDataLineString.h"
#include "GeoWriter.h"
#include "osm/OsmPlacemarkData.h"
#include "osm/OsmObjectManager.h"

namespace Marble
{


void OsmNodeTagWriter::writeNode( const OsmConverter::Node &node, GeoWriter& writer )
{
    QString lat = QString::number( node.first.latitude( GeoDataCoordinates::Degree ), 'f', 7 );
    QString lon = QString::number( node.first.longitude( GeoDataCoordinates::Degree ), 'f', 7 );

    writer.writeStartElement( osm::osmTag_node );

    writer.writeAttribute( "lat", lat );
    writer.writeAttribute( "lon", lon );
    OsmObjectAttributeWriter::writeAttributes( node.second, writer );
    OsmTagTagWriter::writeTags(node.second, writer);

    writer.writeEndElement();
}

void OsmNodeTagWriter::writeAllNodes( const OsmConverter::Nodes& nodes, GeoWriter& writer )
{
    // Writing all the component nodes
    qint64 lastId = 0;
    foreach(const auto &node, nodes) {
        if (node.second.id() != lastId) {
            writeNode(node, writer);
            lastId = node.second.id();
        } // else duplicate/shared node
    }
}

}
