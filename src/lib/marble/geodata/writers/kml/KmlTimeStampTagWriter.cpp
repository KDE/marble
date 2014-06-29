//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011     Javier Becerra <javier@auva.es>
//

#include "KmlTimeStampTagWriter.h"

#include "GeoDataTimeStamp.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

#include <QDateTime>

namespace Marble
{

static GeoTagWriterRegistrar s_writerTimeStamp( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataTimeStampType,
                                                                            kml::kmlTag_nameSpaceOgc22),
                                               new KmlTimeStampTagWriter() );


bool KmlTimeStampTagWriter::write( const GeoNode *node,
                               GeoWriter& writer ) const
{
    const GeoDataTimeStamp *timestamp = static_cast<const GeoDataTimeStamp*>(node);

    if( timestamp->when().isValid() )
    {
        writer.writeStartElement( kml::kmlTag_TimeStamp );
        KmlObjectTagWriter::writeIdentifiers( writer, timestamp );

        writer.writeStartElement( kml::kmlTag_when);
        writer.writeCharacters( toString( *timestamp ) );
        writer.writeEndElement();

        writer.writeEndElement();
    }
    return true;
}

QString KmlTimeStampTagWriter::toString( const GeoDataTimeStamp &timestamp )
{
    switch ( timestamp.resolution() ) {
    case GeoDataTimeStamp::SecondResolution: return timestamp.when().toString( Qt::ISODate );
    case GeoDataTimeStamp::DayResolution:    return timestamp.when().toString( "yyyy-MM-dd" );
    case GeoDataTimeStamp::MonthResolution:  return timestamp.when().toString( "yyyy-MM" );
    case GeoDataTimeStamp::YearResolution:   return timestamp.when().toString( "yyyy" );
    }

    Q_ASSERT( false && "not reachable" );
    return QString();
}

}
