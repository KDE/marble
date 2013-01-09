//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "KmlOverlayTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoDataOverlay.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlColorStyleTagWriter.h"

namespace Marble
{

KmlOverlayTagWriter::KmlOverlayTagWriter(const QString &elementName)
    : m_elementName( elementName )
{
    // nothing to do
}

bool KmlOverlayTagWriter::write( const Marble::GeoNode *node, GeoWriter &writer ) const
{
    writer.writeStartElement( m_elementName );

    GeoDataOverlay const *overlay = static_cast<const GeoDataOverlay*>(node);
    QString const color = KmlColorStyleTagWriter::formatColor( overlay->color() );
    writer.writeOptionalElement( kml::kmlTag_color, color, "ffffffff" );
    QString const drawOrder = QString::number( overlay->drawOrder() );
    writer.writeOptionalElement( kml::kmlTag_drawOrder, drawOrder, "0" );

    if ( !overlay->iconFile().isEmpty() ) {
        writer.writeStartElement( kml::kmlTag_Icon );
        writer.writeStartElement( kml::kmlTag_href );
        writer.writeCharacters( overlay->iconFile() );
        writer.writeEndElement();
        writer.writeEndElement();
    }

    bool const result = writeMid( node, writer );
    writer.writeEndElement();
    return result;
}

}
