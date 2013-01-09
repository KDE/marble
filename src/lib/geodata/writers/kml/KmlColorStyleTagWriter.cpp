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
    writer.writeOptionalElement( kml::kmlTag_color, formatColor( colorStyle->color() ), "ffffffff" );
    QString const colorMode = colorStyle->colorMode() == GeoDataColorStyle::Random ? "random" : "normal";
    writer.writeOptionalElement( kml::kmlTag_colorMode, colorMode, "normal" );

    bool const result = writeMid( node, writer );
    writer.writeEndElement();
    return result;
}

QString KmlColorStyleTagWriter::formatColor( const QColor &color )
{
    QChar const fill = QChar( '0' );
    return QString( "%1%2%3%4" )
                 .arg( color.alpha(), 2, 16, fill )
                 .arg( color.blue(), 2, 16, fill )
                 .arg( color.green(), 2, 16, fill )
                 .arg( color.red(), 2, 16, fill );
}

}
