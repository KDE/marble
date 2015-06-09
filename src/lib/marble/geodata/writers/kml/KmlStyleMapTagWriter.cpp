//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlStyleMapTagWriter.h"

#include "GeoDataStyleMap.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerStyleMap(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataStyleMapType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlStyleMapTagWriter );

bool KmlStyleMapTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataStyleMap *map = static_cast<const GeoDataStyleMap*>( node );

    writer.writeStartElement( kml::kmlTag_StyleMap );
    KmlObjectTagWriter::writeIdentifiers( writer, map );

    QMapIterator<QString, QString> iter( *map );
    while ( iter.hasNext() ) {
        iter.next();
        writer.writeStartElement( kml::kmlTag_Pair );
        writer.writeElement( kml::kmlTag_key, iter.key() );
        writer.writeElement( kml::kmlTag_styleUrl, iter.value() );
        writer.writeEndElement();
    }

    writer.writeEndElement();

    return true;
}

}
