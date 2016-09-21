//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2014      Marek Hakala <hakala.marek@gmail.com>
//

#include "KmlLinearRingTagWriter.h"

#include "GeoDataLinearRing.h"
#include "GeoDataTypes.h"
#include "GeoDataCoordinates.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataLinearRingType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlLinearRingTagWriter );

bool KmlLinearRingTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataLinearRing *ring = static_cast<const GeoDataLinearRing*>( node );

    if ( ring->size() > 1 )
    {
        writer.writeStartElement( kml::kmlTag_LinearRing );
        KmlObjectTagWriter::writeIdentifiers( writer, ring );
        writer.writeOptionalElement( kml::kmlTag_extrude, QString::number( ring->extrude() ), "0" );
        writer.writeOptionalElement( kml::kmlTag_tessellate, QString::number( ring->tessellate() ), "0" );
        writer.writeStartElement( "coordinates" );

        int size = ring->size() >= 3 && ring->first() != ring->last() ? ring->size() + 1 : ring->size();

        for ( int i = 0; i < size; ++i )
        {
            GeoDataCoordinates coordinates = ring->at( i % ring->size() );
            if ( i > 0 )
            {
                writer.writeCharacters( " " );
            }

            qreal lon = coordinates.longitude( GeoDataCoordinates::Degree );
            writer.writeCharacters( QString::number( lon, 'f', 10 ) );
            writer.writeCharacters( "," );
            qreal lat = coordinates.latitude( GeoDataCoordinates::Degree );
            writer.writeCharacters( QString::number( lat, 'f', 10 ) );
        }

        writer.writeEndElement();
        writer.writeEndElement();

        return true;
    }

    return false;
}

}
