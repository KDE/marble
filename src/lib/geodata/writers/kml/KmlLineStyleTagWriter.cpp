//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "KmlLineStyleTagWriter.h"

#include "GeoDataLineStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerLineStyle(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataLineStyleType,
                                 kml::kmlTag_nameSpace22 ),
    new KmlLineStyleTagWriter );

KmlLineStyleTagWriter::KmlLineStyleTagWriter() : KmlColorStyleTagWriter( kml::kmlTag_LineStyle )
{
    // nothing to do
}

bool KmlLineStyleTagWriter::writeMid( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataLineStyle *style = static_cast<const GeoDataLineStyle*>( node );

    writer.writeElement( "width", QString::number( style->width() ) );

    return true;
}

}
