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

#include "KmlLineStringTagWriter.h"

#include "GeoDataLineString.h"
#include "GeoDataTypes.h"
#include "GeoDataCoordinates.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLookAt(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataLineStringType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlLineStringTagWriter );

bool KmlLineStringTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataLineString *lineString = static_cast<const GeoDataLineString*>( node );

    if ( lineString->size() > 1 )
    {
        writer.writeStartElement( kml::kmlTag_LineString );
        KmlObjectTagWriter::writeIdentifiers( writer, lineString );
        writer.writeOptionalElement( kml::kmlTag_extrude, QString::number( lineString->extrude() ), "0" );
        writer.writeOptionalElement( kml::kmlTag_tessellate, QString::number( lineString->tessellate() ), "0" );
        writer.writeStartElement( "coordinates" );

        // Write altitude for *all* elements, if *any* element
        // has altitude information (!= 0.0)
        bool hasAltitude = false;
        for ( int i = 0; i < lineString->size(); ++i ) {
            if ( lineString->at( i ).altitude() ) {
                hasAltitude = true;
                break;
            }
        }

        for ( int i = 0; i < lineString->size(); ++i ) {
            GeoDataCoordinates coordinates = lineString->at( i );
            if ( i > 0 )
            {
                writer.writeCharacters( " " );
            }

            qreal lon = coordinates.longitude( GeoDataCoordinates::Degree );
            writer.writeCharacters( QString::number( lon, 'f', 10 ) );
            writer.writeCharacters( "," );
            qreal lat = coordinates.latitude( GeoDataCoordinates::Degree );
            writer.writeCharacters( QString::number( lat, 'f', 10 ) );

            if ( hasAltitude ) {
                qreal alt = coordinates.altitude();
                writer.writeCharacters( "," );
                writer.writeCharacters( QString::number( alt, 'f', 2 ) );
            }
        }

        writer.writeEndElement();
        writer.writeEndElement();

        return true;
    }

    return false;
}

}
