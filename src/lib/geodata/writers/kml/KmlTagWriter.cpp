//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "KmlTagWriter.h"

#include "GeoWriter.h"
#include "KmlElementDictionary.h"


namespace Marble
{

static GeoTagWriterRegistrar s_writerKml( GeoTagWriter::QualifiedName( "",
                                                                       kml::kmlTag_nameSpace22),
                                               new KmlTagWriter() );


bool KmlTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    Q_UNUSED(node);
    writer.writeStartElement( "kml" );
    writer.writeAttribute( "xmlns", kml::kmlTag_nameSpace22 );
    writer.writeAttribute( "xmlns:gx", kml::kmlTag_nameSpaceGx22 );

    // Do not write an end element for document handlers
    return true;
}

}
