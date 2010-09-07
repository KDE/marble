//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
//


#include "KmlDataTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "GeoDataData.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerData( GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataDataType,
                                                                            kml::kmlTag_nameSpace22 ),
                                               new KmlDataTagWriter() );


bool KmlDataTagWriter::write( const GeoDataObject &node,
                               GeoWriter& writer ) const
{
    const GeoDataData &data = static_cast<const GeoDataData&>( node );

    writer.writeStartElement( kml::kmlTag_Data );
    writer.writeAttribute( "name", data.displayName() );
    writer.writeElement( "value", data.value().toString() );
    writer.writeEndElement();

    return true;
}

}
