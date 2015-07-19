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
#include "OsmObjectManager.h"
#include "OsmTagTagWriter.h"
#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"
#include "osm/OsmPlacemarkData.h"

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
        QString ndId = QString::number( osmData.reference( *it ).id() );
        writer.writeStartElement( osm::osmTag_nd );
        writer.writeAttribute( "ref", ndId );
        writer.writeEndElement();
    }

    writer.writeEndElement();
}

}
