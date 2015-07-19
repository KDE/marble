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
#include "OsmObjectManager.h"
#include "OsmTagTagWriter.h"
#include "GeoDataPoint.h"
#include "GeoDataLineString.h"
#include "osm/OsmPlacemarkData.h"

namespace Marble
{


void OsmNodeTagWriter::writeNode( const GeoDataCoordinates& coordinates, const OsmPlacemarkData& osmData, GeoWriter& writer )
{
    QString lat = QString::number( coordinates.latitude( GeoDataCoordinates::Degree ), 'f', 10 );
    QString lon = QString::number( coordinates.longitude( GeoDataCoordinates::Degree ), 'f', 10 );

    writer.writeStartElement( osm::osmTag_node );

    writer.writeAttribute( "lat", lat );
    writer.writeAttribute( "lon", lon );
    OsmObjectAttributeWriter::writeAttributes( osmData, writer );
    OsmTagTagWriter::writeTags( osmData, writer );

    writer.writeEndElement();
}

void OsmNodeTagWriter::writeAllNodes( const OsmPlacemarkData& osmData, GeoWriter& writer )
{
    QHash< GeoDataCoordinates, OsmPlacemarkData >::const_iterator it = osmData.nodeReferencesBegin();
    QHash< GeoDataCoordinates, OsmPlacemarkData >::const_iterator end = osmData.nodeReferencesEnd();

    // Writing all the component nodes
    for ( ; it != end; ++it ) {
        writeNode( it.key(), it.value(), writer );
    }
}

}
