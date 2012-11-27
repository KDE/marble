//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "KmlColorStyleTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoDataColorStyle.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

KmlColorStyleTagWriter::KmlColorStyleTagWriter(const QString &elementName)
    : m_elementName( elementName )
{
    // nothing to do
}

bool KmlColorStyleTagWriter::write( const Marble::GeoNode *node, GeoWriter &writer ) const
{
    writer.writeStartElement( m_elementName );

    GeoDataColorStyle const *colorStyle = static_cast<const GeoDataColorStyle*>(node);
    QColor const color = colorStyle->color();

    QChar const fill = QChar( '0' );
    QString colorString = QString( "%1%2%3%4" )
                             .arg( color.alpha(), 2, 16, fill )
                             .arg( color.blue(), 2, 16, fill )
                             .arg( color.green(), 2, 16, fill )
                             .arg( color.red(), 2, 16, fill );
    if ( colorString != "ffffffff" ) { // Only write non-default values
        writer.writeElement( kml::kmlTag_color, colorString );
    }

    switch( colorStyle->colorMode() ) {
    case GeoDataColorStyle::Random:
        writer.writeElement( kml::kmlTag_colorMode, "random" );
        break;
    case GeoDataColorStyle::Normal:
        // default value, no need to write it
        break;
    }

    bool const result = writeMid( node, writer );
    writer.writeEndElement();
    return result;
}

}
