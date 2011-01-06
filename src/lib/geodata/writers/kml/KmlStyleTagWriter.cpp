//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "KmlStyleTagWriter.h"

#include "GeoDataStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerStyle(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataStyleType,
                                 kml::kmlTag_nameSpace22 ),
    new KmlStyleTagWriter );

bool KmlStyleTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataStyle *style = static_cast<const GeoDataStyle*>( node );

    writer.writeStartElement( kml::kmlTag_Style );
    writer.writeAttribute( "id", style->styleId() );

    writeElement( &style->iconStyle(), writer );
    writeElement( &style->labelStyle(), writer );
    writeElement( &style->lineStyle(), writer );
    writeElement( &style->polyStyle(), writer );

    writer.writeEndElement();

    return true;
}

}
