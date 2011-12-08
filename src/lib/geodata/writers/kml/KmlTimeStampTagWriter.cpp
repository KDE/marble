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

#include <QtCore/QDateTime>

namespace Marble
{

static GeoTagWriterRegistrar s_writerTimeStamp( GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataTimeStampType,
                                                                            kml::kmlTag_nameSpace22),
                                               new KmlTimeStampTagWriter() );


bool KmlTimeStampTagWriter::write( const GeoNode *node,
                               GeoWriter& writer ) const
{
    const GeoDataTimeStamp *timestamp = static_cast<const GeoDataTimeStamp*>(node);

    if( timestamp->when().isValid() )
    {
        writer.writeStartElement( kml::kmlTag_TimeStamp );

        writer.writeStartElement( kml::kmlTag_when);

        QString timestampString;

        timestampString = timestamp->when().toString(Qt::ISODate);
        writer.writeCharacters( timestampString );

        writer.writeEndElement();
        writer.writeEndElement();
    }
    return true;
}

}
