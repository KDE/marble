//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "KmlMultiGeometryTagWriter.h"

#include "GeoDataMultiGeometry.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerMultiGeometry(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataMultiGeometryType,
                                 kml::kmlTag_nameSpace22 ),
    new KmlMultiGeometryTagWriter );

bool KmlMultiGeometryTagWriter::write( const GeoDataObject &node, GeoWriter& writer ) const
{
    const GeoDataMultiGeometry &geometry = static_cast<const GeoDataMultiGeometry&>( node );

    writer.writeStartElement( kml::kmlTag_MultiGeometry );

    for ( int i = 0; i < geometry.size(); ++i )
    {
        writeElement( geometry.at( i ), writer );
    }

    writer.writeEndElement();

    return true;
}

}
