//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Shou Ya <shouyatf@gmail.com>
// Copyright 2014      Marek Hakala <hakala.marek@gmail.com>
//

#include "KmlPolygonTagWriter.h"

#include "GeoDataPolygon.h"
#include "GeoDataLinearRing.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataPolygonType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlPolygonTagWriter);

bool KmlPolygonTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataPolygon *polygon = static_cast<const GeoDataPolygon*>( node );

    writer.writeStartElement( kml::kmlTag_Polygon );
    KmlObjectTagWriter::writeIdentifiers( writer, polygon );
    writer.writeOptionalElement( kml::kmlTag_extrude, QString::number( polygon->extrude() ), "0" );

    writer.writeStartElement( "outerBoundaryIs" );
    writeElement( &polygon->outerBoundary(), writer );
    writer.writeEndElement();

    const QVector<GeoDataLinearRing>& linearRings = polygon->innerBoundaries();
    if (linearRings.size() > 0) {
	writer.writeStartElement( "innerBoundaryIs" );
	for ( int i = 0; i < linearRings.size(); ++i ) {
	    const GeoDataLinearRing& ring = linearRings[i];
	    writeElement( &ring, writer );
	}
	writer.writeEndElement();
    }

    writer.writeEndElement();

    return true;

}

}

