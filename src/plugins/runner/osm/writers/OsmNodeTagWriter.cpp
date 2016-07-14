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
#include "osm/OsmPlacemarkData.h"
#include "osm/OsmObjectManager.h"


namespace Marble
{


void OsmNodeTagWriter::writeNode( const GeoDataCoordinates& coordinates, const OsmPlacemarkData& osmData, GeoWriter& writer )
{
    QString lat = QString::number( coordinates.latitude( GeoDataCoordinates::Degree ), 'f', 7 );
    QString lon = QString::number( coordinates.longitude( GeoDataCoordinates::Degree ), 'f', 7 );

    writer.writeStartElement( osm::osmTag_node );

    writer.writeAttribute( "lat", lat );
    writer.writeAttribute( "lon", lon );
    OsmObjectAttributeWriter::writeAttributes( osmData, writer );
    OsmTagTagWriter::writeTags( osmData, writer );

    writer.writeEndElement();
}

void OsmNodeTagWriter::writeAllNodes( const QList<OsmPlacemarkData>& values, GeoWriter& writer )
{
    typedef QPair<GeoDataCoordinates, OsmPlacemarkData> Coordinate;
    QVector<Coordinate> nodes;

    foreach(const OsmPlacemarkData &osmData, values) {
        QHash< GeoDataCoordinates, OsmPlacemarkData >::const_iterator it = osmData.nodeReferencesBegin();
        QHash< GeoDataCoordinates, OsmPlacemarkData >::const_iterator end = osmData.nodeReferencesEnd();
        for (; it != end; ++it) {
            nodes.push_back(Coordinate(it.key(), it.value()));
        }
    }

    // Sort by id ascending since some external tools rely on that
    qSort(nodes.begin(), nodes.end(), [] (const Coordinate &a, const Coordinate &b) { return a.second.id() < b.second.id(); });

    // Writing all the component nodes
    qint64 lastId = 0;
    foreach(const auto &node, nodes) {
        if (node.second.id() != lastId) {
            writeNode(node.first, node.second, writer);
            lastId = node.second.id();
        } // else duplicate/shared node
    }
}

}
