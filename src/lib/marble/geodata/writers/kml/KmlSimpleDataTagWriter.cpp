// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include "KmlSimpleDataTagWriter.h"
#include "KmlElementDictionary.h"

#include "GeoDataSimpleData.h"

#include "GeoDataTypes.h"

#include "GeoWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSimpleData(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataSimpleDataType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlSimpleDataTagWriter );

bool KmlSimpleDataTagWriter::write( const GeoNode *node, GeoWriter &writer ) const
{
    const GeoDataSimpleData *simpleData = static_cast<const GeoDataSimpleData*>( node );
    writer.writeStartElement( kml::kmlTag_SimpleData );
    writer.writeAttribute( "name", simpleData->name() );
    writer.writeCharacters( simpleData->data() );
    writer.writeEndElement();

    return true;
}

}
