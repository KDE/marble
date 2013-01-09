//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Thibaut Gridel <tgridel@free.fr>
//

#include "KmlMultiTrackTagWriter.h"

#include "GeoDataMultiTrack.h"
#include "GeoDataTrack.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerMultiTrack(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataMultiTrackType,
                                 kml::kmlTag_nameSpace22 ),
    new KmlMultiTrackTagWriter );

bool KmlMultiTrackTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataMultiTrack *geometry = static_cast<const GeoDataMultiTrack*>( node );

    writer.writeStartElement( "gx:MultiTrack" );

    for ( int i = 0; i < geometry->size(); ++i )
    {
        writeElement( &geometry->at( i ), writer );
    }

    writer.writeEndElement();

    return true;
}

}
