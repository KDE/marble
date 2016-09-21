//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "KmlTrackWriter.h"

#include "GeoDataCoordinates.h"
#include "GeoDataTrack.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

#include <QDateTime>

using namespace Marble;

static GeoTagWriterRegistrar s_writerPoint( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataTrackType,
                                                                            kml::kmlTag_nameSpaceOgc22),
                                               new KmlTrackWriter() );

bool KmlTrackWriter::write( const GeoNode *node, GeoWriter &writer ) const
{
    const GeoDataTrack *track = static_cast<const GeoDataTrack *>( node );

    writer.writeStartElement( "gx:Track" );
    KmlObjectTagWriter::writeIdentifiers( writer, track );

    int points = track->size();
    for ( int i = 0; i < points; i++ ) {
        writer.writeElement( "when", track->whenList().at( i ).toString( Qt::ISODate ) );

        qreal lon, lat, alt;
        track->coordinatesList().at( i ).geoCoordinates( lon, lat, alt, GeoDataCoordinates::Degree );
        const QString coord = QString::number(lon, 'f', 10) + QLatin1Char(' ') +
                              QString::number(lat, 'f', 10) + QLatin1Char(' ') +
                              QString::number(alt, 'f', 10);

        writer.writeElement( "gx:coord", coord );
    }
    writer.writeEndElement();

    return true;
}
