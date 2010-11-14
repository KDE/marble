//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "KmlLinearRingTagWriter.h"

#include "GeoDataLinearRing.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataLinearRingType,
                                 kml::kmlTag_nameSpace22 ),
    new KmlLinearRingTagWriter );

bool KmlLinearRingTagWriter::write( const GeoDataObject &node, GeoWriter& writer ) const
{
    const GeoDataLinearRing &ring = static_cast<const GeoDataLinearRing&>( node );

    if ( ring.size() > 1 )
    {
        writer.writeStartElement( kml::kmlTag_LinearRing );
        writer.writeStartElement( "coordinates" );

        for ( int i = 0; i < ring.size(); ++i )
        {
            GeoDataCoordinates coordinates = ring.at( i );
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
