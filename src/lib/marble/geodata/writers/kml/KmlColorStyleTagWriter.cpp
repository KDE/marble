//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlColorStyleTagWriter.h"

#include "GeoDataTypes.h"
#include "GeoDataColorStyle.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagWriter.h"

namespace Marble
{

KmlColorStyleTagWriter::KmlColorStyleTagWriter(const QString &elementName)
    : m_elementName( elementName )
{
    // nothing to do
}

bool KmlColorStyleTagWriter::write( const Marble::GeoNode *node, GeoWriter &writer ) const
{
    GeoDataColorStyle const *colorStyle = static_cast<const GeoDataColorStyle*>(node);

    if ( colorStyle->id().isEmpty() &&
         colorStyle->targetId().isEmpty() &&
         colorStyle->color() == defaultColor() &&
         colorStyle->colorMode() == GeoDataColorStyle::Normal &&
         isEmpty( node ) ) {
        return true;
    }

    writer.writeStartElement( m_elementName );

    KmlObjectTagWriter::writeIdentifiers( writer, colorStyle);
    writer.writeOptionalElement( kml::kmlTag_color, formatColor( colorStyle->color() ), formatColor( defaultColor() ) );
    QString const colorMode = colorStyle->colorMode() == GeoDataColorStyle::Random ? "random" : "normal";
    writer.writeOptionalElement( kml::kmlTag_colorMode, colorMode, "normal" );

    bool const result = writeMid( node, writer );
    writer.writeEndElement();
    return result;
}

QString KmlColorStyleTagWriter::formatColor( const QColor &color )
{
    QChar const fill = QLatin1Char('0');
    return QString( "%1%2%3%4" )
                 .arg( color.alpha(), 2, 16, fill )
                 .arg( color.blue(), 2, 16, fill )
                 .arg( color.green(), 2, 16, fill )
                 .arg( color.red(), 2, 16, fill );
}

QColor KmlColorStyleTagWriter::defaultColor() const
{
    return QColor( Qt::white );
}

}
