//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include "KmlSchemaTagWriter.h"

#include "GeoDataSchema.h"
#include "GeoDataSimpleField.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSchema(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataSchemaType,
                                 kml::kmlTag_nameSpaceOgc22),
    new KmlSchemaTagWriter );

bool KmlSchemaTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataSchema *schema = static_cast<const GeoDataSchema*>( node );
    writer.writeStartElement( kml::kmlTag_Schema );
    KmlObjectTagWriter::writeIdentifiers( writer, schema );
    QString name = schema->schemaName();
    writer.writeAttribute( "name", name );

    foreach( const GeoDataSimpleField &simpleField, schema->simpleFields() ) {
        writeElement( &simpleField, writer );
    }

    writer.writeEndElement();

    return true;
}

}
