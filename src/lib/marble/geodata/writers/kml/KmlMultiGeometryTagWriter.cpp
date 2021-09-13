// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlMultiGeometryTagWriter.h"

#include "GeoDataMultiGeometry.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerMultiGeometry(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataMultiGeometryType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlMultiGeometryTagWriter );

bool KmlMultiGeometryTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataMultiGeometry *geometry = static_cast<const GeoDataMultiGeometry*>( node );

    writer.writeStartElement( kml::kmlTag_MultiGeometry );
    KmlObjectTagWriter::writeIdentifiers( writer, geometry );

    for ( int i = 0; i < geometry->size(); ++i )
    {
        writeElement( &geometry->at( i ), writer );
    }

    writer.writeEndElement();

    return true;
}

}
