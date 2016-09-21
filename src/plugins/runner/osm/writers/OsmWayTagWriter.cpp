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
#include "OsmWayTagWriter.h"

//Marble
#include "OsmElementDictionary.h"
#include "OsmObjectAttributeWriter.h"
#include "OsmTagTagWriter.h"
#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"
#include "osm/OsmPlacemarkData.h"
#include "osm/OsmObjectManager.h"
#include "GeoWriter.h"


namespace Marble
{

void OsmWayTagWriter::writeWay( const GeoDataLineString& lineString,
                                const OsmPlacemarkData& osmData, GeoWriter& writer )
{

    writer.writeStartElement( osm::osmTag_way );

    OsmObjectAttributeWriter::writeAttributes( osmData, writer );
    OsmTagTagWriter::writeTags( osmData, writer );

    // Writing all the component nodes ( Nd tags )
    QVector<GeoDataCoordinates>::const_iterator it =  lineString.constBegin();
    QVector<GeoDataCoordinates>::ConstIterator const end = lineString.constEnd();

    for ( ; it != end; ++it ) {
        QString ndId = QString::number( osmData.nodeReference( *it ).id() );
        writer.writeStartElement( osm::osmTag_nd );
        writer.writeAttribute( "ref", ndId );
        writer.writeEndElement();
    }

    if (!lineString.isEmpty() && lineString.isClosed()) {
        auto const startId = osmData.nodeReference(lineString.first()).id();
        auto const endId = osmData.nodeReference(lineString.last()).id();
        if (startId != endId) {
            writer.writeStartElement( osm::osmTag_nd );
            writer.writeAttribute( "ref", QString::number(startId));
            writer.writeEndElement();
        }
    }

    writer.writeEndElement();
}

}
