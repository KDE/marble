// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include "KmlSchemaDataTagWriter.h"

#include <QString>

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

    for( const GeoDataSimpleData& data: schemaData->simpleDataList() ) {
        writeElement( &data, writer );
    }
    writer.writeEndElement();

    return true;
}

}
