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

bool KmlLineStyleTagWriter::write( const GeoDataObject &node, GeoWriter& writer ) const
{
    const GeoDataLineStyle &style = static_cast<const GeoDataLineStyle&>( node );

    writer.writeStartElement( kml::kmlTag_LineStyle );

    QColor color = style.color();
    QChar fill = QChar( '0' );
    QString colorString = QString( "%1%2%3%4" )
                             .arg( color.alpha(), 2, 16, fill )
                             .arg( color.blue(), 2, 16, fill )
                             .arg( color.green(), 2, 16, fill )
                             .arg( color.red(), 2, 16, fill );
    writer.writeElement( "color", colorString );
    writer.writeElement( "width", QString::number( style.width() ) );

    writer.writeEndElement();

    return true;
}

}
