// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlStyleTagWriter.h"

#include "GeoDataStyle.h"
#include "GeoDataBalloonStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataListStyle.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerStyle(
    GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataStyleType,
                                 kml::kmlTag_nameSpaceOgc22 ),
    new KmlStyleTagWriter );

bool KmlStyleTagWriter::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataStyle *style = static_cast<const GeoDataStyle*>( node );

    writer.writeStartElement( kml::kmlTag_Style );
    KmlObjectTagWriter::writeIdentifiers( writer, style );

    writeElement( &style->iconStyle(), writer );
    writeElement( &style->labelStyle(), writer );
    writeElement( &style->lineStyle(), writer );
    writeElement( &style->polyStyle(), writer );
    writeElement( &style->balloonStyle(), writer );
    writeElement( &style->listStyle(), writer );

    writer.writeEndElement();

    return true;
}

}
