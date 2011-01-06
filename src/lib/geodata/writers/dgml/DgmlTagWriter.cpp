//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "DgmlTagWriter.h"

#include "GeoWriter.h"
#include "DgmlElementDictionary.h"


namespace Marble
{

static GeoTagWriterRegistrar s_writerDgml( GeoTagWriter::QualifiedName( "", dgml::dgmlTag_nameSpace20 ), new DgmlTagWriter() );


bool DgmlTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    Q_UNUSED(node);
    writer.writeStartElement( "dgml" );
    writer.writeAttribute( "xmlns", dgml::dgmlTag_nameSpace20 );

    // Do not write an end element for document handlers
    return true;
}

}
