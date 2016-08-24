//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#include "OsmTagWriter.h"

#include "GeoWriter.h"
#include "OsmElementDictionary.h"
#include "MarbleGlobal.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerOsm( GeoTagWriter::QualifiedName( "",
                                                                       osm::osmTag_version06 ),
                                               new OsmTagWriter() );


bool OsmTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    Q_UNUSED( node );

    writer.writeStartElement( "osm" );
    writer.writeAttribute( "version", osm::osmTag_version06 );
    writer.writeAttribute("generator", QLatin1String("Marble ") + Marble::MARBLE_VERSION_STRING);

    return true;
}

}

