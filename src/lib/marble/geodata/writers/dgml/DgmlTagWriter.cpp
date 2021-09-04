//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Utku Aydın <utkuaydin34@gmail.com>
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
