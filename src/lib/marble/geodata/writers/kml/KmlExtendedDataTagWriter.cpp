//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
//


#include "KmlExtendedDataTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "GeoDataSchemaData.h"

#include <QHash>

namespace Marble
{

static GeoTagWriterRegistrar s_writerExtendedData( GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataExtendedDataType,
                                                                            kml::kmlTag_nameSpaceOgc22 ),
                                               new KmlExtendedDataTagWriter() );


bool KmlExtendedDataTagWriter::write( const GeoNode *node,
                               GeoWriter& writer ) const
{
    const GeoDataExtendedData *extended = static_cast<const GeoDataExtendedData*>( node );

    writer.writeStartElement( kml::kmlTag_ExtendedData );
    
    QHash< QString, GeoDataData >::const_iterator begin = extended->constBegin();
    QHash< QString, GeoDataData >::const_iterator end = extended->constEnd();

    for( QHash< QString, GeoDataData >::const_iterator i = begin; i != end; ++i ){
        writeElement( &i.value(), writer );
    }

    foreach( const GeoDataSchemaData &schemaData, extended->schemaDataList() ) {
        writeElement( &schemaData, writer );
    }

    writer.writeEndElement();

    return true;
}

}
