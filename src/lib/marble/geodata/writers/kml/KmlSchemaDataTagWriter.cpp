//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include <QString>

#include "KmlSchemaDataTagWriter.h"
#include "KmlElementDictionary.h"

#include "GeoWriter.h"

#include "GeoDataTypes.h"

#include "GeoDataSchemaData.h"
#include "GeoDataSimpleData.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSchemaData(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataSchemaDataType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlSchemaDataTagWriter );

bool KmlSchemaDataTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataSchemaData *schemaData = static_cast<const GeoDataSchemaData*>( node );
    writer.writeStartElement( kml::kmlTag_SchemaData );
    QString schemaUrl = schemaData->schemaUrl();
    writer.writeAttribute( "schemaUrl", schemaUrl );

    foreach( const GeoDataSimpleData& data, schemaData->simpleDataList() ) {
        writeElement( &data, writer );
    }
    writer.writeEndElement();

    return true;
}

}
